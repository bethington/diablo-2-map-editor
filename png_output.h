#ifndef PNG_OUTPUT_H
#define PNG_OUTPUT_H

#include "allegro5_compat.h"

// PNG output function declarations using Allegro 5.2.10 native PNG support
int save_png(const char *filename, BITMAP *bmp, PALETTE *pal);
int save_png_a4_compat(const char *filename, BITMAP *bmp, PALETTE *pal);

// Note: Removed unused function declarations:
// - save_png_with_options()
// - get_png_extension()
// - is_png_supported()

// Update save macros to use Allegro 5.2.10 native PNG support
#ifdef USE_PNG_OUTPUT
#define save_pcx(file, bmp, pal) save_png_a4_compat(file, bmp, pal)
#define save_bitmap(file, bmp, pal) save_png_a4_compat(file, bmp, pal)
#define save_bmp(file, bmp, pal) save_png_a4_compat(file, bmp, pal)
#endif

#endif // PNG_OUTPUT_H
