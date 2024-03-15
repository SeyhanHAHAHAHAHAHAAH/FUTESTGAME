#include <stdio.h>
#include <SDL2/SDL.h>

// Function prototyp für createMap()
void createMap(SDL_Renderer *renderer);

int main() {
    // Initialisierung für SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init fehlgeschlagen: %s\n", SDL_GetError());
        return 1;
    }

    // Erstellung SDL-Fenster
    SDL_Window* window = SDL_CreateWindow(
        "SDL Fenster",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 640, // Breite und Höhe 
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("SDL_CreateWindow fehlgeschlagen: %s\n", SDL_GetError());
        return 1;
    }

   SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
   if (renderer == NULL){
    printf("SDL_CreateRenderer fehlgeschlagen: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);//falls render fehlschlägt auch window schliessen
    SDL_Quit();
    return 1;
   }
   SDL_Surface *surface = IMG_Load("resources/hello.png");
   if (surface==NULL)
   {
    printf("SDL_Surface fehlgeschlagen: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit;
   }
    createMap(renderer);

    // Schleife die das fenster geöffnet hält bis benutzer es schließt
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }

    //Beenden von SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// Definition von createMap() function
void createMap(SDL_Renderer *renderer){

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); 
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect rect = {200, 200, 50, 50};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
}
