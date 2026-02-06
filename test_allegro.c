#include <stdio.h>
#include <allegro5/allegro.h>

int main()
{
	printf("Testing basic Allegro 5 functionality...\n");

	if (!al_init())
	{
		printf("ERROR: Failed to initialize Allegro!\n");
		return -1;
	}

	printf("SUCCESS: Allegro 5 initialized successfully!\n");
	printf("Version: %u\n", al_get_allegro_version());

	return 0;
}
