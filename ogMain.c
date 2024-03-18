#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"
#include <SDL2/SDL_image.h>

int game_is_running = FAIL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
int tw, th; // texture width and height
int last_frame_time = 0;
int Mph = 600, Mpw = 600; //
int *ptrh = &Mph, *ptrw = &Mpw;
int x = 100, y = 200 ;
float delta_time;

void createMap(SDL_Renderer *renderer);

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
    texture = IMG_LoadTexture(renderer, IMGPATH); // loaded img
    SDL_QueryTexture(texture, NULL, NULL, ptrw, ptrh);
    SDL_Rect texture2;
    texture2.x = SCREEN_WIDTH / 2;
    texture2.y = SCREEN_HEIGHT / 2;
    return SUCCESS;
}

void update() {
    float delta_time = SDL_GetTicks() - last_frame_time / 1000.0f;
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
    SDL_Rect rect = {x, y, 100, 250};
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
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = FAIL;
        else if (event.key.keysym.sym == SDLK_RIGHT)
        x += 20 * delta_time;
        else if (event.key.keysym.sym == SDLK_LEFT)
        x -= 10;
        //else if (event.key.keysym.sym == SDLK_DOWN)
        break;
    }
}

void createMap(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void exit_game() { //funktion die programm schließt
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
    createMap(renderer);
    game_is_running = SUCCESS; // Setze game_is_running auf SUCCESS, um die Hauptschleife zu betreten

    while (game_is_running == SUCCESS) {
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
