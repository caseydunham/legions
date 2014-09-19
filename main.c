//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////

// I N C L U D E S ///////////////////////////////////////////////////////////
#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include "2dx.h"
#include "keyx.h"

// D E F I N E S /////////////////////////////////////////////////////////////
#define FONT_WIDTH					4
#define FONT_HEIGHT					7
#define NUM_FONT						64

#define MISS_ACTIVE					1
#define MISS_INACTIVE				0

#define MAX_PLAYERS_MISSILES     5
#define MAX_MISSILES					12

#define MISS_SPEED					5
#define PLAYER_SPEED					5

#define PLAYER							1
#define ALIEN							2
#define MAX_ALIENS					1                 // This is max number of aliens per level
#define ALIEN_FRAMES					15

#define EXPLOSION_INACTIVE			0
#define EXPLOSION_ACTIVE			1
#define EXPLOSION_FRAMES			6
#define MAX_EXPLOSIONS				MAX_ALIENS

#define WORM_HOLE_FRAMES			8
#define HOLE_ACTIVE					1
#define HOLE_INACTIVE			   0
#define MAX_HOLES						MAX_ALIENS

#define PLAYER_MISS_COLOR			15
#define ALIEN_MISS_COLOR			4

#define PLAYER_BEGIN_X				160
#define PLAYER_BEGIN_Y				178

#define ALIVE							1
#define DEAD							0

#define SCORE_X						0
#define SCORE_Y						0

#define LIVES_X						0
#define LIVES_Y						20

#define GAME_OVER						1

// S T R U C T U R E S ////////////////////////////////////////////////////////

typedef struct object_typ
{
	int x,y;						// Screen position of missile
   int xv,yv;					// speed of missile
   int state;					// Inactive or Active
   int color;
   int back_color;
   int owner;              // either player or alien

} object, *object_ptr;

typedef struct player_typ
{

	int lives;
   long score;
   int active_missiles;

} player_info, *player_ptr;


// G L O B A L S //////////////////////////////////////////////////////////////

bitmap game_font[NUM_FONT];
pcx_picture image;

object missile[MAX_MISSILES];

sprite p_ship,
       a_ship[MAX_ALIENS],
       explosions[MAX_EXPLOSIONS],
       worm_hole[MAX_HOLES];

player_info player;

// P R O T O T Y P E S ///////////////////////////////////////////////////////
void ScaleTest(void);
void GameLoop(void);
void GameStartup(void);
void GameShutDown(void);
void TitleScreen(void);
void InitGame(void);
void LoadShips(void);
void InitWormHole(void);
void LoadWormHole(void);
void FontEngine(int x, int y, char *string, unsigned char far *destination);
void FontPrint(int x, int y, char *string,unsigned char far *destination);
void InitMissiles(void);
int StartMissiles(int x, int y,int yv,int color);
void DrawMissiles(void);
void EraseMissiles(void);
void UnderMissiles(void);
void MoveMissiles(void);
void UnderAliens(void);
void EraseAliens(void);
void DrawAliens(void);
void MoveAliens(void);
void InitAliens(void);
void DrawInfo(void);
void InitExplosions(void);
void LoadExplosions(void);
void StartExplosions(int x, int y, int speed);
void UnderExplosions(void);
void EraseExplosions(void);
void DrawExplosions(void);
void AnimateExplosions(void);

//////////////////////////////////////////////////////////////////////////////

void main()
{
   int index;

   GameStartup();

   //TitleScreen();

   LoadShips();

   //ScaleTest();

   LoadExplosions();

   LoadWormHole();

   InitGame();
   
	InitAliens();

   InitMissiles();

   InitExplosions();

   GameLoop();

   GameShutDown();

}
//////////////////////////////////////////////////////////////////////////////
void ScaleTest()
{
   int index;
   float scale=1;

   a_ship[0].x = 100;
   a_ship[0].y = 100;
   a_ship[0].counter_1 = 40;

   SpriteDraw((sprite_ptr)&a_ship[0], double_buffer,1);
   DisplayDoubleBuffer(double_buffer,0);

   for (index=0; index<a_ship[0].counter_1; index++)
   {
   	scale += .5;
      FillDoubleBuffer(0);
   	ScaleSprite((sprite_ptr)&a_ship[0], scale);
   	SpriteDraw((sprite_ptr)&a_ship[0], double_buffer,1);
      DisplayDoubleBuffer(double_buffer,0);
   }
   

   while(!keys_active){}
   GameShutDown();
}

void GameLoop()
{
   int new_x,
   	game_over=0;

   FillDoubleBuffer(0);

   SpriteUnder((sprite_ptr)&p_ship,double_buffer);
   UnderAliens();
	UnderMissiles();
   UnderExplosions();

	while(!game_over)
   {

   	SpriteErase((sprite_ptr)&p_ship,double_buffer);
      EraseAliens();
      EraseMissiles();
      EraseExplosions();

   	if (keys_active)
      {
      	if (keyboard_state[MAKE_ESC])
         	game_over=1;
         else
         if (keyboard_state[MAKE_RIGHT])
         {
         	new_x = p_ship.x + PLAYER_SPEED;

            if (new_x < 298)
            	p_ship.x = new_x;

         }
         else
         if (keyboard_state[MAKE_LEFT])
         {
         	new_x = p_ship.x - PLAYER_SPEED;

            if (new_x > 0)
            	p_ship.x = new_x;
         }
         else
         if (keyboard_state[MAKE_SPACE])
         {
         	StartMissiles(p_ship.x +11,p_ship.y+3,MISS_SPEED,PLAYER_MISS_COLOR);
            keyboard_state[MAKE_SPACE] = NULL;

         }
      }

      SpriteUnder((sprite_ptr)&p_ship, double_buffer);
      UnderAliens();
		UnderMissiles();
      UnderExplosions();

      MoveAliens();
      MoveMissiles();
      AnimateExplosions();

   	SpriteDraw((sprite_ptr)&p_ship, double_buffer,1);
      DrawAliens();
		DrawMissiles();
      DrawExplosions();
      
      DrawInfo();

      WaitRetrace();
      DisplayDoubleBuffer(double_buffer,0);

   }

} // end GameLoop //

//////////////////////////////////////////////////////////////////////////////

void GameStartup()
{

	SetGraphicsMode(GRAPHICS_MODE13);

   KeyboardInstallDriver();

   CreateDoubleBuffer(200);

   FontEngine(0,0,NULL,NULL);


} // end GameStartup //

//////////////////////////////////////////////////////////////////////////////

void GameShutDown()
{
	DeleteDoubleBuffer();

   KeyboardRemoveDriver();

   SetGraphicsMode(TEXT_MODE);

}  // end GameShutDown //

//////////////////////////////////////////////////////////////////////////////

void TitleScreen()
{
	//
   PCXInit((pcx_picture_ptr)&image);
   PCXLoad("title2.pcx", (pcx_picture_ptr)&image,1);
   PCXCopyToBuffer((pcx_picture_ptr)&image,double_buffer);
   DisplayDoubleBuffer(double_buffer,0);
   while(!keys_active){}

   FillDoubleBuffer(0);
   PCXDelete((pcx_picture_ptr)&image);

   //
}

//////////////////////////////////////////////////////////////////////////////

void InitGame()
{

	player.lives = 3;
   player.score = 0;

}  // end InitGame //

//////////////////////////////////////////////////////////////////////////////

void LoadShips()
{
   int a_index, index, y_index=1,x_index=0;

	SpriteInit((sprite_ptr)&p_ship,PLAYER_BEGIN_X, PLAYER_BEGIN_Y, 22,22, 0,0,0,0,0,0);

   PCXInit((pcx_picture_ptr)&image);
   PCXLoad("ships.pcx", (pcx_picture_ptr)&image,1);
	PCXGetSprite((pcx_picture_ptr)&image, (sprite_ptr)&p_ship, 0,0,0);

   for (a_index=0; a_index<MAX_ALIENS; a_index++)
   {
      SpriteInit((sprite_ptr)&a_ship[a_index],0,0,22,22,0,0,0,0,0,0);

      for (index=0; index < ALIEN_FRAMES; index++)
      {

			PCXGetSprite((pcx_picture_ptr)&image, (sprite_ptr)&a_ship[a_index],index,x_index,y_index);

         if (++x_index == 11)
         {
         	x_index = 0;
            y_index = 2;
         }

      }

   }

	PCXDelete((pcx_picture_ptr)&image);

} // end LoadShips //

///////////////////////////////////////////////////////////////////////////////

void InitWormHole()
{
	int index;

   for (index=0; index < MAX_HOLES; index++)
   	worm_hole[index].state = HOLE_INACTIVE;

} // end InitWormHole //

///////////////////////////////////////////////////////////////////////////////

void LoadWormHole()
{
	int index,w_index;

   PCXInit((pcx_picture_ptr)&image);
   PCXLoad("hole.pcx", (pcx_picture_ptr)&image,1);

   for (index=0; index < MAX_HOLES; index++)
   {
   	SpriteInit((sprite_ptr)&worm_hole[index],0,0,26,22,0,0,0,0,0,0);

      for (w_index=0; w_index < WORM_HOLE_FRAMES; w_index++)
      	PCXGetSprite((pcx_picture_ptr)&image, (sprite_ptr)&worm_hole[index],w_index, w_index,0);

   }

} // end LoadWormHole //

///////////////////////////////////////////////////////////////////////////////
void FontEngine(int x, int y, char *string, unsigned char far *destination)
{

	static int font_loaded=0;

   int index, c_index, length;

   if (!font_loaded)
   {
   	PCXInit((pcx_picture_ptr)&image);
      PCXLoad("font.pcx", (pcx_picture_ptr)&image,1);

      for (index=0; index<NUM_FONT; index++)
      {
      	BitmapAllocate((bitmap_ptr)&game_font[index], FONT_WIDTH,FONT_HEIGHT);

         game_font[index].width = FONT_WIDTH;
         game_font[index].height = FONT_HEIGHT;

         game_font[index].x = 1 + (index % 16) * (FONT_WIDTH +1);
         game_font[index].y = 1 + (index / 16) * (FONT_HEIGHT+1);

         BitmapGet((bitmap_ptr)&game_font[index],(unsigned char far *)image.buffer);

      }

      PCXDelete((pcx_picture_ptr)&image);
      font_loaded=1;
   }
   else
   {
   	length = strlen(string);

      for (index=0; index<length; index++)
      {
      	c_index = string[index] - ' ';

         game_font[c_index].y = y;
         game_font[c_index].x = x;

         BitmapPut((bitmap_ptr)&game_font[c_index],(unsigned char far *)destination,0);

         x+=(FONT_WIDTH+1);

         }
      }
} // end FontEngine //

///////////////////////////////////////////////////////////////////////////////

void FontPrint(int x, int y, char *string, unsigned char far *destination)
{
	int length, index, counter;

   char buffer[3];

   length = strlen(string);

   for (index=0; index<length; index++)
   {
   	buffer[0] = string[index];
      buffer[1] = '<';
      buffer[2] = 0;

      FontEngine(x,y,buffer,destination);

      x+=(FONT_WIDTH+1);

      WaitRetrace();

   }

   buffer[0] = ' ';
   buffer[1] = ' ';
   buffer[2] = 0;

   FontEngine(x,y,buffer,destination);

} // end FontPrint //

///////////////////////////////////////////////////////////////////////////////

void InitMissiles()
{
	int index;

   for (index=0;index < MAX_MISSILES; index++)
      missile[index].state = MISS_INACTIVE;

}  // end InitMissiles //

///////////////////////////////////////////////////////////////////////////////

int StartMissiles(int x, int y, int yv,int color)
{
	int index;

   for (index=0; index<MAX_MISSILES; index++)
   {
   	if (missile[index].state == MISS_INACTIVE)
      {
      	missile[index].state 		= MISS_ACTIVE;
         missile[index].x				= x;
         missile[index].y				= y;
         missile[index].yv				= yv;
         missile[index].color			= color;
         missile[index].back_color	= 0;



      	return(1);
      }

   }

   return(0);
} // end StartMissiles //

//////////////////////////////////////////////////////////////////////////////

void DrawMissiles(void)
{
	int index;

   for (index =0; index<MAX_MISSILES; index++)
   {
   	if (missile[index].state == MISS_ACTIVE)
      {
      	PutPixel(missile[index].x,missile[index].y,missile[index].color);
      }
   }
} // end DrawMissiles //

//////////////////////////////////////////////////////////////////////////////

void EraseMissiles(void)
{
	int index;

   for (index=0;index<MAX_MISSILES; index++)
   {
   	if (missile[index].state == MISS_ACTIVE)
      {
      	PutPixel(missile[index].x,missile[index].y,missile[index].back_color);
      }
   }

} // end EraseMissiles //

//////////////////////////////////////////////////////////////////////////////

void UnderMissiles(void)
{
	int index,mx,my;

   for (index=0; index<MAX_MISSILES; index++)
   {
   	if (missile[index].state == MISS_ACTIVE)
      {
      	missile[index].back_color = ReadPixel(missile[index].x, missile[index].y);
      }
   }

} // end UnderMissiles //

//////////////////////////////////////////////////////////////////////////////

void MoveMissiles(void)
{
	int index,a_index, miss_x, miss_y;

   for (index=0; index<MAX_MISSILES; index++)
   {
   	if (missile[index].state == MISS_ACTIVE)
      {

         	missile[index].y -= missile[index].yv;

            // check to see if missile has gone beyond top of screen
            if (missile[index].y < 0)
            	missile[index].state = MISS_INACTIVE;

            // check to see if missile has hit an alien
            for (a_index=0; a_index< MAX_ALIENS; a_index++)
            {
            	if (missile[index].x > a_ship[a_index].x && missile[index].x < a_ship[a_index].x+22 &&
               		missile[index].y > a_ship[a_index].y && missile[index].y < a_ship[a_index].y+22 &&
                     a_ship[a_index].state == ALIVE)
      			{
               	a_ship[a_index].state = DEAD;
                  SpriteErase((sprite_ptr)&a_ship[a_index], double_buffer);
                  StartExplosions(a_ship[a_index].x, a_ship[a_index].y, 5);
                  
               }
            }


      }

   }

      // update active_missiles

} // end MoveMissiles //

//////////////////////////////////////////////////////////////////////////////

void UnderAliens()
{
   int index;

   for (index=0; index<MAX_ALIENS; index++)
   {
      if (a_ship[index].state == ALIVE)
   		SpriteUnder((sprite_ptr)&a_ship[index], double_buffer);
   }
} // end UnderAliens //

//////////////////////////////////////////////////////////////////////////////

void EraseAliens()
{
	int index;

   for (index=0; index < MAX_ALIENS; index++)
   {
   	if (a_ship[index].state == ALIVE)
      	SpriteErase((sprite_ptr)&a_ship[index], double_buffer);
   }

} // end EraseAliens //

//////////////////////////////////////////////////////////////////////////////

void DrawAliens()
{
	int index;

   for (index=0; index < MAX_ALIENS; index++)
   {
   	if (a_ship[index].state == ALIVE)
      	SpriteDraw((sprite_ptr)&a_ship[index], double_buffer, 1);
   }

} // end DrawAliens //

//////////////////////////////////////////////////////////////////////////////

void MoveAliens()
{
	int index, direction;

   randomize();

   direction = (int)rand() % 4;

   for (index=0; index < MAX_ALIENS; index++)
   {


   }

} // end MoveAliens //

//////////////////////////////////////////////////////////////////////////////

void InitAliens()
{
   int index;

   randomize();

   for (index=0; index<MAX_ALIENS; index++)
   {
   	a_ship[index].state		= ALIVE;
      a_ship[index].x			= (int)rand() % 298;
      a_ship[index].y			= (int)rand() % 100;
      a_ship[index].curr_frame = 8;
   }

} // end InitAliens //

//////////////////////////////////////////////////////////////////////////////

void DrawInfo()
{
   char temp[9];

	FontEngine(SCORE_X, SCORE_Y, "SCORE", double_buffer);
   FontEngine(SCORE_X+40, SCORE_Y, ltoa(player.score, temp, 10),double_buffer);

   FontEngine(LIVES_X, LIVES_Y, "LIVES", double_buffer);
   FontEngine(LIVES_X + 40, LIVES_Y, itoa(player.lives, temp, 10),double_buffer);

} // end DrawInfo //

//////////////////////////////////////////////////////////////////////////////
void InitExplosions()
{
   int index;

	for (index=0; index < MAX_EXPLOSIONS; index++)
   	explosions[index].state = EXPLOSION_INACTIVE;

} // end InitExplosions //

//////////////////////////////////////////////////////////////////////////////

void LoadExplosions()
{
   int index,e_index;

	PCXInit((pcx_picture_ptr)&image);
   PCXLoad("galaxia.pcx",(pcx_picture_ptr)&image,1);



   for (e_index=0; e_index< MAX_EXPLOSIONS; e_index++)
   {
      SpriteInit((sprite_ptr)&explosions[e_index],0,0,28,22,0,0,0,0,0,0);

		for (index=0; index < EXPLOSION_FRAMES; index++)
   	{
   		PCXGetSprite((pcx_picture_ptr)&image,(sprite_ptr)&explosions[e_index],index,index,4);
   	}


   }


   PCXDelete((pcx_picture_ptr)&image);

} // end LoadExplosions //

//////////////////////////////////////////////////////////////////////////////

void StartExplosions(int x, int y, int speed)
{
	int index;

   for (index = 0; index < MAX_EXPLOSIONS; index++)
   {
   	if (explosions[index].state == EXPLOSION_INACTIVE)
      {
      	explosions[index].state			= EXPLOSION_ACTIVE;
         explosions[index].x				= x;
         explosions[index].y				= y;

         explosions[index].curr_frame  = 0;
         explosions[index].threshold_1 = speed;
         explosions[index].counter_1   = 0;

         break;
      }
   }

} // end StartExplosion //

//////////////////////////////////////////////////////////////////////////////

void UnderExplosions(void)
{
   static int first_time=0;
	int index;

   for (index=0; index<MAX_EXPLOSIONS; index++)
   {
      if (!first_time)
      {
      	SpriteUnder((sprite_ptr)&explosions[index],double_buffer);
         first_time=1;
      }
      else
   	if (explosions[index].state == EXPLOSION_ACTIVE)
      {
      	SpriteUnder((sprite_ptr)&explosions[index], double_buffer);
      }
   }
} // end UnderExplosions

///////////////////////////////////////////////////////////////////////////////

void EraseExplosions(void)
{
	int index;

   for (index=0; index < MAX_EXPLOSIONS; index++)
   {
   	if (explosions[index].state == EXPLOSION_ACTIVE)
      {
      	SpriteErase((sprite_ptr)&explosions[index], double_buffer);
      }
   }

} // end EraseExplosions //

/////////////////////////////////////////////////////////////////////////////

void DrawExplosions(void)
{
	int index;

   for (index = 0; index < MAX_EXPLOSIONS; index++)
   {
   	if (explosions[index].state == EXPLOSION_ACTIVE)
      {
      	SpriteDraw((sprite_ptr)&explosions[index], double_buffer,1);
      }
   }
} // end DrawExplosions //

/////////////////////////////////////////////////////////////////////////////

void AnimateExplosions(void)
{
	int index;

   for (index=0; index < MAX_EXPLOSIONS; index++)
   {
   	if (explosions[index].state == EXPLOSION_ACTIVE)
      {
      	if (++explosions[index].counter_1 >= explosions[index].threshold_1)
         {
         	if (++explosions[index].curr_frame == EXPLOSION_FRAMES)
            	explosions[index].state = EXPLOSION_INACTIVE;

            explosions[index].counter_1 = 0;

         }
      }
   }

} // end AnimateExplosions //

/////////////////////////////////////////////////////////////////////////////


