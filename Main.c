#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"

int game_is_running = FAIL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

int init(void) { 
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    window = SDL_CreateWindow("Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
    if(window == NULL) {      
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL) {
        printf("Window couldn't be rendered! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    return SUCCESS;
}

void exit_game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update() {
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME)); //locks game frame rate
    last_frame_time = SDL_GetTicks();
}

// Function prototyp für createMap()
void createMap(SDL_Renderer *renderer);

int main() {
    game_is_running = init();
    while(game_is_running) {
        SDL_Surface *surface = IMG_Load("resources/hello.png");
        if (surface==NULL) {
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
    }
    exit_game();
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
