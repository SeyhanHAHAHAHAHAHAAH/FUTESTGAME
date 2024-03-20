#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include "./definitionen.h"

int x, y, rx = 100, ry = 200, game_is_running = FAIL, lastmove;

SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL;
SDL_Texture* map_texture = NULL;
SDL_Texture* character_texture = NULL;
SDL_Texture* texture = NULL;
TTF_Font* font;

typedef struct {
    short life;
    float dy;
    int animFrame;
    bool jumping;
    SDL_Rect player_rect;
    SDL_Texture *texture;
} Player;

int initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL couldn't be initialized! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image couldn't be initialized! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init couldn't be initialized! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    font = TTF_OpenFont("resource/font.ttf", 10);
    if (font == NULL) {
        printf("font.tft couldn't be found! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        printf("Window couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
} 

int load_textures() {
    SDL_Surface *map_surface = IMG_Load("resource/map.png");
    if (map_surface == NULL) {
        printf("map.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    map_texture = SDL_CreateTextureFromSurface(renderer, map_surface);
    SDL_FreeSurface(map_surface);

    SDL_Surface* character_surface = IMG_Load("resource/standing.png");
    if (character_surface == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    character_texture = SDL_CreateTextureFromSurface(renderer, character_surface);
    SDL_FreeSurface(character_surface);

    return 0;
}

void jump(Player* player1) {
    if (!player1 -> jumping) {
        player1 -> dy = -10;
        player1 -> jumping = 1;
    }
}

void exit_game(Player *player) { 
    SDL_DestroyTexture(player -> texture);
    SDL_DestroyTexture(map_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void render(Player *player) {
    SDL_Color color = {0, 0, 0, 255};
    SDL_Rect recht = {140, 30, 60, 60};

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, map_texture, NULL, NULL);
    SDL_RenderCopy(renderer, player -> texture, NULL, &player ->player_rect);

    char hp[20]; // Assuming the maximum length of the integer is 20 characters
    snprintf(hp, sizeof(hp), "%d", player->life);
    SDL_Surface* surfacetext = TTF_RenderText_Solid(font, hp, color);
    
    surfacetext = TTF_RenderText_Solid(font, hp , color);
    SDL_Texture* texturetext = SDL_CreateTextureFromSurface(renderer, surfacetext);
    SDL_FreeSurface(surfacetext);
    SDL_RenderCopy(renderer, texturetext, NULL, &recht);
    SDL_DestroyTexture(texturetext);
    SDL_Rect ground = {0, 700, 1346, 100};
    SDL_Rect p1 = {218, 431, 300, 31};
    SDL_Rect p2 = {860, 431, 300, 31};
    SDL_Rect p3 = {525, 273, 299, 31};
    SDL_RenderPresent(renderer);
} 

bool checkCollision(Player* player1, SDL_Rect b) {
    return SDL_HasIntersection(&player1 -> player_rect, &b);
}

void inputs(Player* player1) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                exit_game(player1);
                exit(1);
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit_game(player1);
                        exit(1);
                        break;
                    case SDLK_SPACE:
                        jump(player1);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
        player1->player_rect.x -= MOVEMENT_SPEED;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        player1->player_rect.x += MOVEMENT_SPEED;
    }
    if (player1->jumping) {
        player1->dy += GRAVITY;
        player1->player_rect.y += player1->dy;
        if (player1->player_rect.y >= SCREEN_HEIGHT / 2 - 25) {
            player1->player_rect.y = SCREEN_HEIGHT / 2 - 25;
            player1->dy = 0;
            player1->jumping = 0;
        }
    }
}

int main() {
    if (initialize() != 0 || load_textures() != 0) {
        return 1;
    }

    Player player1;
    player1.player_rect.x = 100;
    player1.player_rect.y = 100;
    player1.player_rect.w = 50;
    player1.player_rect.h = 60;
    player1.texture = character_texture;
    player1.life = 5;
    player1.jumping = false;

    SDL_Rect rect = {rx, ry, 25, 40};
    SDL_Rect ground = {0,710, 1346, 100};
    SDL_Rect p1 = {226, 440, 255, 31};
    SDL_Rect p2 = {862, 440, 250, 31};
    SDL_Rect p3 = {525, 285, 299, 31};

    game_is_running = SUCCESS; 

    while (game_is_running == SUCCESS) {
        inputs(&player1);
        if (checkCollision(&player1, ground) || checkCollision(&player1, p1) || checkCollision(&player1, p2) || checkCollision(&player1, p3)) {
            switch (lastmove) {
                case 0:
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
        render(&player1);
    }
    exit_game(&player1);
    return 0;
}