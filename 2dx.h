///////////////////////////////////////////////////////////////////////////////
//	Header File for use with 2DX library
//	
//	Last updated: 7/11/02
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//		D  E  F  I  N  E  S
///////////////////////////////////////////////////////////////////////////////

#define GRAPHICS_MODE13				0x13
#define TEXT_MODE						0x03

#define COLOR_MASK					0x3C6
#define COLOR_REGISTER_RD			0x3C7
#define COLOR_REGISTER_WR			0x3C8
#define COLOR_DATA					0x3C9

#define ROM_CHAR_SET_SEG			0xF000
#define ROM_CHAR_SET_OFF			0xFA6E
#define ROM_CHAR_WIDTH				8
#define ROM_CHAR_HEIGHT				8

#define MODE13_WIDTH					(unsigned int)320
#define MODE13_HEIGHT				(unsigned int)200
#define SCREEN_WIDTH					(unsigned int)320
#define SCREEN_HEIGHT				(unsigned int)200

#define VGA_MASK						0x08
#define VGA_STATUS					0x3DA

#define SPRITE_WIDTH					16
#define SPRITE_HEIGHT				16

#define MAX_SPRITE_FRAMES			32

#define SPRITE_DEAD					0
#define SPRITE_ALIVE					1
#define SPRITE_DYING					2

#define SET_SPRITE_SIZE(w,h) {sprite_width=w; sprite_height=h;}



///////////////////////////////////////////////////////////////////////////////
//
//		S  T  R  C  T  U  R  E  S
//
///////////////////////////////////////////////////////////////////////////////

typedef struct RGB_color_typ
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;

} RGB_color, *RGB_color_ptr;

typedef struct RGB_palette_typ
{
	int start_reg;
	int end_reg;

	RGB_color colors[256];

} RGB_palette, *RGB_palette_ptr;

typedef struct pcx_header_typ
{

	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	int x,y;
	int width, height;
	int vert_res;
	char ega_palette[48];
	char reserved;
	char num_color_planes;
	int bytes_per_line;
	int palette_type;
	char padding[58];

} pcx_header, *pcx_header_ptr;

typedef struct pcx_picture_typ
{
	pcx_header header;
	RGB_color palette[256];
	unsigned char far *buffer;

} pcx_picture, *pcx_picture_ptr;

typedef struct bitmap_typ
{
	int x,y;
   int width,height;
   unsigned char far *buffer;
} bitmap, *bitmap_ptr;

typedef struct sprite_typ
{
	int x,y;
   int width, height;

   int counter_1,
   	 counter_2,
   	 counter_3;

   int threshold_1,
   	 threshold_2,
       threshold_3;

   unsigned char far *frames[MAX_SPRITE_FRAMES];

   int curr_frame;
   int num_frames;
   int state;

   unsigned char far *background;

   int x_clip,y_clip;
   int width_clip,height_clip;
   int visible;

} sprite, *sprite_ptr;



///////////////////////////////////////////////////////////////////////////////
//
//		P  R  O  T  O  T  Y  P  E  S
//
///////////////////////////////////////////////////////////////////////////////

void SetGraphicsMode(int mode);
void FillScreen(int color);
void PutPixel(int x, int y, int color);
int ReadPixel(int x, int y);
void LineH(int x1, int x2, int y, int color);
void LineV(int y1, int y2, int x, int color);
void WriteColorReg(int index, RGB_color_ptr color);
RGB_color_ptr ReadColorReg(int index, RGB_color_ptr color);
void ReadPalette(int start_reg, int end_reg, RGB_palette_ptr the_palette);
void WritePalette(int start_reg,int end_reg, RGB_palette_ptr the_palette);
void PrintChar(int xc, int yc, char c, int color, int transparent);
void PrintString(int x, int y, int color, char *string, int transparent);
int CreateDoubleBuffer(int num_lines);
void FillDoubleBuffer(int color);
void DeleteDoubleBuffer(void);
void DisplayDoubleBuffer(unsigned char far *buffer, int y);
int PCXInit(pcx_picture_ptr image);
void PCXDelete(pcx_picture_ptr image);
int PCXLoad(char *filename, pcx_picture_ptr image, int load_palette);
void PCXShowBuffer(pcx_picture_ptr image);
void PCXCopyToBuffer(pcx_picture_ptr image, unsigned char far *buffer);
void fwordcpy(void far *destination, void far *source, int num_words);
void WaitRetrace(void);
int BitmapAllocate(bitmap_ptr image, int width, int height);
void BitmapDelete(bitmap_ptr the_bitmap);
void BitmapGet(bitmap_ptr image, unsigned char far *source);
void BitmapPut(bitmap_ptr image, unsigned char far *destination, int transparent);
void SpriteInit(sprite_ptr sprite, int x, int y, int width, int height,
												int c1, int c2, int c3,
                                    int t1, int t2, int t3);
void SpriteDelete(sprite_ptr sprite);
void PCXGetSprite(pcx_picture_ptr image, sprite_ptr sprite, int sprite_frame,
								int cell_x, int cell_y);
void SpriteDraw(sprite_ptr sprite, unsigned char far *buffer, int transparent);
void SpriteErase(sprite_ptr sprite, unsigned char far *buffer);
void SpriteUnder(sprite_ptr sprite, unsigned char far *buffer);
void ScaleSprite(sprite_ptr sprite, float scale);

///////////////////////////////////////////////////////////////////////////////
//
//		G  L  O  B  A  L  S
//
///////////////////////////////////////////////////////////////////////////////

extern unsigned char far *video_buffer;
extern unsigned char far *rom_char_set;
extern unsigned char far *double_buffer;
extern unsigned int double_buffer_height;
extern unsigned int double_buffer_size;
extern unsigned int sprite_width;
extern unsigned int sprite_height;

