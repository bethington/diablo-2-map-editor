#ifndef ALLEGRO5_COMPAT_H
#define ALLEGRO5_COMPAT_H

// Include Allegro 5.2 from NuGet package
#include "packages/Allegro.5.2.10/build/native/include/allegro5/allegro.h"
#include "packages/Allegro.5.2.10/build/native/include/allegro5/allegro_primitives.h"
#include "packages/Allegro.5.2.10/build/native/include/allegro5/allegro_image.h"
#include "packages/Allegro.5.2.10/build/native/include/allegro5/allegro_font.h"
#include "packages/Allegro.5.2.10/build/native/include/allegro5/allegro_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h> // for access() on Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Undefine Windows BITMAP to avoid conflicts
#ifdef BITMAP
#undef BITMAP
#endif
#endif

// Basic compatibility types - only define if not already defined
#ifndef UBYTE
typedef unsigned char UBYTE;
#endif

// Allegro 4 compatibility layer for Allegro 5.2
// Global state for Allegro 5
extern ALLEGRO_DISPLAY *g_display;
extern ALLEGRO_BITMAP *g_screen_buffer;
extern ALLEGRO_FONT *g_font;
extern ALLEGRO_EVENT_QUEUE *g_event_queue;
extern ALLEGRO_TIMER *g_timer;

// Allegro 4 type compatibility using Allegro 5.2 types
typedef struct
{
	ALLEGRO_BITMAP *al_bmp;
	int w, h;
	unsigned char **line; // line pointers for direct access
	int clip_x1, clip_y1; // clipping rectangle
	int clip_x2, clip_y2;
	void *vtable;		 // vtable pointer (unused in our implementation)
	unsigned char *data; // raw bitmap data
} BITMAP_A4;

// For compatibility, use BITMAP_A4 as BITMAP
#define BITMAP BITMAP_A4

// Font compatibility - use Allegro 5 font directly
#define FONT ALLEGRO_FONT

typedef ALLEGRO_COLOR RGB;
typedef ALLEGRO_COLOR PALETTE[256];

// Allegro 4 COLOR_MAP compatibility structure
typedef struct
{
	ALLEGRO_COLOR data[64][256];
} COLOR_MAP;

// Mouse and keyboard state variables
extern int mouse_x, mouse_y, mouse_b;
extern int key[ALLEGRO_KEY_MAX];

// Screen dimensions
extern int SCREEN_W, SCREEN_H;

// Global color map for palette effects
extern COLOR_MAP color_map;

// Allegro 4 sprite structure compatibility
typedef struct RLE_SPRITE
{
	int w, h;
	int color_depth;
	void *dat;
	int size;
} RLE_SPRITE;

// File finding structure
typedef struct al_ffblk
{
	char name[260];
	int attrib;
	long size;
} al_ffblk;

// Color conversion macros - convert int colors to ALLEGRO_COLOR
#define INT_TO_COLOR(i) al_map_rgb(((i) >> 16) & 0xFF, ((i) >> 8) & 0xFF, (i) & 0xFF)
#define PALETTE_COLOR(i) ((i) < 256 ? al_map_rgb((i), (i), (i)) : al_map_rgb(255, 255, 255))

// Color conversion utility functions
static inline int color_to_int(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return (r << 16) | (g << 8) | b;
}

static inline ALLEGRO_COLOR int_to_color(int color_int)
{
	return al_map_rgb((color_int >> 16) & 0xFF, (color_int >> 8) & 0xFF, color_int & 0xFF);
}

// RGB component access functions for ALLEGRO_COLOR
static inline unsigned char rgb_r(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return r;
}

static inline unsigned char rgb_g(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return g;
}

static inline unsigned char rgb_b(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return b;
}

// Key definitions for Allegro 4 compatibility
#define KEY_ESC ALLEGRO_KEY_ESCAPE
#define KEY_ENTER ALLEGRO_KEY_ENTER
#define KEY_ENTER_PAD ALLEGRO_KEY_PAD_ENTER
#define KEY_SPACE ALLEGRO_KEY_SPACE
#define KEY_TAB ALLEGRO_KEY_TAB
#define KEY_BACKSPACE ALLEGRO_KEY_BACKSPACE
#define KEY_F1 ALLEGRO_KEY_F1
#define KEY_F2 ALLEGRO_KEY_F2
#define KEY_F3 ALLEGRO_KEY_F3
#define KEY_F4 ALLEGRO_KEY_F4
#define KEY_F5 ALLEGRO_KEY_F5
#define KEY_F6 ALLEGRO_KEY_F6
#define KEY_F7 ALLEGRO_KEY_F7
#define KEY_F8 ALLEGRO_KEY_F8
#define KEY_F9 ALLEGRO_KEY_F9
#define KEY_F10 ALLEGRO_KEY_F10
#define KEY_F11 ALLEGRO_KEY_F11
#define KEY_F12 ALLEGRO_KEY_F12
#define KEY_UP ALLEGRO_KEY_UP
#define KEY_DOWN ALLEGRO_KEY_DOWN
#define KEY_LEFT ALLEGRO_KEY_LEFT
#define KEY_RIGHT ALLEGRO_KEY_RIGHT
#define KEY_MINUS ALLEGRO_KEY_MINUS
#define KEY_EQUALS ALLEGRO_KEY_EQUALS
#define KEY_MINUS_PAD ALLEGRO_KEY_PAD_MINUS
#define KEY_PLUS_PAD ALLEGRO_KEY_PAD_PLUS
#define KEY_DEL_PAD ALLEGRO_KEY_PAD_DELETE
#define KEY_0_PAD ALLEGRO_KEY_PAD_0
#define KEY_1_PAD ALLEGRO_KEY_PAD_1
#define KEY_2_PAD ALLEGRO_KEY_PAD_2
#define KEY_3_PAD ALLEGRO_KEY_PAD_3
#define KEY_4_PAD ALLEGRO_KEY_PAD_4
#define KEY_5_PAD ALLEGRO_KEY_PAD_5
#define KEY_6_PAD ALLEGRO_KEY_PAD_6
#define KEY_7_PAD ALLEGRO_KEY_PAD_7
#define KEY_8_PAD ALLEGRO_KEY_PAD_8
#define KEY_9_PAD ALLEGRO_KEY_PAD_9
#define KEY_LEFT ALLEGRO_KEY_LEFT
#define KEY_RIGHT ALLEGRO_KEY_RIGHT
#define KEY_PGUP ALLEGRO_KEY_PGUP
#define KEY_PGDN ALLEGRO_KEY_PGDN
#define KEY_HOME ALLEGRO_KEY_HOME
#define KEY_END ALLEGRO_KEY_END
#define KEY_DEL ALLEGRO_KEY_DELETE
#define KEY_INSERT ALLEGRO_KEY_INSERT
#define KEY_LSHIFT ALLEGRO_KEY_LSHIFT
#define KEY_RSHIFT ALLEGRO_KEY_RSHIFT
#define KEY_LCONTROL ALLEGRO_KEY_LCTRL
#define KEY_RCONTROL ALLEGRO_KEY_RCTRL
#define KEY_ALT ALLEGRO_KEY_ALT
#define KEY_ALTGR ALLEGRO_KEY_ALTGR
#define KEY_TILDE ALLEGRO_KEY_TILDE
#define KEY_1 ALLEGRO_KEY_1
#define KEY_2 ALLEGRO_KEY_2
#define KEY_3 ALLEGRO_KEY_3
#define KEY_4 ALLEGRO_KEY_4
#define KEY_5 ALLEGRO_KEY_5
#define KEY_6 ALLEGRO_KEY_6
#define KEY_7 ALLEGRO_KEY_7
#define KEY_8 ALLEGRO_KEY_8
#define KEY_9 ALLEGRO_KEY_9
#define KEY_0 ALLEGRO_KEY_0
#define KEY_A ALLEGRO_KEY_A
#define KEY_B ALLEGRO_KEY_B
#define KEY_C ALLEGRO_KEY_C
#define KEY_D ALLEGRO_KEY_D
#define KEY_E ALLEGRO_KEY_E
#define KEY_F ALLEGRO_KEY_F
#define KEY_G ALLEGRO_KEY_G
#define KEY_H ALLEGRO_KEY_H
#define KEY_I ALLEGRO_KEY_I
#define KEY_J ALLEGRO_KEY_J
#define KEY_K ALLEGRO_KEY_K
#define KEY_L ALLEGRO_KEY_L
#define KEY_M ALLEGRO_KEY_M
#define KEY_N ALLEGRO_KEY_N
#define KEY_O ALLEGRO_KEY_O
#define KEY_P ALLEGRO_KEY_P
#define KEY_Q ALLEGRO_KEY_Q
#define KEY_R ALLEGRO_KEY_R
#define KEY_S ALLEGRO_KEY_S
#define KEY_T ALLEGRO_KEY_T
#define KEY_U ALLEGRO_KEY_U
#define KEY_V ALLEGRO_KEY_V
#define KEY_W ALLEGRO_KEY_W
#define KEY_X ALLEGRO_KEY_X
#define KEY_Y ALLEGRO_KEY_Y
#define KEY_Z ALLEGRO_KEY_Z

// Mouse button constants
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4

// Graphics mode constants
#define GFX_TEXT 0x1000
#define GFX_AUTODETECT_WINDOWED 0x2000
#define GFX_AUTODETECT_FULLSCREEN 0x3000

// Global Allegro 4 variables
extern char *allegro_id;
extern char *allegro_error;
extern int os_type;
extern int os_version;
extern int os_revision;
extern int os_multitasking;
extern char *cpu_vendor;
extern int cpu_family;
extern int cpu_model;
extern int cpu_capabilities;
extern volatile int mouse_z;

// OS Type constants
#define OSTYPE_UNKNOWN 0
#define OSTYPE_WIN3 1
#define OSTYPE_WIN95 2
#define OSTYPE_WIN98 3
#define OSTYPE_WINME 4
#define OSTYPE_WINNT 5
#define OSTYPE_WIN2000 6
#define OSTYPE_WINXP 7
#define OSTYPE_WIN2003 8
#define OSTYPE_WINVISTA 9
#define OSTYPE_OS2 10
#define OSTYPE_WARP 11
#define OSTYPE_DOSEMU 12
#define OSTYPE_OPENDOS 13
#define OSTYPE_LINUX 14
#define OSTYPE_SUNOS 15
#define OSTYPE_FREEBSD 16
#define OSTYPE_NETBSD 17
#define OSTYPE_IRIX 18
#define OSTYPE_DARWIN 19
#define OSTYPE_QNX 20
#define OSTYPE_UNIX 21
#define OSTYPE_BEOS 22
#define OSTYPE_MACOS 23
#define OSTYPE_MACOSX 24

// CPU Family constants
#define CPU_FAMILY_UNKNOWN 0
#define CPU_FAMILY_I386 1
#define CPU_FAMILY_I486 2
#define CPU_FAMILY_I586 3
#define CPU_FAMILY_I686 4
#define CPU_FAMILY_ITANIUM 5
#define CPU_FAMILY_POWERPC 6
#define CPU_FAMILY_EXTENDED 7

// CPU Model constants
#define CPU_MODEL_PENTIUM 1
#define CPU_MODEL_K5 2
#define CPU_MODEL_K6 3
#define CPU_MODEL_PENTIUMPRO 4
#define CPU_MODEL_PENTIUMII 5
#define CPU_MODEL_PENTIUMIIIKATMAI 6
#define CPU_MODEL_PENTIUMIIICOPPERMINE 7
#define CPU_MODEL_ATHLON 8
#define CPU_MODEL_DURON 9
#define CPU_MODEL_PENTIUMIV 10
#define CPU_MODEL_XEON 11
#define CPU_MODEL_ATHLON64 12
#define CPU_MODEL_OPTERON 13
#define CPU_MODEL_POWERPC_601 14
#define CPU_MODEL_POWERPC_602 15
#define CPU_MODEL_POWERPC_603 16
#define CPU_MODEL_POWERPC_604 17
#define CPU_MODEL_POWERPC_620 18
#define CPU_MODEL_POWERPC_750 19
#define CPU_MODEL_POWERPC_7400 20
#define CPU_MODEL_POWERPC_7450 21

// Switch constants
#define SWITCH_PAUSE 1

// Forward declarations for DS1 Editor structures
struct sDS1;
struct sDS1EDIT;
struct sCFG;
struct sWORKSPACE_DATAS;
struct sDT1;

// Initialization function from allegro5_globals.c
int ds1editor_allegro5_init(void);

// Global variables are defined in allegro5_globals.c
// and declared in structs.h to avoid conflicts
// extern declarations removed to prevent type conflicts

// Drawing macros that convert color index to ALLEGRO_COLOR
#define makecol(r, g, b) ((int)(((r) << 16) | ((g) << 8) | (b)))
#define makecol15(r, g, b) ((int)(((r) << 16) | ((g) << 8) | (b)))
#define makecol16(r, g, b) ((int)(((r) << 16) | ((g) << 8) | (b)))
#define makecol24(r, g, b) ((int)(((r) << 16) | ((g) << 8) | (b)))
#define makecol32(r, g, b) ((int)(((r) << 16) | ((g) << 8) | (b)))

// Drawing function macros - use color conversion
#define line(bmp, x1, y1, x2, y2, c) line_a4_compat(bmp, x1, y1, x2, y2, int_to_color(c))
#define rect(bmp, x1, y1, x2, y2, c) rect_a4_compat(bmp, x1, y1, x2, y2, int_to_color(c))
#define rectfill(bmp, x1, y1, x2, y2, c) rectfill_a4_compat(bmp, x1, y1, x2, y2, int_to_color(c))
#define circle(bmp, x, y, r, c) circle_a4_compat(bmp, x, y, r, int_to_color(c))
#define circlefill(bmp, x, y, r, c) circlefill_a4_compat(bmp, x, y, r, int_to_color(c))

// Text output macros - use color conversion
#define textout(bmp, font, text, x, y, color) textout_a4_compat(bmp, font, text, x, y, int_to_color(color))
#define textprintf(bmp, font, x, y, color, fmt, ...) textprintf_a4_compat(bmp, font, x, y, int_to_color(color), fmt, ##__VA_ARGS__)

// Line drawing shortcuts
#define hline(bmp, x1, y, x2, color) line_a4_compat(bmp, x1, y, x2, y, int_to_color(color))
#define vline(bmp, x, y1, y2, color) line_a4_compat(bmp, x, y1, x, y2, int_to_color(color))

// Bitmap manipulation macros
#define create_bitmap(w, h) create_bitmap_a4_compat(w, h)
#define destroy_bitmap(bmp) destroy_bitmap_a4_compat(bmp)
#define clear_bitmap(bmp) clear_bitmap_a4_compat(bmp)
#define blit(src, dest, sx, sy, dx, dy, w, h) blit_a4_compat(src, dest, sx, sy, dx, dy, w, h)
#define masked_blit(src, dest, sx, sy, dx, dy, w, h) masked_blit_a4_compat(src, dest, sx, sy, dx, dy, w, h)
#define stretch_blit(src, dest, sx, sy, sw, sh, dx, dy, dw, dh) stretch_blit_a4_compat(src, dest, sx, sy, sw, sh, dx, dy, dw, dh)

// File I/O macros
#define load_bitmap(filename, pal) load_bitmap_a4_compat(filename, pal)
#define save_bitmap(filename, bmp, pal) save_bitmap_a4_compat(filename, bmp, pal)
#define load_pcx(filename, pal) load_bitmap_a4_compat(filename, pal)
#define save_pcx(filename, bmp, pal) save_bitmap_a4_compat(filename, bmp, pal)

// Screen and display
#define screen g_screen_buffer
#define font g_font

// Utility macros
#define keypressed() keypressed_a4_compat()
#define readkey() readkey_a4_compat()
#define poll_mouse() poll_mouse_a4_compat()
#define show_mouse(bmp) show_mouse_a4_compat(bmp)
#define scare_mouse() scare_mouse_a4_compat()
#define unscare_mouse() unscare_mouse_a4_compat()

// Function declarations for compatibility layer implementation
void allegro_init_a4_compat(void);
void allegro_exit_a4_compat(void);
int set_gfx_mode_a4_compat(int card, int w, int h, int v_w, int v_h);
void set_palette_a4_compat(PALETTE pal);

// Drawing functions
void line_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void rect_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void rectfill_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void circle_a4_compat(BITMAP *bitmap, int x, int y, int radius, ALLEGRO_COLOR color);
void circlefill_a4_compat(BITMAP *bitmap, int x, int y, int radius, ALLEGRO_COLOR color);

// Text functions
void textout_a4_compat(BITMAP *bmp, void *font, const char *text, int x, int y, ALLEGRO_COLOR color);
void textprintf_a4_compat(BITMAP *bmp, void *font, int x, int y, ALLEGRO_COLOR color, const char *fmt, ...);

// Bitmap functions
BITMAP *create_bitmap_a4_compat(int width, int height);
void destroy_bitmap_a4_compat(BITMAP *bmp);
void clear_bitmap_a4_compat(BITMAP *bmp);
void blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
void masked_blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
void stretch_blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int source_w, int source_h, int dest_x, int dest_y, int dest_w, int dest_h);

// File I/O functions
BITMAP *load_bitmap_a4_compat(const char *filename, PALETTE pal);
int save_bitmap_a4_compat(const char *filename, BITMAP *bmp, PALETTE pal);

// Input functions
int keypressed_a4_compat(void);
int readkey_a4_compat(void);
void poll_mouse_a4_compat(void);
void show_mouse_a4_compat(BITMAP *bmp);
void scare_mouse_a4_compat(void);
void unscare_mouse_a4_compat(void);

// Palette functions
void set_color_a4_compat(int index, RGB *p);
void get_color_a4_compat(int index, RGB *p);

// Timer functions
void install_timer_a4_compat(void);
void remove_timer_a4_compat(void);

// Sound functions (stubs)
int install_sound_a4_compat(int digi, int midi, const char *cfg_path);
void remove_sound_a4_compat(void);

// Memory functions
void *_AL_MALLOC(size_t size);
void _AL_FREE(void *ptr);

// String functions for file finding
int findfirst_a4_compat(const char *pattern, struct al_ffblk *info, int attrib);
int findnext_a4_compat(struct al_ffblk *info);
void findclose_a4_compat(struct al_ffblk *info);

// Conversion utilities
ALLEGRO_COLOR makecol_a4_compat(int r, int g, int b);
int getr_a4_compat(ALLEGRO_COLOR color);
int getg_a4_compat(ALLEGRO_COLOR color);
int getb_a4_compat(ALLEGRO_COLOR color);

// Color and palette functions
int palette_color(int index);
void create_color_table(COLOR_MAP *table, const PALETTE pal, void (*callback)(const PALETTE, int, int, RGB *), void (*progress)(int));
void create_trans_table(COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*progress)(int));

#endif // ALLEGRO5_COMPAT_H
