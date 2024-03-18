#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"
#include <SDL2/SDL_image.h>

int game_is_running = FAIL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
int tw, th;// textur breite u höhe
int last_frame_time = 0;
int Mph = 600, Mpw = 600; //
int *ptrh = &Mph, *ptrw = &Mpw;

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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL; 
    
    }
    texture = IMG_LoadTexture(renderer,IMGPATH);// loaded img
    SDL_QueryTexture(texture ,NULL,NULL, ptrw , ptrh);
    SDL_Rect texture2; texture2.x = SCREEN_WIDTH/2; texture2.y = SCREEN_HEIGHT/2;
    return SUCCESS; 
    SDL_Rect rect; rect.x = 100; rect.y = 200; rect.w = 100; rect.h= 250;
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
}


void update() {
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME)); //locks game frame rate
    last_frame_time = SDL_GetTicks();
}

void render() {
    SDL_RenderClear(renderer); // render-Funktion ruft die createMap-Funktion auf
    SDL_RenderCopy(renderer,texture, NULL, NULL ); // aktualisiere den Renderer
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
            if(event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = FAIL;
            //else if (event.key.keysym.sym == SDLK_UP)
            //else if (event.key.keysym.sym == SDLK_RIGHT)
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
    if(initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
createMap(renderer);
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