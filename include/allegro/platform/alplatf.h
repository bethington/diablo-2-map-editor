#ifndef __al_included_allegro_platform_alplatf_h
#define __al_included_allegro_platform_alplatf_h

/* Define platform based on compiler */
#ifdef _MSC_VER
   #define ALLEGRO_MSVC
#endif

/* These are always defined now. */
#define ALLEGRO_NO_ASM
#define ALLEGRO_USE_C

#endif
