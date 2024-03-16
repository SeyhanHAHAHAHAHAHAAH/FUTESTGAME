#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"

int game_is_running = FAIL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;
void createMap(SDL_Renderer *renderer);

int initialize(void) { 
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
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
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

void render() {
    createMap(renderer); // render-Funktion ruft die createMap-Funktion auf
    SDL_RenderPresent(renderer); // aktualisiere den Renderer
}

void inputs() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            game_is_running = FAIL;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = FAIL;
            break;
    }
}

void createMap(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); 
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect rect = {200, 200, 50, 50};
    SDL_RenderFillRect(renderer, &rect);
}
 
int main() {
    if(initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
createMap(renderer);
game_is_running = SUCCESS; // Setze game_is_running auf SUCCESS, um die Hauptschleife zu betreten

    while(game_is_running == SUCCESS) {
        printf("1");
        inputs();
        update();
        render();
        
    }
    SDL_Event event;
        int running = SUCCESS;
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
            }
        }
    
    
    exit_game();
    return 0;
}