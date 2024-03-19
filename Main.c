#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"
#include <SDL2/SDL_image.h>
int rx = 100;
int ry = 200; 
int tile_size = 16;
int game_is_running = FAIL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *surface = NULL;
SDL_Texture *texture = NULL;
int last_frame_time = 0;
float delta_time;
void read_map(const char* path, int map[SCREEN_HEIGHT][SCREEN_WIDTH]);
int solid_tile(int tile);

int initialize(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    window = SDL_CreateWindow("Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    surface = IMG_Load("resource/map.png");
    if (surface == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }
    int map[SCREEN_HEIGHT][SCREEN_WIDTH];
    read_map("resource/collision.csv", map);
    return 0;
}

void read_map(const char *path, int map[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("File couldn't be loaded: %s", path);
        exit(1);
    }
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            fscanf(fp, "%d", &map[y][x]);
        }
    }
    fclose(fp);
}

int solid_tile (int tile) {
    return tile != -1;
}

void update() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()- last_frame_time);
    if (time_to_wait > 0 && time_to_wait >= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
}
void render() {
    SDL_RenderClear(renderer); // render-Funktion ruft die createMap-Funktion auf
    SDL_RenderCopy(renderer, texture, NULL, NULL); // aktualisiere den Renderer
    // Draw a rectangle at position (100, 200) with width 100 and height 250
    SDL_Rect rect = {rx, ry, 23,100};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red (R,G,B,Alpha)
    SDL_RenderFillRect(renderer, &rect);             // Fill the rectangle with the current color
    SDL_RenderPresent(renderer);
}

void inputs() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = FAIL;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            game_is_running = FAIL;}
        else if (event.key.keysym.sym == SDLK_LEFT){
            rx -= 10;
        }  else if (event.key.keysym.sym == SDLK_RIGHT){
            rx += 10;}
        break;
        }
    }


void exit_game() { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
    game_is_running = SUCCESS; // Setze game_is_running auf SUCCESS, um die Hauptschleife zu betreten
    while (game_is_running == SUCCESS) {
        render();
        inputs();
        update();
    }
    exit_game();
    return 0;
}