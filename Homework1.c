#include <SDL2/SDL.h>

int main() {
    // Initialisierung von SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init fehlgeschlagen: %s\n", SDL_GetError());
        return 1;
    }

    // Erstellung des SDL-Fensters
    SDL_Window* window = SDL_CreateWindow(
        "SDL Fenster",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480, // Breite und Höhe des Fensters
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("SDL_CreateWindow fehlgeschlagen: %s\n", SDL_GetError());
        return 1;
    }

    // Hier können Sie weitere SDL-Initialisierungen durchführen oder mit dem Fenster interagieren

    // Schleife, die das Fenster geöffnet hält, bis der Benutzer es schließt
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }

    // Aufräumen und Beenden von SDL
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
