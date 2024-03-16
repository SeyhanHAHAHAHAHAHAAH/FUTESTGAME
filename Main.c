#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./definitionen.h"

int game_is_running = FAIL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;
void createMap(SDL_Renderer *renderer);

typedef struct {
    int x, y, hp;
    char *name;
} Player;

void structs() {
    Player player;
    player.x = 50;
    player.y = 50;
    player.hp = 3;
    player.name = "player 1";
}

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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time); //sleep the exe until target frame time is reached (in milliseconds)
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f; //time elapsed since last frame converted to seconds
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
 
int main() {
    if(initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
    game_is_running = SUCCESS; // Setze game_is_running auf SUCCESS, um die Hauptschleife zu betreten
    while(game_is_running == SUCCESS) {
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