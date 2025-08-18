#include "include/allegro.h"

// Stub implementations for missing Allegro functions
// Note: clear() and textout() are already defined in alcompat.h, so we comment them out
/*
void clear(BITMAP *bitmap) {
    // Simple clear function - just memset to 0
    if (bitmap && bitmap->line && bitmap->vtable) {
        int y;
        int bytes_per_pixel;
        // Use vtable's color_depth to calculate bytes per pixel
        switch (bitmap->vtable->color_depth) {
            case 8:  bytes_per_pixel = 1; break;
            case 15:
            case 16: bytes_per_pixel = 2; break;
            case 24: bytes_per_pixel = 3; break;
            case 32: bytes_per_pixel = 4; break;
            default: bytes_per_pixel = 1; break;
        }
        for (y = 0; y < bitmap->h; y++) {
            memset(bitmap->line[y], 0, bitmap->w * bytes_per_pixel);
        }
    }
}

void textout(BITMAP *bmp, const FONT *f, const char *str, int x, int y, int color) {
    // Stub - do nothing for now
    (void)bmp; (void)f; (void)str; (void)x; (void)y; (void)color;
}
*/

// Color conversion blitter stubs - return NULL to disable color conversion
void *_colorconv_blit_8_to_8 = NULL;
void *_colorconv_blit_8_to_15 = NULL;
void *_colorconv_blit_8_to_16 = NULL;
void *_colorconv_blit_8_to_24 = NULL;
void *_colorconv_blit_8_to_32 = NULL;
void *_colorconv_blit_15_to_8 = NULL;
void *_colorconv_blit_15_to_16 = NULL;
void *_colorconv_blit_15_to_24 = NULL;
void *_colorconv_blit_15_to_32 = NULL;
void *_colorconv_blit_16_to_8 = NULL;
void *_colorconv_blit_16_to_15 = NULL;
void *_colorconv_blit_16_to_24 = NULL;
void *_colorconv_blit_16_to_32 = NULL;
void *_colorconv_blit_24_to_8 = NULL;
void *_colorconv_blit_24_to_15 = NULL;
void *_colorconv_blit_24_to_16 = NULL;
void *_colorconv_blit_24_to_32 = NULL;
void *_colorconv_blit_32_to_8 = NULL;
void *_colorconv_blit_32_to_15 = NULL;
void *_colorconv_blit_32_to_16 = NULL;
void *_colorconv_blit_32_to_24 = NULL;
