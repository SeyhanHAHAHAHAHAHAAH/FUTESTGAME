#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./definitionen.h"
#include <SDL2/SDL_image.h>
int x, y;
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
int lastmove; //speichert in welche richtung das letzte mal bewegt wurde

typedef struct {
    float x, y;
    float dx, dy;
    short life;
    int onLedge;
    int animFrame, facingLeft, slowingDown;
} Player;

typedef struct {
    Player player1;
    SDL_Texture *playerFrames[2];
    int time;
    SDL_Renderer *renderer;
} GameState;

void loadgame(GameState *game);

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
    return SUCCESS;
}

void exit_game() { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void loadgame(GameState *game) {
    surface = IMG_Load("resource/walking.png");
    if (surface == NULL) {
        printf("Cannot find standing.png!");
        exit_game();
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

void process(GameState *game) {
    game -> time++;
    Player *player1 = &game -> player1;
    player1 -> x += player1 -> dx;
    player1 -> y += player1 -> dy;
    if (player1 -> dx != 0 && player1 -> onLedge && !player1 -> slowingDown) {
        if (game -> time % 8 == 0){
            if (player1 -> animFrame == 0){
                player1 -> animFrame = 1;
            }
            else {
                player1 -> animFrame = 0;
            }
        }
    }
    player1 -> dy += GRAVITY;
}

void render(GameState *game) {
    SDL_RenderClear(renderer); 
    SDL_RenderCopy(renderer, texture, NULL, NULL); // aktualisiere den Renderer
    SDL_Rect rect = {rx, ry, 25,40};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red (R,G,B,Alpha)
    SDL_RenderFillRect(renderer, &rect);             // Fill the rectangle with the current color
    SDL_Rect ground = {0,759, 1346,-57};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
     SDL_RenderFillRect(renderer, &ground);
      SDL_RenderPresent(renderer); 
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void inputs(GameState *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                game_is_running = FAIL;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit_game();
                        break;
                    case SDLK_UP:
                        if(game -> player1.onLedge) {
                            game -> player1.dy = -8;
                            game -> player1.onLedge = 0;
                        }
                        break;
                }
                break;
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) {
        game -> player1.dy -= 0.2f;
    }
    if (state[SDL_SCANCODE_LEFT]) {
        game -> player1.dx -= 0.5;
        if (game -> player1.dx < -6) {
            game -> player1.dx = -6;
        }
        game -> player1.facingLeft = 1;
        game -> player1.slowingDown = 0;
    }
    else if(state[SDL_SCANCODE_RIGHT]) {
        game -> player1.dx += 0.5;
        if (game -> player1.dx > 6) {
            game -> player1.dx = 6;
        } 
        game -> player1.facingLeft = 0;
        game -> player1.slowingDown = 0;
    }
    else {
        game -> player1.animFrame = 0;
        game -> player1.dx *= 0.8f;
        game -> player1.slowingDown = 1;
        if (fabsf(game -> player1.dx) < 0.15) {
            game -> player1.dx  = 0;
        }
    }
}

int main() {
    GameState gameState;
    if (initialize() != SUCCESS) {
        return 1; // Beende das Programm mit einem Fehlercode, wenn die Initialisierung fehlschlägt
    }
    gameState.renderer = renderer;
    loadgame(&gameState);
    game_is_running = SUCCESS; // Setze game_is_running auf SUCCESS, um die Hauptschleife zu betreten
    while (game_is_running == SUCCESS) {
        render();
        inputs();
        update();
        SDL_Rect rect = {rx, ry, 23,100};
        SDL_Rect ground = {0,759, 1346,57};
        if (checkCollision(rect, ground)) {
            switch (lastmove)
            {case 0:
            rx -= 20;
            break;
            case 1:
            rx += 20;
            break;
            case 2:
            ry -= 20;
            break;
            }
        }
    }
    exit_game();
    return 0;
}