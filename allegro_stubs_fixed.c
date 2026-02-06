// DS1 Editor Allegro Compatibility Implementation 
// Fixes for type conversion and missing function issues 
 
#include "allegro5_compat.h" 
#include <stdarg.h> 
 
// Global variables for Allegro 5 compatibility 
ALLEGRO_DISPLAY *g_display = NULL; 
ALLEGRO_BITMAP *g_screen_buffer = NULL; 
ALLEGRO_FONT *g_font = NULL; 
ALLEGRO_EVENT_QUEUE *g_event_queue = NULL; 
ALLEGRO_TIMER *g_timer = NULL; 
int mouse_x = 0, mouse_y = 0, mouse_b = 0; 
int key[ALLEGRO_KEY_MAX]; 
int SCREEN_W = 800, SCREEN_H = 600; 
 
