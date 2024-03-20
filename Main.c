#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./definitionen.h"
#include <time.h>
#include <stdbool.h>



int x, y, rx = 100, ry = 200, game_is_running = FAIL, lastmove;
int tile_size = 16;
int bulletlive = 5;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* map_texture = NULL;
SDL_Texture* character_texture = NULL;
SDL_Texture* texture = NULL;
int facing;


typedef struct {
    short life;
    float dy;
    int animFrame;
    bool jumping;
    SDL_Rect player_rect;
    SDL_Texture *texture;
} Player;

typedef struct {
    float x, y;
    float dx, dy;
    int status; //zeigt ob kugel noch existiert
} Projektil;

Projektil bullet = {0};

int initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL couldn't be initialized! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }

    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image couldn't be initialized! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }

    window = SDL_CreateWindow("Gaming", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        printf("Window couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer couldn't be created! SDL_Error: %s\n", SDL_GetError());
        return FAIL;
    }

    return SUCCESS;
} 

int load_textures() {
    SDL_Surface *map_surface = IMG_Load("resource/map.png");
    if (map_surface == NULL) {
        printf("map.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return FAIL;
    }
    map_texture = SDL_CreateTextureFromSurface(renderer, map_surface);
    SDL_FreeSurface(map_surface);

    SDL_Surface* character_surface = IMG_Load("resource/standing.png");
    if (character_surface == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return FAIL;
    }
    character_texture = SDL_CreateTextureFromSurface(renderer, character_surface);
    SDL_FreeSurface(character_surface);

    return SUCCESS;
}

void jump(Player* player1) {
    if (!player1 -> jumping) {
        player1 -> dy= -7;
        player1 -> jumping = true;
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
    SDL_RenderClear(renderer); 
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_Rect ground = {0, 700, 1346, 100};
    SDL_Rect p1 = {218, 431, 300, 31};
    SDL_Rect p2 = {860, 431, 300, 31};
    SDL_Rect p3 = {525, 273, 299, 31};
    SDL_RenderCopy(renderer, map_texture, NULL, NULL);
    SDL_RenderCopy(renderer, player -> texture, NULL, &player -> player_rect);

    // Render the bullet
    if (bullet.status) {
        SDL_Rect bulletRect = { (int)bullet.x, (int)bullet.y, 10, 10 }; // Adjust the size as needed
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderFillRect(renderer, &bulletRect);
    }
    SDL_RenderPresent(renderer);
}

bool checkrectCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}
void fire_bullet(Player* player1) {
    if (!bullet.status) {
        bullet.x = player1->player_rect.x + (player1->player_rect.w / 2); // Spawn bullet at player's center horizontally
        bullet.y = player1->player_rect.y; // Spawn bullet at player's position vertically
        bullet.dx = 5; // Adjust bullet speed as needed
        bullet.dy = 0;
        bullet.status = 1; // Set bullet status to alive
    }
}
void update_bullet() {
    if (bullet.status) {
        bullet.x += bullet.dx;
        bullet.y += bullet.dy;
        if (bullet.x > SCREEN_WIDTH || bullet.x < 0 || bullet.y > SCREEN_HEIGHT || bullet.y < 0) {
            bullet.status = 0; // Destroy bullet if it goes off-screen
        }
    }
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
                        case SDLK_e:
                        fire_bullet(player1);
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
        facing = 0;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        player1->player_rect.x += MOVEMENT_SPEED;
        facing = 1;
    }
    if (player1->jumping) {
        player1->dy += GRAVITY;
        player1->player_rect.y += player1->dy;
        if (player1->player_rect.y >= SCREEN_HEIGHT / 2 - 25) {
            player1->player_rect.y = SCREEN_HEIGHT / 2 - 25;
            player1->dy = 0;
            player1->jumping = false;
        }
    }
}

int main() {
    time_t tStart, tEnd; 
    double ctime;// vergangene Zeit
    time(&tStart);
    if (initialize() != SUCCESS || load_textures() != SUCCESS) {
        return 1;
    }

    Player player1;
    player1.player_rect.x = 100;
    player1.player_rect.y = 100;
    player1.player_rect.w = 24;
    player1.player_rect.h = 24;
    player1.texture = character_texture;
    player1.life = 3;
    player1.jumping = false;

    game_is_running = SUCCESS; 

    while (game_is_running == SUCCESS) {
        inputs(&player1);
        update_bullet();
        render(&player1);
    }

    exit_game(&player1);
    return 0;
}