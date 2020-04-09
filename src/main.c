#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define ANIMATION_TIME 400
#define ANIMATION_KEY_FRAME_COUNT 4

#define MOVE_DOWN(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.y, 10, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_UP(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.y, -10, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_RIGHT(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.x, 10, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_LEFT(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.x, -10, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)

typedef enum {
    MOTHER_FACING_DOWN_CRYING = 0,
    MOTHER_FACING_DOWN_PLAIN = 20,
    MOTHER_FACING_UP_ONE = 40,
    MOTHER_FACING_UP_TWO = 60,
    MOTHER_FACING_RIGHT_WALKING = 80,
    MOTHER_FACING_RIGHT_STILL = 100,
    MOTHER_FACING_LEFT_WALKING = 120,
    MOTHER_FACING_LEFT_STILL = 140
} Sprite;

// Globals
typedef struct G {
    SDL_Window *window;
    SDL_Renderer *renderer;
} Global;

Global global;

void destroy_window_and_renderer() {
    SDL_DestroyRenderer(global.renderer);
    SDL_DestroyWindow(global.window);
}

inline void panic(char* msg) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg, SDL_GetError());
    exit(EXIT_FAILURE);
}

SDL_Rect* get_sprite(Sprite sprite) {
    static SDL_Rect buffer_rect = {
    .x = 0,
    .y = 0,
    .w = 20,
    .h = 15
    };
    buffer_rect.x = sprite;
    return &buffer_rect;
}

void _move(Uint32 ticks_snapshot, SDL_Rect *entity, int *entity_property, int pixels_to_move, SDL_Renderer *renderer, SDL_Texture *sprite_sheet, Sprite first_sprite, Sprite second_sprite) {
    int ms_elapsed = 0;
    while(ms_elapsed < (ANIMATION_TIME / 2)) {
        if ((SDL_GetTicks() - ticks_snapshot - ms_elapsed) >= ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT) {
            *entity_property += pixels_to_move;
            ms_elapsed += ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT;
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(first_sprite), entity);
            SDL_RenderPresent(renderer);
        }
    }
    while(ms_elapsed < ANIMATION_TIME) {
        if ((SDL_GetTicks() - ticks_snapshot - ms_elapsed) >= ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT) {
            *entity_property += pixels_to_move;
            ms_elapsed += ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT;
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(second_sprite), entity);
            SDL_RenderPresent(renderer);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) panic("Couldn't initialize SDL: %s");
    atexit(SDL_Quit);

    if (TTF_Init() == -1) panic("Couldn't initialize TTF: %s");
    atexit(TTF_Quit);

    SDL_Window window;
    SDL_Renderer renderer;
    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED,
        &window, &renderer) == -1) panic("Couldn't create window and renderer: %s");
    atexit(destroy_window_and_renderer);

    char *base_path = SDL_GetBasePath();
    void *buffer = malloc(SDL_utf8strlen(base_path) + 4 + 256);

    snprintf((char*)buffer, SDL_utf8strlen(base_path) + SDL_strlen("res?Minecraft.ttf") + 1, "%sres%cMinecraft.ttf", base_path, PATH_SEPARATOR);
    TTF_Font *font = TTF_OpenFont(buffer, 16);

    snprintf((char*)buffer, SDL_utf8strlen(base_path) + SDL_strlen("res?sprite_sheet.bmp") + 1, "%sres%csprite_sheet.bmp", base_path, PATH_SEPARATOR);

    SDL_Surface* sprite_sheet_surface = SDL_LoadBMP_RW(SDL_RWFromFile(buffer, "rb"), 1);

    if (!sprite_sheet_surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create sprite sheet surface from bitmap file: %s", SDL_GetError());
        return 3;
    }

    SDL_SetColorKey(sprite_sheet_surface, SDL_TRUE, SDL_MapRGB(sprite_sheet_surface->format, 6, 6, 6));

    SDL_Texture *sprite_sheet = SDL_CreateTextureFromSurface(renderer, sprite_sheet_surface);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

    SDL_Rect mother = {
        .x = 0,
        .y = 0,
        .w = 1280 / 16,
        .h = 720 / 9
    };

    SDL_Event event;
    Uint8 *keyboard_state;
    Uint32 ticks_snapshot;

    while (SDL_TRUE) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;

        SDL_RenderClear(renderer);

        keyboard_state = SDL_GetKeyboardState(NULL);

        ticks_snapshot = SDL_GetTicks();

        if (keyboard_state[SDL_SCANCODE_DOWN]) MOVE_DOWN(&mother, MOTHER_FACING_DOWN_PLAIN, MOTHER_FACING_DOWN_CRYING);
        else if (keyboard_state[SDL_SCANCODE_UP]) MOVE_UP(&mother, MOTHER_FACING_UP_ONE, MOTHER_FACING_UP_TWO);
        else if (keyboard_state[SDL_SCANCODE_RIGHT]) MOVE_RIGHT(&mother, MOTHER_FACING_RIGHT_WALKING, MOTHER_FACING_RIGHT_STILL);
        else if (keyboard_state[SDL_SCANCODE_LEFT]) MOVE_LEFT(&mother, MOTHER_FACING_LEFT_WALKING, MOTHER_FACING_LEFT_STILL);
        else if (keyboard_state[SDL_SCANCODE_ESCAPE]) break;
        else SDL_RenderCopy(renderer, sprite_sheet, get_sprite(MOTHER_FACING_DOWN_CRYING), &mother);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_free(base_path);
    free(buffer);
    SDL_Quit();

    return 0;
}
