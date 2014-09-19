///////////////////////////////////////////////////////////////////////////////
//
//		Header File for use with: keyx.cpp
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//					D  E  F  I  N  E  S
///////////////////////////////////////////////////////////////////////////////

#define KEYBOARD_INTERRUPT				0x09
#define KEY_BUFFER						0x60
#define KEY_CONTROL						0x61
#define PIC_PORT							0x20

#define SHIFT_RIGHT						0x0001
#define SHIFT_LEFT						0x0002
#define CTRL								0x0004
#define ALT									0x0008
#define SCROLL_LOCK_ON					0x0010
#define NUM_LOCK_ON						0x0020
#define CAPS_LOCK_ON						0x0040
#define INSERT_MODE						0x0080
#define CTRL_LEFT							0x0100
#define ALT_LEFT							0x0200
#define CTRL_RIGHT						0x0400
#define ALT_RIGHT							0x0800
#define SCROLL_LOCK_DOWN				0x1000
#define NUM_LOCK_DOWN					0x2000
#define CAPS_LOCK_DOWN					0x4000
#define SYS_REQ_DOWN						0x8000

#define MAKE_ESC							1
#define MAKE_1								2
#define MAKE_2								3
#define MAKE_3								4
#define MAKE_4								5
#define MAKE_5                6
#define MAKE_6                7
#define MAKE_7                8
#define MAKE_8                9
#define MAKE_9                10
#define MAKE_0                11
#define MAKE_MINUS            12
#define MAKE_EQUALS           13
#define MAKE_BKSP             14
#define MAKE_TAB              15
#define MAKE_Q                16
#define MAKE_W                17
#define MAKE_E                18
#define MAKE_R                19
#define MAKE_T                20
#define MAKE_Y                21
#define MAKE_U                22
#define MAKE_I                23
#define MAKE_O                24
#define MAKE_P                25
#define MAKE_LFT_BRACKET      26
#define MAKE_RGT_BRACKET      27
#define MAKE_ENTER            28
#define MAKE_CTRL             29
#define MAKE_A                30
#define MAKE_S                31
#define MAKE_D                32
#define MAKE_F                33
#define MAKE_G                34
#define MAKE_H                35
#define MAKE_J                36
#define MAKE_K                37
#define MAKE_L                38
#define MAKE_SEMI             39
#define MAKE_APOS             40
#define MAKE_TILDE            41
#define MAKE_LEFT_SHIFT       42
#define MAKE_BACK_SLASH       43
#define MAKE_Z                44
#define MAKE_X                45
#define MAKE_C                46
#define MAKE_V                47
#define MAKE_B                48
#define MAKE_N                49
#define MAKE_M                50
#define MAKE_COMMA            51
#define MAKE_PERIOD           52
#define MAKE_FOWARD_SLASH     53
#define MAKE_RIGHT_SHIFT      54
#define MAKE_PRT_SCRN         55
#define MAKE_ALT              56
#define MAKE_SPACE            57
#define MAKE_CAPS_LOCK        58
#define MAKE_F1               59
#define MAKE_F2               60
#define MAKE_F3               61
#define MAKE_F4               62
#define MAKE_F5               63
#define MAKE_F6               64
#define MAKE_F7               65
#define MAKE_F8               66
#define MAKE_F9               67
#define MAKE_F10              68
#define MAKE_F11              87
#define MAKE_F12              88
#define MAKE_NUM_LOCK         69
#define MAKE_SCROLL_LOCK      70
#define MAKE_HOME             71
#define MAKE_UP               72
#define MAKE_PGUP             73
#define MAKE_KEYPAD_MINUS     74
#define MAKE_LEFT             75
#define MAKE_CENTER           76
#define MAKE_RIGHT            77
#define MAKE_KEYPAD_PLUS      78
#define MAKE_END              79
#define MAKE_DOWN             80
#define MAKE_PGDWN            81
#define MAKE_INS              82
#define MAKE_DEL              83

#define BREAK_ESC              (1 +128)
#define BREAK_1                (2 +128)
#define BREAK_2                (3 +128)
#define BREAK_3                (4 +128)
#define BREAK_4                (5 +128)
#define BREAK_5                (6 +128)
#define BREAK_6                (7 +128)
#define BREAK_7                (8 +128)
#define BREAK_8                (9 +128)
#define BREAK_9                (10+128)
#define BREAK_0                (11+128)
#define BREAK_MINUS            (12+128)
#define BREAK_EQUALS           (13+128)
#define BREAK_BKSP             (14+128)
#define BREAK_TAB              (15+128)
#define BREAK_Q                (16+128)
#define BREAK_W                (17+128)
#define BREAK_E                (18+128)
#define BREAK_R                (19+128)
#define BREAK_T                (20+128)
#define BREAK_Y                (21+128)
#define BREAK_U                (22+128)
#define BREAK_I                (23+128)
#define BREAK_O                (24+128)
#define BREAK_P                (25+128)
#define BREAK_LFT_BRACKET      (26+128)
#define BREAK_RGT_BRACKET      (27+128)
#define BREAK_ENTER            (28+128)
#define BREAK_CTRL             (29+128)
#define BREAK_A                (30+128)
#define BREAK_S                (31+128)
#define BREAK_D                (32+128)
#define BREAK_F                (33+128)
#define BREAK_G                (34+128)
#define BREAK_H                (35+128)
#define BREAK_J                (36+128)
#define BREAK_K                (37+128)
#define BREAK_L                (38+128)
#define BREAK_SEMI             (39+128)
#define BREAK_APOS             (40+128)
#define BREAK_TILDE            (41+128)
#define BREAK_LEFT_SHIFT       (42+128)
#define BREAK_BACK_SLASH       (43+128)
#define BREAK_Z                (44+128)
#define BREAK_X                (45+128)
#define BREAK_C                (46+128)
#define BREAK_V                (47+128)
#define BREAK_B                (48+128)
#define BREAK_N                (49+128)
#define BREAK_M                (50+128)
#define BREAK_COMMA            (51+128)
#define BREAK_PERIOD           (52+128)
#define BREAK_FOWARD_SLASH     (53+128)
#define BREAK_RIGHT_SHIFT      (54+128)
#define BREAK_PRT_SCRN         (55+128)
#define BREAK_ALT              (56+128)
#define BREAK_SPACE            (57+128)
#define BREAK_CAPS_LOCK        (58+128)
#define BREAK_F1               (59+128)
#define BREAK_F2               (60+128)
#define BREAK_F3               (61+128)
#define BREAK_F4               (62+128)
#define BREAK_F5               (63+128)
#define BREAK_F6               (64+128)
#define BREAK_F7               (65+128)
#define BREAK_F8               (66+128)
#define BREAK_F9               (67+128)
#define BREAK_F10              (68+128)
#define BREAK_F11              (87+128)
#define BREAK_F12              (88+128)
#define BREAK_NUM_LOCK         (69+128)
#define BREAK_SCROLL_LOCK      (70+128)
#define BREAK_HOME             (71+128)
#define BREAK_UP               (72+128)
#define BREAK_PGUP             (73+128)
#define BREAK_KEYPAD_MINUS     (74+128)
#define BREAK_LEFT             (75+128)
#define BREAK_CENTER           (76+128)
#define BREAK_RIGHT            (77+128)
#define BREAK_KEYPAD_PLUS      (78+128)
#define BREAK_END              (79+128)
#define BREAK_DOWN             (80+128)
#define BREAK_PGDWN            (81+128)
#define BREAK_INS              (82+128)
#define BREAK_DEL              (83+128)

#define KEY_UP                 0
#define KEY_DOWN               1

//  E X T E R N A L S /////////////////////////////////////////////////////////
extern void (_interrupt _far *Old_Keyboard_ISR)();
extern int raw_scan_code;
extern int keyboard_state[128];
extern int keys_active;

//  P R O T O T Y P E S ////////////////////////////////////////////////////////
void _interrupt _far KeyboardDriver();
void KeyboardInstallDriver(void);
void KeyboardRemoveDriver(void);
unsigned char GetKey(void);
unsigned int GetControlKeys(unsigned int mask);
unsigned char GetScanCode(void);

