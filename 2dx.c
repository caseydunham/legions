////////////////////////////////////////////////////////////////////////////////
//		
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
//			I  N  C  L  U  D  E  S
//
///////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////
//
//			G  L  O  B  A  L  S
//
////////////////////////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (unsigned char far *)0xA0000000L;
unsigned char far *rom_char_set = (unsigned char far *)0xF000FA6EL;
unsigned char far *double_buffer = NULL;
unsigned int double_buffer_height = SCREEN_HEIGHT;
unsigned int double_buffer_size = SCREEN_WIDTH*SCREEN_HEIGHT/2;

////////////////////////////////////////////////////////////////////////////////
//
// 			F  U  N  C  T  I  O  N  S
//
////////////////////////////////////////////////////////////////////////////////


void SetGraphicsMode(int mode)
{

	union REGS inregs,outregs;

	inregs.h.ah = 0;
	inregs.h.al = (unsigned char)mode;
	
	int86(0x10, &inregs, &outregs);


} //  end SetGraphicsMode  //

//////////////////////////////////////////////////////////////

void FillScreen(int color)
{
	_asm {
		les di, video_buffer
		mov al,BYTE PTR color
		mov ah,al
		mov cx,320*200/2
		rep stosw
	          }

} // end FillScreen //

//////////////////////////////////////////////////////////////

void PutPixel(int x, int y, int color)
{

	double_buffer[((y<<8)+(y<<6))+x] = (unsigned char)color;

} // end PutPixel //

//////////////////////////////////////////////////////////////

int ReadPixel(int x, int y)
{

	return((int)(double_buffer[((y<<8)+(y<<6))+x]));

} // end ReadPixel //

//////////////////////////////////////////////////////////////

void LineH(int x1, int x2, int y, int color)
{
	int temp;

	if (x1>x2)
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	_fmemset((char far *)(double_buffer + ((y<<8)+(y<<6))+x1),
		(unsigned char)color, x2-x1+1);

} // end LineH //

//////////////////////////////////////////////////////////////

void LineV(int y1, int y2, int x, int color)
{
	unsigned char far *start_offset;
	int index, temp;

	if (y1>y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	start_offset = double_buffer +((y1<<8)+(y1<<6))+x;

	for (index=0; index<=y2-y1; index++)
	{
		*start_offset = (unsigned char)color;
		start_offset+=320;
	}

} // end LineV //

//////////////////////////////////////////////////////////////

void WriteColorReg(int index, RGB_color_ptr color)
{
	outp(COLOR_REGISTER_WR, index);

   	outp(COLOR_DATA,color->red);
   	outp(COLOR_DATA,color->green);
   	outp(COLOR_DATA,color->blue);

} // end WriteColorReg //


///////////////////////////////////////////////////////////////////////////////

RGB_color_ptr ReadColorReg(int index, RGB_color_ptr color)
{
	outp(COLOR_REGISTER_RD, index);
	
	color->red		= (unsigned char)inp(COLOR_DATA);
	color->green		= (unsigned char)inp(COLOR_DATA);
	color->blue		= (unsigned char)inp(COLOR_DATA);

	return(color);

} // end ReadColorReg //

///////////////////////////////////////////////////////////////////////////////


void ReadPalette(int start_reg, int end_reg, RGB_palette_ptr the_palette)
{
	int index;
	RGB_color color;

	for(index=start_reg; index<=end_reg; index++)
	{
		ReadColorReg(index, (RGB_color_ptr)&color);
	
		the_palette->colors[index].red	= color.red;
		the_palette->colors[index].green 	= color.green;
		the_palette->colors[index].blue	= color.blue;

	}

	the_palette->start_reg 	= start_reg;
	the_palette->end_reg	= end_reg;

} // end ReadPalette //

///////////////////////////////////////////////////////////////////////////////

void WritePalette(int start_reg,int end_reg, RGB_palette_ptr the_palette)
{
	int index;

	for(index=start_reg; index<=end_reg; index++)
	{
		WriteColorReg(index,(RGB_color_ptr)&(the_palette->colors[index]));
	}

} // end WritePalette //

///////////////////////////////////////////////////////////////////////////////

void PrintChar(int xc, int yc, char c, int color, int transparent)
{
	int offset,x,y;
	unsigned char far *work_char;
	unsigned char bit_mask;

	work_char = rom_char_set + c * ROM_CHAR_HEIGHT;
	
	offset = (yc<<8)+(yc<<6) + xc;

	for (y=0; y<ROM_CHAR_HEIGHT; y++)
	{
		bit_mask = 0x80;
	
		for(x=0; x<ROM_CHAR_WIDTH;x++)
		{
			if ((*work_char & bit_mask))
				double_buffer[offset+x] = (unsigned int)color;
			else
			if (!transparent)
				double_buffer[offset+x] = 0;

			bit_mask = (bit_mask >> 1);
		}

		offset	+= MODE13_WIDTH;
		work_char++;

	}

} // end PrintChar //

///////////////////////////////////////////////////////////////////////////////

void PrintString(int x, int y, int color, char *string, int transparent)
{
	int index, length;

	length = strlen(string);

	for(index=0; index<length; index++)
		PrintChar(x+(index<<3),y,string[index],color,transparent);

} // end PrintString //

///////////////////////////////////////////////////////////////////////////////

int CreateDoubleBuffer(int num_lines)
{
	if ((double_buffer = (unsigned char far *)farmalloc(SCREEN_WIDTH * (num_lines+1)))==NULL)
	{
		printf("\nCouldn't allocate double buffer.");
		return(0);
	}

	double_buffer_height = num_lines;
	double_buffer_size = SCREEN_WIDTH * num_lines/2;
	
	_fmemset(double_buffer,0,SCREEN_WIDTH * num_lines);

	return(1);

} // end CreateDoubleBuffer //

///////////////////////////////////////////////////////////////////////////////

void FillDoubleBuffer(int color)
{

	_asm {
		mov cx,double_buffer_size
		mov al,BYTE PTR color
		mov ah,al
		les di,double_buffer
		rep stosw
	         }

} // end FillDoubleBuffer //

///////////////////////////////////////////////////////////////////////////////

void DeleteDoubleBuffer(void)
{

	if (double_buffer)
		farfree(double_buffer);

} // end DeleteDoubleBuffer //

///////////////////////////////////////////////////////////////////////////////

void DisplayDoubleBuffer(unsigned char far *buffer, int y)
{
	_asm {
		push ds
		mov cx,double_buffer_size
		les di,video_buffer
		mov ax,320
		mul y
		add di,ax
		lds si,buffer
		rep movsw
		pop ds
	         }

} // end DisplayDoubleBuffer //

///////////////////////////////////////////////////////////////////////////////

int PCXInit(pcx_picture_ptr image)
{
	if(!(image->buffer = (unsigned char far *)farmalloc(SCREEN_WIDTH*SCREEN_HEIGHT + 1)))
	{
		printf("\nPCX System - Couldn't allocate PCX!");
		return(0);
	}

	return(1);

} // end PCXInit //

///////////////////////////////////////////////////////////////////////////////

void PCXDelete(pcx_picture_ptr image)
{
	farfree(image->buffer);
} // end PCXDelete //

///////////////////////////////////////////////////////////////////////////////

int PCXLoad(char *filename, pcx_picture_ptr image, int load_palette)
{
	FILE *fp;

	int num_bytes, index;
	long count;
	unsigned char data;
	char far *temp_buffer;

	if ((fp=fopen(filename, "rb"))==NULL)
	{
		printf("\nPCX System - File not found!");
		return(0);
	}

	temp_buffer = (char far *)image;

	for (index=0; index<128; index++)
	{
		temp_buffer[index] = (char)getc(fp);
	}

	count = 0;

	while(count<=SCREEN_WIDTH *SCREEN_HEIGHT)
	{
		data = (unsigned char)getc(fp);

		if (data>= 192 && data <= 255)
		{
			num_bytes = data - 192;
			data =(unsigned char)getc(fp);

			while(num_bytes-->0)
			{
				image->buffer[count++] = data;
			}
		}
		else
		{
			image->buffer[count++] = data;
		}
	}

	fseek(fp,-768L,SEEK_END);

	for(index=0; index<256; index++)
	{
		image->palette[index].red = (unsigned char)(getc(fp)>>2);
		image->palette[index].green = (unsigned char)(getc(fp) >> 2);
		image->palette[index].blue = (unsigned char)(getc(fp)>>2);
	}

	fclose(fp);

	if(load_palette)
	{
		for(index=0;index<256; index++)
		{
			WriteColorReg(index,(RGB_color_ptr)&image->palette[index]);
		}
	}

	return(1);

} // end PCXLoad //

///////////////////////////////////////////////////////////////////////////////

void PCXShowBuffer(pcx_picture_ptr image)
{
	char far *data;
	data = image->buffer;

	_asm {
		push ds
		les di, video_buffer
		lds si, data
		mov cx,320*200/2
		cld
		rep movsw
		pop ds
	          }

} // end PCXShowBuffer //

///////////////////////////////////////////////////////////////////////////////

void PCXCopyToBuffer(pcx_picture_ptr image, unsigned char far *buffer)
{

	fwordcpy((void far *)buffer,(void far *)image->buffer,double_buffer_size);

} // end PCXCopyToBuffer //

///////////////////////////////////////////////////////////////////////////////

void fwordcpy(void far *destination, void far *source, int num_words)
{

	_asm {
   		push ds
    		les di,destination
         lds si,source
         mov cx,num_words
         rep movsw
         pop ds
         }

} // end fwordcpy //

//////////////////////////////////////////////////////////////////////////////

void WaitRetrace(void)
{
	while(inp(VGA_STATUS) & VGA_MASK) {}

   while(!(inp(VGA_STATUS) & VGA_MASK)){}

} // end WaitRetrace //

//////////////////////////////////////////////////////////////////////////////

int BitmapAllocate(bitmap_ptr image, int width, int height)
{
	if ((image->buffer = (unsigned char far *)farmalloc(width*height+1))==NULL)
   	return(0);
   else
   	return(1);

} // end BitmapAllocate //

///////////////////////////////////////////////////////////////////////////////

void BitmapDelete(bitmap_ptr the_bitmap)
{
	if (the_bitmap->buffer)
   	farfree(the_bitmap->buffer);
} // end BitmapDelete //

///////////////////////////////////////////////////////////////////////////////

void BitmapGet(bitmap_ptr image, unsigned char far *source)
{
	unsigned int source_off, bitmap_off;

   int y, width, height;
   unsigned char far *bitmap_data;

   source_off = (image->y << 8) + (image->y <<6)+ image->x;

   bitmap_off = 0;

   height = image->height;
   width = image->width;
   bitmap_data = image->buffer;

   for (y=0; y<height; y++)
   {
   	_fmemcpy((void far *)&bitmap_data[bitmap_off],
      			(void far *)&source[source_off], width);

      source_off += SCREEN_WIDTH;
      bitmap_off += width;

   }

} // end BitmapGet //

///////////////////////////////////////////////////////////////////////////////

void BitmapPut(bitmap_ptr image, unsigned char far *destination, int transparent)
{
	int x,y, width, height;

   unsigned char far *bitmap_data;
   unsigned char far *dest_buffer;

   unsigned char pixel;

   dest_buffer = destination + (image->y << 8) + (image->y << 6) + image->x;

   height = image->height;
   width = image->width;
   bitmap_data = image->buffer;

   if (transparent)
   {
   	for (y=0; y<height; y++)
      {
      	for (x=0; x<width; x++)
         {
         	if ((pixel=bitmap_data[x]))
            	dest_buffer[x] = pixel;
         }

         dest_buffer += SCREEN_WIDTH;
         bitmap_data += width;

      }
   }
   else
   {
   	for (y=0; y<height; y++)
      {
      	_fmemcpy((void far *)dest_buffer, (void far *)bitmap_data,width);

         dest_buffer += SCREEN_WIDTH;
         bitmap_data += width;
      }
   }

} // end BitmapPut //

///////////////////////////////////////////////////////////////////////////////

void SpriteInit(sprite_ptr sprite, int x, int y, int width, int height,
												int c1, int c2, int c3,
                                    int t1, int t2, int t3)
{
	int index;

   sprite->x				=x;
   sprite->y				=y;
   sprite->width		  	= width;
   sprite->height   		= height;
   sprite->visible  		= 1;
   sprite->counter_1 	= c1;
   sprite->counter_2 	= c2;
   sprite->counter_3 	= c3;
   sprite->threshold_1 	= t1;
   sprite->threshold_2  = t2;
   sprite->threshold_3  = t3;
   sprite->curr_frame   = 0;
   sprite->state        = SPRITE_DEAD;
   sprite->num_frames   = 0;
   sprite->background   = (unsigned char far *)farmalloc(width*height+1);

   for (index=0; index<MAX_SPRITE_FRAMES; index++)
   	sprite->frames[index] = NULL;

} // end SpriteInit //

///////////////////////////////////////////////////////////////////////////////

void SpriteDelete(sprite_ptr sprite)
{
	int index;

   farfree(sprite->background);

   for(index=0; index<MAX_SPRITE_FRAMES; index++)
   	farfree(sprite->frames[index]);

} // end SpriteDelete

///////////////////////////////////////////////////////////////////////////////

void PCXGetSprite(pcx_picture_ptr image, sprite_ptr sprite, int sprite_frame,
							int cell_x, int cell_y)
{
	int x_off, y_off,
   	 y,
       width, height;

   unsigned char far *sprite_data;

   width = sprite->width;
	height = sprite->height;

   sprite->frames[sprite_frame] = (unsigned char far *)farmalloc(width*height+1);

   sprite_data = sprite->frames[sprite_frame];

   x_off = (width+1) *cell_x+1;
   y_off = (height+1) * cell_y +1;

   y_off = y_off * 320;

   for (y=0; y<height; y++,y_off+=320)
   {
   	_fmemcpy((void far *)&sprite_data[y*width],
      		(void far *)&(image->buffer[y_off + x_off]),width);

   }

   sprite->num_frames++;

} // end PCXGetSprite //

///////////////////////////////////////////////////////////////////////////////

void SpriteDraw(sprite_ptr sprite, unsigned char far *buffer, int transparent)
{
	unsigned char far *sprite_data;
   unsigned char far *dest_buffer;

   int x,y,
   	width,height;

   unsigned char pixel;

   sprite_data = sprite->frames[sprite->curr_frame];

   width = sprite->width;
   height = sprite->height;

   dest_buffer = buffer + (sprite->y << 8) + (sprite->y << 6) + sprite->x;

   if (transparent)
   {
   	for (y=0; y<height; y++)
      {
      	for (x=0; x<width; x++)
         {
         	if ((pixel=sprite_data[x]))
            	dest_buffer[x] = pixel;

         }

         dest_buffer += SCREEN_WIDTH;
         sprite_data += width;

      }
   }
   else
   {
   	for (y=0; y<height; y++)
      {
      	_fmemcpy((void far *)dest_buffer,(void far *)sprite_data,width);

         dest_buffer += SCREEN_WIDTH;
         sprite_data += width;

      }
   }
} // end SpriteDraw //

///////////////////////////////////////////////////////////////////////////////

void SpriteErase(sprite_ptr sprite, unsigned char far *buffer)
{
	unsigned char far *back_buffer;

   int y, width, height;

   back_buffer = sprite->background;

   width = sprite->width;
   height = sprite->height;

   buffer = buffer + (sprite->y << 8) + (sprite->y << 6) + sprite->x;

   for (y=0; y<height; y++)
   {
   	_fmemcpy((void far *)buffer, (void far *)back_buffer, width);

      buffer	+= SCREEN_WIDTH;
      back_buffer += width;

   }

} // end SpriteErase //

///////////////////////////////////////////////////////////////////////////////

void SpriteUnder(sprite_ptr sprite, unsigned char far *buffer)
{
	unsigned char far *back_buffer;

   int y, width, height;

   back_buffer = sprite->background;

   width = sprite->width;
   height = sprite->height;

   buffer = buffer + (sprite->y << 8) + (sprite->y << 6) + sprite->x;

   for (y=0; y<height; y++)
   {
   	_fmemcpy((void far *)back_buffer, (void far *)buffer, width);

      buffer += SCREEN_WIDTH;
      back_buffer += width;

   }

} // end SpriteUnder //

///////////////////////////////////////////////////////////////////////////////

void ScaleSprite(sprite_ptr sprite, float scale)
{
	char far *work_sprite;
   int work_offset=0, offset, x, y;
   unsigned char data;
	float y_scale_index, x_scale_step, y_scale_step, x_scale_index;

   y_scale_index = 0;

   y_scale_step =	sprite->height / scale;
   x_scale_step = sprite->width / scale;

   work_sprite = sprite->frames[sprite->curr_frame];

   offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

   for (y=0; y < (int)scale; y++)
   {
   	x_scale_index = 0;

      for (x=0; x<(int)scale; x++)
      {
      	if ((data=work_sprite[work_offset+(int)x_scale_index]))
         	double_buffer[offset+x] = data;

      	x_scale_index += x_scale_step;

      }

      y_scale_index += y_scale_step;

      offset += SCREEN_WIDTH;
      work_offset = sprite->width*(int)y_scale_index;

   }

} // end ScaleSprite

//////////////////////////////////////////////////////////////////////////



