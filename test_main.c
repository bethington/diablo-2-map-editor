#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    printf("DS1 Editor Test Executable\n");
    printf("==========================\n");
    printf("Arguments received: %d\n", argc);
    
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }
    
    // Check if DS1_DEBUG environment variable is set
    char* debug_env = getenv("DS1_DEBUG");
    if (debug_env && strcmp(debug_env, "1") == 0) {
        printf("\n*** DEBUG MODE ENABLED ***\n");
    }
    
    printf("\nPress any key to continue...\n");
    getchar();
    return 0;
}
