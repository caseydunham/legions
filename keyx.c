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

#include "keyx.h"

///////////////////////////////////////////////////////////////////////////////
//
//						G  L  O  B  A  L  S
//
///////////////////////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Keyboard_ISR)();
int raw_scan_code=0;
int keyboard_state[128];

int keys_active = 0; // number of keys being pressed

///////////////////////////////////////////////////////////////////////////////
//
//						F  U  N  C  T  I  O  N  S
//
///////////////////////////////////////////////////////////////////////////////

unsigned char GetKey(void)
{

	if (_bios_keybrd(_KEYBRD_READY))
   	return((unsigned char)_bios_keybrd(_KEYBRD_READ));
   else return(0);

} // end GetKey //

///////////////////////////////////////////////////////////////////////////////

unsigned int GetShiftState(unsigned int mask)
{

	return(mask & _bios_keybrd(_KEYBRD_SHIFTSTATUS));

} // end GetShiftState //

///////////////////////////////////////////////////////////////////////////////

unsigned char GetScanCode(void)
{
	_asm {
   	 	mov ah,01h
         int 16h
         jz buffer_empty
         mov ah,00h
         int 16h
         mov al,ah
         xor ah,ah
         jmp done

   buffer_empty:
   		xor ax,ax
   done:

   		}

} // end GetScanCode //

///////////////////////////////////////////////////////////////////////////////

void _interrupt _far KeyboardDriver()
{
	_asm {
   		sti
         in al, KEY_BUFFER
         xor ah,ah
         mov raw_scan_code, ax
         in al, KEY_CONTROL
         or al, 82h
         out KEY_CONTROL,al
         and al,7fh
         out KEY_CONTROL,al
         mov al,20h
         out PIC_PORT,al

         }

   if (raw_scan_code < 128)
   {
   	if (keyboard_state[raw_scan_code]==KEY_UP)
      {
      	keys_active++;

         keyboard_state[raw_scan_code] = KEY_DOWN;

      }
   }
   else
   {
   	if (keyboard_state[raw_scan_code-128]==KEY_DOWN)
      {
      	keys_active--;

         keyboard_state[raw_scan_code-128] = KEY_UP;

      }
   }

} // end KeyboardDriver //

///////////////////////////////////////////////////////////////////////////////
void KeyboardInstallDriver(void)
{
	int index;

    for (index=0; index<128; index++)
    	keyboard_state[index]=0;

    Old_Keyboard_ISR = _dos_getvect(KEYBOARD_INTERRUPT);

    _dos_setvect(KEYBOARD_INTERRUPT, KeyboardDriver);

} // end InstallKeyboardDriver

///////////////////////////////////////////////////////////////////////////////

void KeyboardRemoveDriver(void)
{

	_dos_setvect(KEYBOARD_INTERRUPT, Old_Keyboard_ISR);

} // end KeyboardRemoveDriver //

///////////////////////////////////////////////////////////////////////////////






