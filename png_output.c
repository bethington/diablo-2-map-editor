#include "allegro5_compat.h"
#include "png_output.h"
#include <stdio.h>
#include <string.h>

// PNG output implementation using Allegro 5.2.10 native PNG support
int save_png_a4_compat(const char *filename, BITMAP *bmp, PALETTE *pal)
{
	if (!bmp || !filename)
	{
		printf("Error: Invalid parameters for PNG save\n");
		return -1;
	}

	if (!bmp->al_bmp)
	{
		printf("Error: Invalid Allegro bitmap for PNG save\n");
		return -1;
	}

	// Convert filename from .pcx/.bmp to .png
	char png_filename[512];
	strcpy(png_filename, filename);

	// Find the last dot and replace extension
	char *dot = strrchr(png_filename, '.');
	if (dot)
	{
		strcpy(dot, ".png");
	}
	else
	{
		strcat(png_filename, ".png");
	}

	printf("Saving PNG screenshot: %s\n", png_filename);

	// Create directory if needed for tests/images folder
	if (strstr(png_filename, "tests\\images\\") == png_filename)
	{
// This is a test image, ensure directory exists
#ifdef _WIN32
		system("mkdir tests\\images 2>nul");
#else
		system("mkdir -p tests/images");
#endif
	}

	// Use Allegro 5.2.10's native PNG support via al_save_bitmap()
	// This function automatically handles PNG compression and format conversion
	bool success = al_save_bitmap(png_filename, bmp->al_bmp);

	if (success)
	{
		printf("PNG saved successfully: %s (using Allegro 5.2.10 native PNG support)\n", png_filename);
		return 0;
	}
	else
	{
		printf("Error: Failed to save PNG using al_save_bitmap(): %s\n", png_filename);
		return -1;
	}
}

// Legacy compatibility function
int save_png(const char *filename, BITMAP *bmp, PALETTE *pal)
{
	return save_png_a4_compat(filename, bmp, pal);
}

// Note: Removed unused utility functions:
// - save_png_with_options() (never called)
// - get_png_extension() (never called)
// - is_png_supported() (never called)
