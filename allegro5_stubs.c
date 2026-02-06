// Allegro 5 compatibility layer implementation
// Provides Allegro 4 function equivalents using Allegro 5

#include "allegro5_compat.h"
#include <math.h>

// External global variables
extern ALLEGRO_DISPLAY *g_display;
extern ALLEGRO_BITMAP *g_screen_buffer;
extern PALETTE g_palette;
extern int mouse_x, mouse_y, mouse_z, mouse_b, key;

// Bitmap compatibility functions
BITMAP_A4 *create_bitmap_a4_compat(int width, int height)
{
	BITMAP_A4 *bmp = (BITMAP_A4 *)malloc(sizeof(BITMAP_A4));
	if (bmp)
	{
		bmp->allegro_bitmap = al_create_bitmap(width, height);
		bmp->w = width;
		bmp->h = height;
	}
	return bmp;
}

void destroy_bitmap_a4_compat(BITMAP_A4 *bmp)
{
	if (bmp)
	{
		if (bmp->allegro_bitmap)
		{
			al_destroy_bitmap(bmp->allegro_bitmap);
		}
		free(bmp);
	}
}

void blit_a4_compat(BITMAP_A4 *src, BITMAP_A4 *dest, int sx, int sy, int dx, int dy, int w, int h)
{
	if (src && src->allegro_bitmap && dest && dest->allegro_bitmap)
	{
		al_set_target_bitmap(dest->allegro_bitmap);
		al_draw_bitmap_region(src->allegro_bitmap, sx, sy, w, h, dx, dy, 0);
	}
}

void stretch_blit_a4_compat(BITMAP_A4 *src, BITMAP_A4 *dest, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
	if (src && src->allegro_bitmap && dest && dest->allegro_bitmap)
	{
		al_set_target_bitmap(dest->allegro_bitmap);
		al_draw_scaled_bitmap(src->allegro_bitmap, sx, sy, sw, sh, dx, dy, dw, dh, 0);
	}
}

// Drawing functions
void putpixel(BITMAP_A4 *bmp, int x, int y, ALLEGRO_COLOR color)
{
	if (bmp && bmp->allegro_bitmap)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_put_pixel(x, y, color);
	}
}

ALLEGRO_COLOR getpixel(BITMAP_A4 *bmp, int x, int y)
{
	if (bmp && bmp->allegro_bitmap)
	{
		return al_get_pixel(bmp->allegro_bitmap, x, y);
	}
	return al_map_rgba(0, 0, 0, 0);
}

ALLEGRO_COLOR makecol(int r, int g, int b)
{
	return al_map_rgb(r, g, b);
}

void clear(BITMAP_A4 *bmp)
{
	if (bmp && bmp->allegro_bitmap)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}
}

void draw_sprite(BITMAP_A4 *dest, BITMAP_A4 *sprite, int x, int y)
{
	if (dest && dest->allegro_bitmap && sprite && sprite->allegro_bitmap)
	{
		al_set_target_bitmap(dest->allegro_bitmap);
		al_draw_bitmap(sprite->allegro_bitmap, x, y, 0);
	}
}

// Graphics primitives
void line_a4_compat(BITMAP_A4 *bmp, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	if (bmp && bmp->allegro_bitmap)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_draw_line(x1, y1, x2, y2, color, 1.0);
	}
}

void rect_a4_compat(BITMAP_A4 *bmp, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	if (bmp && bmp->allegro_bitmap)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_draw_rectangle(x1, y1, x2, y2, color, 1.0);
	}
}

void rectfill_a4_compat(BITMAP_A4 *bmp, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	if (bmp && bmp->allegro_bitmap)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
	}
}

// Text functions
void textout_a4_compat(BITMAP_A4 *bmp, ALLEGRO_FONT *font, const char *text, int x, int y, ALLEGRO_COLOR color)
{
	if (bmp && bmp->allegro_bitmap && font && text)
	{
		al_set_target_bitmap(bmp->allegro_bitmap);
		al_draw_text(font, color, x, y, 0, text);
	}
}

void textprintf_a4_compat(BITMAP_A4 *bmp, ALLEGRO_FONT *font, int x, int y, ALLEGRO_COLOR color, const char *format, ...)
{
	if (bmp && bmp->allegro_bitmap && font && format)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		al_set_target_bitmap(bmp->allegro_bitmap);
		al_draw_text(font, color, x, y, 0, buffer);
	}
}

void text_mode(int mode)
{
	// Text mode is handled differently in Allegro 5
	// This is a stub for compatibility
}

// Color palette functions
ALLEGRO_COLOR palette_color(int index)
{
	if (index >= 0 && index < 256)
	{
		RGB *rgb = &g_palette[index];
		return al_map_rgb(rgb->r * 4, rgb->g * 4, rgb->b * 4); // Scale from 0-63 to 0-255
	}
	return al_map_rgb(0, 0, 0);
}

int color_to_palette_index(ALLEGRO_COLOR color)
{
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);

	// Find closest palette color (simple implementation)
	int best_match = 0;
	int best_distance = INT_MAX;

	for (int i = 0; i < 256; i++)
	{
		RGB *rgb = &g_palette[i];
		int pr = rgb->r * 4;
		int pg = rgb->g * 4;
		int pb = rgb->b * 4;

		int distance = (r - pr) * (r - pr) + (g - pg) * (g - pg) + (b - pb) * (b - pb);
		if (distance < best_distance)
		{
			best_distance = distance;
			best_match = i;
		}
	}

	return best_match;
}

// File functions
int file_exists(const char *filename)
{
	if (!filename)
		return 0;

	ALLEGRO_FS_ENTRY *entry = al_create_fs_entry(filename);
	if (!entry)
		return 0;

	bool exists = al_fs_entry_exists(entry);
	al_destroy_fs_entry(entry);

	return exists ? 1 : 0;
}

// Configuration functions
void set_config_file(const char *filename)
{
	// Stub - configuration handled differently in DS1 Editor
}

const char *get_config_string(const char *section, const char *key, const char *default_value)
{
	// Stub - return default value for now
	return default_value;
}

int get_config_int(const char *section, const char *key, int default_value)
{
	// Stub - return default value for now
	return default_value;
}

// Message box
int allegro_message(const char *message)
{
	// In a real implementation, this would show a message box
	printf("DS1 Editor Message: %s\n", message);
	return 1;
}

// Input functions
void position_mouse(int x, int y)
{
	if (g_display)
	{
		al_set_mouse_xy(g_display, x, y);
	}
}

// Timing
void rest(int time_ms)
{
	al_rest(time_ms / 1000.0);
}

// Palette functions
void set_palette(PALETTE palette)
{
	if (palette)
	{
		memcpy(g_palette, palette, sizeof(PALETTE));
	}
}

// Color depth
void set_color_depth(int depth)
{
	// Color depth is handled differently in Allegro 5
	// This is a stub for compatibility
}

// Screen functions
void show_video_bitmap(BITMAP_A4 *bmp)
{
	if (bmp && bmp->allegro_bitmap && g_display)
	{
		al_set_target_backbuffer(g_display);
		al_draw_bitmap(bmp->allegro_bitmap, 0, 0, 0);
		al_flip_display();
	}
}

// Vsync
int vsync(void)
{
	al_wait_for_vsync();
	return 0;
}

// File finder functions (stubs)
int al_findfirst(const char *pattern, struct al_ffblk *info, int attrib)
{
	// Stub implementation
	return -1;
}

int al_findnext(struct al_ffblk *info)
{
	// Stub implementation
	return -1;
}

void al_findclose(struct al_ffblk *info)
{
	// Stub implementation
}

// Get file extension
char *get_extension(const char *filename)
{
	if (!filename)
		return NULL;

	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return NULL;

	return (char *)(dot + 1);
}
