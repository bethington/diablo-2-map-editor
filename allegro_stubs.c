// Allegro 4 to 5.2 compatibility layer implementation
#include "allegro5_compat.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Global state variables
ALLEGRO_DISPLAY *g_display = NULL;
ALLEGRO_BITMAP *g_screen_buffer = NULL;
ALLEGRO_FONT *g_font = NULL;
ALLEGRO_EVENT_QUEUE *g_event_queue = NULL;
ALLEGRO_TIMER *g_timer = NULL;

// Global Allegro 4 variables
char *allegro_id = "Allegro 5.2.10 (Compatibility Layer)";
char *allegro_error = "No error";
int os_type = OSTYPE_WINNT;
int os_version = 10;
int os_revision = 0;
int os_multitasking = 1;
char *cpu_vendor = "Intel";
int cpu_family = CPU_FAMILY_I686;
int cpu_model = CPU_MODEL_XEON;
int cpu_capabilities = 0;
volatile int mouse_z = 0;

// Mouse and keyboard state
int mouse_x = 0, mouse_y = 0, mouse_b = 0;
int key[ALLEGRO_KEY_MAX] = {0};

// Screen dimensions and color map
int SCREEN_W = 800, SCREEN_H = 600;
COLOR_MAP color_map;

// Basic Allegro initialization
void allegro_init_a4_compat(void)
{
	if (!al_init())
	{
		printf("Failed to initialize Allegro!\n");
		exit(1);
	}

	// Initialize addons
	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_install_keyboard();
	al_install_mouse();
	al_install_audio();

	// Create event queue
	g_event_queue = al_create_event_queue();
	if (!g_event_queue)
	{
		printf("Failed to create event queue!\n");
		exit(1);
	}
}

void allegro_exit_a4_compat(void)
{
	if (g_event_queue)
		al_destroy_event_queue(g_event_queue);
	if (g_timer)
		al_destroy_timer(g_timer);
	if (g_font)
		al_destroy_font(g_font);
	if (g_screen_buffer && g_screen_buffer != al_get_backbuffer(g_display))
	{
		al_destroy_bitmap(g_screen_buffer);
	}
	if (g_display)
		al_destroy_display(g_display);

	al_uninstall_audio();
	al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_image_addon();
	al_shutdown_font_addon();
	al_shutdown_primitives_addon();
}

int set_gfx_mode_a4_compat(int card, int w, int h, int v_w, int v_h)
{
	SCREEN_W = w;
	SCREEN_H = h;

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	g_display = al_create_display(w, h);
	if (!g_display)
	{
		printf("Failed to create display!\n");
		return -1;
	}

	// Create screen buffer
	g_screen_buffer = al_get_backbuffer(g_display);

	// Register event sources
	al_register_event_source(g_event_queue, al_get_display_event_source(g_display));
	al_register_event_source(g_event_queue, al_get_keyboard_event_source());
	al_register_event_source(g_event_queue, al_get_mouse_event_source());

	// Create default font
	g_font = al_create_builtin_font();

	return 0;
}

void set_palette_a4_compat(PALETTE pal)
{
	// Allegro 5 doesn't use palettes, so this is a no-op
}

// Drawing functions
void line_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bitmap && bitmap->al_bmp)
	{
		al_set_target_bitmap(bitmap->al_bmp);
	}
	al_draw_line(x1, y1, x2, y2, color, 1.0f);
	al_set_target_bitmap(old_target);
}

void rect_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bitmap && bitmap->al_bmp)
	{
		al_set_target_bitmap(bitmap->al_bmp);
	}

	// Draw rectangle outline
	al_draw_line(x1, y1, x2, y1, color, 1.0f); // top
	al_draw_line(x2, y1, x2, y2, color, 1.0f); // right
	al_draw_line(x2, y2, x1, y2, color, 1.0f); // bottom
	al_draw_line(x1, y2, x1, y1, color, 1.0f); // left

	al_set_target_bitmap(old_target);
}

void rectfill_a4_compat(BITMAP *bitmap, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bitmap && bitmap->al_bmp)
	{
		al_set_target_bitmap(bitmap->al_bmp);
	}

	al_draw_filled_rectangle(x1, y1, x2, y2, color);

	al_set_target_bitmap(old_target);
}

void circle_a4_compat(BITMAP *bitmap, int x, int y, int radius, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bitmap && bitmap->al_bmp)
	{
		al_set_target_bitmap(bitmap->al_bmp);
	}

	al_draw_circle(x, y, radius, color, 1.0f);

	al_set_target_bitmap(old_target);
}

void circlefill_a4_compat(BITMAP *bitmap, int x, int y, int radius, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bitmap && bitmap->al_bmp)
	{
		al_set_target_bitmap(bitmap->al_bmp);
	}

	al_draw_filled_circle(x, y, radius, color);

	al_set_target_bitmap(old_target);
}

// Text functions
void textout_a4_compat(BITMAP *bmp, void *font, const char *text, int x, int y, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
	}

	ALLEGRO_FONT *al_font = font ? (ALLEGRO_FONT *)font : g_font;
	al_draw_text(al_font, color, x, y, ALLEGRO_ALIGN_LEFT, text);

	al_set_target_bitmap(old_target);
}

void textprintf_a4_compat(BITMAP *bmp, void *font, int x, int y, ALLEGRO_COLOR color, const char *fmt, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	textout_a4_compat(bmp, font, buffer, x, y, color);
}

// Bitmap functions
BITMAP *create_bitmap_a4_compat(int width, int height)
{
	BITMAP *bmp = (BITMAP *)malloc(sizeof(BITMAP));
	if (!bmp)
		return NULL;

	bmp->al_bmp = al_create_bitmap(width, height);
	if (!bmp->al_bmp)
	{
		free(bmp);
		return NULL;
	}

	bmp->w = width;
	bmp->h = height;

	return bmp;
}

void destroy_bitmap_a4_compat(BITMAP *bmp)
{
	if (bmp)
	{
		if (bmp->al_bmp)
		{
			al_destroy_bitmap(bmp->al_bmp);
		}
		free(bmp);
	}
}

void clear_bitmap_a4_compat(BITMAP *bmp)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}
	al_set_target_bitmap(old_target);
}

void blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
	if (!source || !dest || !source->al_bmp || !dest->al_bmp)
		return;

	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	al_set_target_bitmap(dest->al_bmp);

	al_draw_bitmap_region(source->al_bmp, source_x, source_y, width, height, dest_x, dest_y, 0);

	al_set_target_bitmap(old_target);
}

void masked_blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
	// Same as regular blit in Allegro 5 (transparency handled automatically)
	blit_a4_compat(source, dest, source_x, source_y, dest_x, dest_y, width, height);
}

void stretch_blit_a4_compat(BITMAP *source, BITMAP *dest, int source_x, int source_y, int source_w, int source_h, int dest_x, int dest_y, int dest_w, int dest_h)
{
	if (!source || !dest || !source->al_bmp || !dest->al_bmp)
		return;

	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	al_set_target_bitmap(dest->al_bmp);

	al_draw_scaled_bitmap(source->al_bmp, source_x, source_y, source_w, source_h, dest_x, dest_y, dest_w, dest_h, 0);

	al_set_target_bitmap(old_target);
}

// File I/O functions
BITMAP *load_bitmap_a4_compat(const char *filename, PALETTE pal)
{
	ALLEGRO_BITMAP *al_bmp = al_load_bitmap(filename);
	if (!al_bmp)
		return NULL;

	BITMAP *bmp = (BITMAP *)malloc(sizeof(BITMAP));
	if (!bmp)
	{
		al_destroy_bitmap(al_bmp);
		return NULL;
	}

	bmp->al_bmp = al_bmp;
	bmp->w = al_get_bitmap_width(al_bmp);
	bmp->h = al_get_bitmap_height(al_bmp);

	return bmp;
}

int save_bitmap_a4_compat(const char *filename, BITMAP *bmp, PALETTE pal)
{
	if (!bmp || !bmp->al_bmp)
		return -1;
	return al_save_bitmap(filename, bmp->al_bmp) ? 0 : -1;
}

// Input functions
int keypressed_a4_compat(void)
{
	if (!g_event_queue)
		return 0;

	ALLEGRO_EVENT event;
	while (al_get_next_event(g_event_queue, &event))
	{
		switch (event.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode < ALLEGRO_KEY_MAX)
			{
				key[event.keyboard.keycode] = 1;
			}
			return 1;
		case ALLEGRO_EVENT_KEY_UP:
			if (event.keyboard.keycode < ALLEGRO_KEY_MAX)
			{
				key[event.keyboard.keycode] = 0;
			}
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			mouse_x = event.mouse.x;
			mouse_y = event.mouse.y;
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (event.mouse.button == 1)
				mouse_b |= MOUSE_LEFT;
			if (event.mouse.button == 2)
				mouse_b |= MOUSE_RIGHT;
			if (event.mouse.button == 3)
				mouse_b |= MOUSE_MIDDLE;
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (event.mouse.button == 1)
				mouse_b &= ~MOUSE_LEFT;
			if (event.mouse.button == 2)
				mouse_b &= ~MOUSE_RIGHT;
			if (event.mouse.button == 3)
				mouse_b &= ~MOUSE_MIDDLE;
			break;
		}
	}
	return 0;
}

int readkey_a4_compat(void)
{
	// Wait for a key press and return the key code
	ALLEGRO_EVENT event;
	while (true)
	{
		al_wait_for_event(g_event_queue, &event);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			return event.keyboard.keycode;
		}
	}
}

void poll_mouse_a4_compat(void)
{
	// Mouse state is updated in keypressed_a4_compat
}

void show_mouse_a4_compat(BITMAP *bmp)
{
	// Show mouse cursor - no-op in this implementation
}

void scare_mouse_a4_compat(void)
{
	// Hide mouse cursor temporarily - no-op
}

void unscare_mouse_a4_compat(void)
{
	// Show mouse cursor again - no-op
}

// Palette functions (no-ops in Allegro 5)
void set_color_a4_compat(int index, RGB *p)
{
	// No-op in Allegro 5
}

void get_color_a4_compat(int index, RGB *p)
{
	// No-op in Allegro 5 - just return white
	*p = al_map_rgb(255, 255, 255);
}

// Timer functions
void install_timer_a4_compat(void)
{
	// Timer is handled by event system
}

void remove_timer_a4_compat(void)
{
	// Timer cleanup handled in allegro_exit
}

// Sound functions (stubs)
int install_sound_a4_compat(int digi, int midi, const char *cfg_path)
{
	return 0; // Success
}

void remove_sound_a4_compat(void)
{
	// No-op
}

// Memory functions
void *_AL_MALLOC(size_t size)
{
	return malloc(size);
}

void _AL_FREE(void *ptr)
{
	free(ptr);
}

// File finding functions (Windows implementation)
#ifdef _WIN32
#include <io.h>
#include <direct.h>

int findfirst_a4_compat(const char *pattern, struct al_ffblk *info, int attrib)
{
	// Use Windows _findfirst
	intptr_t handle = _findfirst(pattern, (struct _finddata_t *)info);
	return (handle == -1) ? -1 : 0;
}

int findnext_a4_compat(struct al_ffblk *info)
{
	// Use Windows _findnext
	return _findnext(0, (struct _finddata_t *)info);
}

void findclose_a4_compat(struct al_ffblk *info)
{
	// Use Windows _findclose
	_findclose(0);
}
#else
// Unix implementation would go here
int findfirst_a4_compat(const char *pattern, struct al_ffblk *info, int attrib)
{
	return -1;
}

int findnext_a4_compat(struct al_ffblk *info)
{
	return -1;
}

void findclose_a4_compat(struct al_ffblk *info)
{
}
#endif

// Color conversion utilities
ALLEGRO_COLOR makecol_a4_compat(int r, int g, int b)
{
	return al_map_rgb(r, g, b);
}

int getr_a4_compat(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return r;
}

int getg_a4_compat(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return g;
}

int getb_a4_compat(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return b;
}

// Missing utility functions
void putpixel(BITMAP_A4 *bmp, int x, int y, int color)
{
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
		al_put_pixel(x, y, PALETTE_COLOR(color));
	}
}

int getpixel(BITMAP_A4 *bmp, int x, int y)
{
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
		ALLEGRO_COLOR color = al_get_pixel(bmp->al_bmp, x, y);
		unsigned char r, g, b;
		al_unmap_rgb(color, &r, &g, &b);
		return (int)r; // Simple approximation
	}
	return 0;
}

void clear(BITMAP_A4 *bmp)
{
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}
}

void vsync(void)
{
	// Modern systems don't need explicit vsync
}

void text_mode(int mode)
{
	// Stub for text mode switch
}

// Color conversion functions
int palette_color(int index)
{
	// Return RGB packed as int rather than ALLEGRO_COLOR
	if (index < 256)
	{
		return (index << 16) | (index << 8) | index; // Grayscale RGB
	}
	return 0xFFFFFF; // White
}

// Color table creation stubs (simplified)
void create_color_table(COLOR_MAP *table, const PALETTE pal, void (*callback)(const PALETTE, int, int, RGB *), void (*progress)(int))
{
	// Simplified stub - just clear the table
	memset(table, 0, sizeof(COLOR_MAP));
}

void create_trans_table(COLOR_MAP *table, const PALETTE pal, int r, int g, int b, void (*progress)(int))
{
	// Simplified stub - just clear the table
	memset(table, 0, sizeof(COLOR_MAP));
}

int color_to_palette_index(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	return (int)r; // Simple approximation
}

// Message dialog
int allegro_message(const char *text)
{
	printf("MESSAGE: %s\n", text);
	return 0;
}

// Configuration functions
void set_config_file(const char *filename)
{
	// Stub implementation
}

const char *get_config_string(const char *section, const char *name, const char *def)
{
	return def; // Return default
}

int get_config_int(const char *section, const char *name, int def)
{
	return def; // Return default
}

// File finding functions
int al_findfirst(const char *pattern, struct al_ffblk *info, int attrib)
{
	return -1; // Not found
}

int al_findnext(struct al_ffblk *info)
{
	return -1; // No more files
}

void al_findclose(struct al_ffblk *info)
{
	// Cleanup stub
}

// Additional bitmap and drawing functions
void clear_to_color(BITMAP_A4 *bmp, ALLEGRO_COLOR color)
{
	if (bmp && bmp->al_bmp)
	{
		al_set_target_bitmap(bmp->al_bmp);
		al_clear_to_color(color);
	}
}

BITMAP_A4 *create_sub_bitmap(BITMAP_A4 *parent, int x, int y, int width, int height)
{
	if (!parent || !parent->al_bmp)
		return NULL;

	BITMAP_A4 *sub = malloc(sizeof(BITMAP_A4));
	if (!sub)
		return NULL;

	sub->al_bmp = al_create_sub_bitmap(parent->al_bmp, x, y, width, height);
	sub->w = width;
	sub->h = height;
	sub->line = NULL;
	sub->clip_x1 = 0;
	sub->clip_y1 = 0;
	sub->clip_x2 = width - 1;
	sub->clip_y2 = height - 1;
	sub->vtable = NULL;
	sub->data = NULL;

	return sub;
}

BITMAP_A4 *create_video_bitmap(int width, int height)
{
	return create_bitmap_a4_compat(width, height);
}

int bitmap_color_depth(BITMAP_A4 *bmp)
{
	return 32; // Allegro 5 uses 32-bit by default
}

// Mouse and input functions
void position_mouse(int x, int y)
{
	if (g_display)
	{
		al_set_mouse_xy(g_display, x, y);
		mouse_x = x;
		mouse_y = y;
	}
}

int desktop_color_depth(void)
{
	return 32;
}

void get_desktop_resolution(int *width, int *height)
{
	ALLEGRO_MONITOR_INFO info;
	if (al_get_monitor_info(0, &info))
	{
		*width = info.x2 - info.x1;
		*height = info.y2 - info.y1;
	}
	else
	{
		*width = 1024;
		*height = 768;
	}
}

void set_window_title(const char *title)
{
	if (g_display)
	{
		al_set_window_title(g_display, title);
	}
}

void request_refresh_rate(int rate)
{
	// Stub - Allegro 5 handles refresh rate automatically
}

int get_refresh_rate(void)
{
	return 60; // Default refresh rate
}

void set_display_switch_mode(int mode)
{
	// Stub - modern systems handle this automatically
}

void set_mouse_speed(int xspeed, int yspeed)
{
	// Stub implementation
}

void rest(int milliseconds)
{
	al_rest(milliseconds / 1000.0);
}

// Locking functions (not needed in Allegro 5)
void LOCK_VARIABLE(void *var)
{
	// No-op in Allegro 5
}

void LOCK_FUNCTION(void *func)
{
	// No-op in Allegro 5
}

// Timer and interrupt functions
void install_int(void (*proc)(void), int speed)
{
	// Stub - use Allegro 5 timers instead
}

// Palette functions
void set_palette(const PALETTE *p)
{
	// Stub - Allegro 5 doesn't use indexed palettes
}

// RLE sprite functions
void destroy_rle_sprite(RLE_SPRITE *sprite)
{
	if (sprite)
	{
		free(sprite);
	}
}

void draw_sprite(BITMAP_A4 *bmp, RLE_SPRITE *sprite, int x, int y)
{
	// Stub implementation
}

// File utility functions
int file_exists(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file)
	{
		fclose(file);
		return 1;
	}
	return 0;
}

// Project-specific stub functions (to be implemented properly later)
// These are minimal stubs to get the build working

// DCC/DC6 functions
void *dcc_decode(void *dcc, int frame, int direction) { return NULL; }
void *dcc_mem_load(void *data, int size) { return NULL; }
void dcc_destroy(void *dcc) {}

// MPQ and file loading
void *misc_load_mpq_file(const char *filename, int *size) { return NULL; }
int misc_get_txt_column_num(void *data, const char *column) { return -1; }

// Edit object functions
void editobj_set_drawing_order(void) {}
void editobj_make_obj_label(void) {}
void editobj_make_obj_desc(void) {}
void editobj_handler(void) {}

// Misc functions
void change_zoom(int zoom) {}
void misc_increase_ds1_objects_max(void) {}
int coord_to_tile(int x, int y) { return 0; }
int mouse_to_tile(int x, int y) { return 0; }
void misc_draw_screen(void) {}
void misc_update_pal_with_gamma(void) {}
void misc_pcx_put_d2_palette(void) {}
void misc_pal_d2_2_vga(void) {}
void misc_read_gamma(void) {}
void misc_make_cmaps(void) {}
void misc_load_pal_from_disk(void) {}
void misc_save_pal_on_disk(void) {}
void misc_open_1_ds1(void) {}
void misc_open_several_ds1(void) {}
void misc_walkable_tile_info_pcx(void) {}
void misc_open_1_ds1_force_dt1(void) {}
void misc_pl2_correct(void) {}
void misc_cmd_line_parse(int argc, char **argv) {}

// wEdit functions
void wedit_test(void) {}
void wedit_make_2nd_buttons(void) {}
void wedit_read_pcx(void) {}
void wedit_free(void) {}

// wPreview functions
void wpreview_draw_tiles(void) {}
void wpreview_draw_tiles_big_screenshot(void) {}
