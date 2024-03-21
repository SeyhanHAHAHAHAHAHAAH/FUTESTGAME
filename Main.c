#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include "./definitionen.h"

int x, y;
int rx = 100, ry = 200;
int game_is_running = FAIL;
int lastmove, goblin_count = 0;
int points_count;
bool cancelgravity = false;
SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL;
SDL_Texture* map_texture = NULL, *character_texture_left = NULL, *character_texture_right = NULL, *character_texture_jump_left = NULL, *character_texture_jump_right = NULL, *goblin_texture = NULL, *texture = NULL, *bullet_texture = NULL;
TTF_Font* font;
int floorh = SCREEN_HEIGHT / 2 + 260;
int facing;

typedef struct {
    short life;
    float dy;
    int jumping;
    bool facing_left;
    SDL_Rect player_rect;
    SDL_Rect sprite_image;
    SDL_Texture *texture;
} Player;

void fire_bullet(Player* player1);

typedef struct {
    short goblin_life;
    int x, y;
    int goblin_side; //0 = rechts, 1 = links
    SDL_Rect goblin_rect;
    SDL_Texture *texture;
} Goblin;

typedef struct {
    float x, y;
    float dx, dy;
    int status; //zeigt ob kugel noch existiert
    SDL_Texture *texture;
    SDL_Rect bullet_rect;
} Projektil;

Projektil bullet = {0};

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

    window = SDL_CreateWindow("Frog Adventure", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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


    SDL_Surface *bullet_surface = IMG_Load("resource/bullet.png");
    if (bullet_surface == NULL) {
        printf("bullet.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    bullet_texture = SDL_CreateTextureFromSurface(renderer, bullet_surface);
    SDL_FreeSurface(bullet_surface);

    SDL_Surface* character_surface_left = IMG_Load("resource/walking_left.png");
    if (character_surface_left == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    character_texture_left = SDL_CreateTextureFromSurface(renderer, character_surface_left);
    SDL_FreeSurface(character_surface_left);

    SDL_Surface* character_surface_right = IMG_Load("resource/walking_right.png");
    if (character_surface_right == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    character_texture_right = SDL_CreateTextureFromSurface(renderer, character_surface_right);
    SDL_FreeSurface(character_surface_right);

    SDL_Surface* character_surface_jump_right = IMG_Load("resource/jump_right.png");
    if (character_surface_jump_right == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    character_texture_jump_right = SDL_CreateTextureFromSurface(renderer, character_surface_jump_right);
    SDL_FreeSurface(character_surface_jump_right);

    SDL_Surface* character_surface_jump_left= IMG_Load("resource/jump_left.png");
    if (character_surface_jump_left == NULL) {
        printf("standing.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    character_texture_jump_left = SDL_CreateTextureFromSurface(renderer, character_surface_jump_left);
    SDL_FreeSurface(character_surface_jump_left);


    SDL_Surface* goblin_surface = IMG_Load("resource/snail.png");
    if (goblin_surface == NULL) {
        printf("goblin.png couldn't be found! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }
    goblin_texture = SDL_CreateTextureFromSurface(renderer, goblin_surface);
    SDL_FreeSurface(goblin_surface);

    return 0;
}

void jump(Player* player1) {
    if ((player1 -> jumping) != 2) {
        player1 -> dy = -11;
        player1 -> jumping++;
    }
}

void spawn_goblin(Goblin** goblin) {
    *goblin = (Goblin*)malloc(sizeof(Goblin));
    if(*goblin == NULL) {
        printf("Couldn't allocate memory for goblin!");
        exit(1);
    }
    srand(time(NULL));
    int r = rand() % 2;
    if (r == 1) {
        (*goblin) -> x = 0;
        (*goblin) -> goblin_side = 1;
    }
    else {
        (*goblin) -> x = SCREEN_WIDTH;
        (*goblin) -> goblin_side = 0;
    }
    (*goblin) -> goblin_life = 1;   
    (*goblin) -> y = 635;
    (*goblin) -> goblin_rect.w = 90;
    (*goblin) -> goblin_rect.h = 65 ;
    (*goblin) -> goblin_rect.x = (*goblin) -> x;
    (*goblin) -> goblin_rect.y = (*goblin) -> y;
    (*goblin) -> texture = goblin_texture;
}

void move_goblin(Goblin** goblin) {
    srand(time(NULL));
    int r = (rand() % 15) + 3;
    if (*goblin != NULL) {
        if ((*goblin) -> goblin_side == 1) {
            (*goblin) -> x += r;
            (*goblin) -> goblin_rect.x = (*goblin)->x;  
        }
        else {
            (*goblin) -> x -= r;
            (*goblin) -> goblin_rect.x = (*goblin)->x;  
        }
        if ((*goblin) -> x > SCREEN_WIDTH || (*goblin) -> x < 0) {
            free(*goblin);
            *goblin = NULL;
        }
    }
}

void exit_game(Player *player) {
    TTF_CloseFont(font); 
    SDL_DestroyTexture(player -> texture);
    SDL_DestroyTexture(map_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void render(Player *player, Goblin *goblin, Projektil* bullet) {
    SDL_Color color = {0, 0, 0, 255};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, map_texture, NULL, NULL);

    SDL_Rect characterRect = player->player_rect;

    if (player->jumping) {
        characterRect.w *= 1.35;
        characterRect.h *= 1.35;
        SDL_RenderCopy(renderer, (player->facing_left) ? character_texture_jump_left : character_texture_jump_right, NULL, &characterRect);
    } 
    else {
        SDL_RenderCopy(renderer, player->texture, NULL, &characterRect);
    }
    
    if(goblin != NULL) {
        SDL_RenderCopy(renderer, goblin -> texture, NULL, &goblin -> goblin_rect);
    }

    if (bullet -> status) {
        SDL_RenderCopy(renderer, bullet->texture, NULL, &bullet -> bullet_rect);
       // SDL_Rect bulletRect = { (int)bullet.x, (int)bullet.y, 10, 10 }; // Adjust the size as needed
        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
       // SDL_RenderFillRect(renderer, &bulletRect);
    }

    char hp[3];
    snprintf(hp, sizeof(hp), "%d", player->life);
    SDL_Surface* surfacetext = TTF_RenderText_Solid(font, hp, color);
    SDL_Texture* texturetext = SDL_CreateTextureFromSurface(renderer, surfacetext);
    SDL_Rect hp_bar = {140, 30, 60, 60};
    SDL_FreeSurface(surfacetext);
    SDL_RenderCopy(renderer, texturetext, NULL, &hp_bar);

    char points[20]; 
    snprintf(points, sizeof(points), "Points: %d", points_count); 
    SDL_Surface* surfacepoints = TTF_RenderText_Solid(font, points, color);
    SDL_Texture* texturepoints = SDL_CreateTextureFromSurface(renderer, surfacepoints); 
    SDL_Rect points_rect = {SCREEN_WIDTH - 165, 25, 140, 60};
    SDL_FreeSurface(surfacepoints); 
    SDL_RenderCopy(renderer, texturepoints, NULL, &points_rect);
   
    SDL_DestroyTexture(texturepoints); 
    SDL_DestroyTexture(texturetext);
    SDL_Rect rect = {player->player_rect.x, player->player_rect.y, 24, 24};
    SDL_Rect ground = {0, 700, 1346, 100};
    SDL_Rect p1 = {218, 431, 300, 31};
    SDL_Rect p2 = {860, 431, 300, 31};
    SDL_Rect p3 = {525, 273, 299, 31};
    
    SDL_RenderPresent(renderer);
} 

bool checkrectCollision1(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}
bool checkrectCollision(Player* player1, SDL_Rect b) {
    return SDL_HasIntersection(&player1 -> player_rect, &b);
}
bool checkrectCollision2(Goblin* goblin, SDL_Rect b) {
    return SDL_HasIntersection(&goblin -> goblin_rect, &b);
}
void collision(Player *player1) {
    SDL_Rect rect = {player1->player_rect.x, player1->player_rect.y, 23, 100};
    SDL_Rect ground = {0, 759, 1346, -57};
    SDL_Rect p1 = {218, 411, 280, 31};
    SDL_Rect p2 = {860, 411, 280, 31};
    SDL_Rect p3 = {520, 300, 315, 200};// mittlere box, gravity switch
    SDL_Rect p4 = {0, 300, 210, 200};// linker grav switch
    SDL_Rect p5 = {1170, 300, 200, 200};
    if (checkrectCollision1(rect, ground)) {
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
        }}
   if (checkrectCollision1(rect,p3) ){
        player1-> jumping = true;
        floorh = SCREEN_HEIGHT/2+240;
    }
        if (checkrectCollision1(rect,p4) ){
        player1-> jumping = true;
        floorh = SCREEN_HEIGHT/2+240;
    }
            if (checkrectCollision1(rect,p5) ){
        player1-> jumping = true;
        floorh = SCREEN_HEIGHT/2+240;
    }
    if (checkrectCollision1(rect,p1) && rect.y > p1.y) {// checkt ob collision zwischen spieler und linke platform gibt und spieler über platform ist
         floorh = 380 ;
    }
    if (checkrectCollision1(rect,p2) && rect.y > p2.y) {// checkt ob collision zwischen spieler und rechte platform gibt und spieler über platform ist
        floorh = 380 ;
    }
}

void inputs(Player* player1) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                exit_game(player1);
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit_game(player1);
                        break;
                    case SDLK_SPACE:
                        jump(player1);
                        break;
                    case SDLK_RIGHT:
                        facing = 1;
                        break;
                    case SDLK_LEFT:
                        facing = 0;
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
        if ((player1 -> player_rect.x - MOVEMENT_SPEED) >= 10) {
            player1 -> player_rect.x -= MOVEMENT_SPEED;
            player1 -> texture = character_texture_left;
            player1 -> facing_left = true;
        }
    }

    if (state[SDL_SCANCODE_RIGHT]) {
        if (player1 -> player_rect.x + MOVEMENT_SPEED + player1 -> player_rect.w <= SCREEN_WIDTH - 10) {
            player1 -> player_rect.x += MOVEMENT_SPEED;
            player1 -> facing_left = false;
            player1 -> texture = character_texture_right;
        }
    }

    if (player1 -> jumping) {
        player1 -> dy += GRAVITY;
        player1 -> player_rect.y += player1 -> dy;
        player1 -> texture = player1 -> facing_left ? character_texture_jump_left : character_texture_jump_right;
        if (player1 -> player_rect.y >= floorh) {
            player1 -> player_rect.y = floorh;
            player1 -> dy = 0;
            player1 -> jumping = 0;
            player1 -> texture = player1 -> facing_left ? character_texture_left : character_texture_right;
        }
    }
}

void fire_bullet(Player* player1) {
    if (!bullet.status) {
        bullet.x = player1 -> player_rect.x + (player1 -> player_rect.w / 2); // 
        bullet.y = player1 -> player_rect.y + 20; //"bullet" wird im spieler gespawnt
        if (facing > 0) {
        bullet.dx = 10; //falls er nach rechts sich bewegt hat letztens
        }
        else {
            bullet.dx = -10;// falls er nach links sich bewegt hat letztens
        }
         
        bullet.dy = 0;
        bullet.status = 1; // Set bullet status to alive
    }
}

void update_bullet() {
    if (bullet.status) {
        bullet.x += bullet.dx;
        if (bullet.x > SCREEN_WIDTH || bullet.x < 0 || bullet.y > SCREEN_HEIGHT || bullet.y < 0) {
            bullet.status = 0; // Destroy bullet if it goes off-screen
        }
    }
}

int main() {
    if (initialize() != 0 || load_textures() != 0) {
        return 1;
    }

    Player player1;
    player1.jumping = false;
    player1.facing_left = true;
    player1.player_rect.x = SCREEN_WIDTH/2;
    player1.player_rect.y = 640;
    player1.player_rect.w = 50;
    player1.player_rect.h = 60;
    player1.texture = character_texture_left;
    player1.life = 4;

    Goblin* goblin = NULL;

    SDL_Rect rect = {rx, ry, 25, 40};
    SDL_Rect ground = {0,710, 1346, 100};
    SDL_Rect p1 = {226, 440, 255, 31};
    SDL_Rect p2 = {862, 440, 250, 31};
    SDL_Rect p3 = {525, 285, 299, 31};

    game_is_running = SUCCESS;

    while (game_is_running == SUCCESS) {
        if (goblin == NULL) {
            spawn_goblin(&goblin);
        }
        move_goblin(&goblin);
        inputs(&player1);
        collision(&player1);
        update_bullet();
        if (bullet.status && goblin != NULL) {
            SDL_Rect bulletRect = {(int)bullet.x, (int)bullet.y, 10, 10};
            if (checkrectCollision2(goblin, bulletRect)) {
                free(goblin); // töte goblin
                points_count++;
                goblin = NULL;
                bullet.status = 0; // lösch bullet
            }
        }
        render(&player1, goblin, &bullet);
    }
    exit_game(&player1);
    return 0;
}