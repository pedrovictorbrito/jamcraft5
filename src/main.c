#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANIMATION_TIME 400
#define ANIMATION_KEY_FRAME_COUNT 8

#define EQUAL 0

#define MOVE_DOWN(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.y, tile_size, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_UP(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.y, -tile_size, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_RIGHT(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.x, tile_size, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)
#define MOVE_LEFT(ENTITY_ADDRESS, SPRITE_ONE, SPRITE_TWO) _move(ticks_snapshot, ENTITY_ADDRESS, ENTITY_ADDRESS.x, -tile_size, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)

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

void sdl_panic(char* msg) {
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

void _move(Uint32 ticks_snapshot, SDL_Rect *entity, int *entity_property, int tile_size, SDL_Renderer *renderer, SDL_Texture *sprite_sheet, Sprite first_sprite, Sprite second_sprite) {
    int ms_elapsed = 0;
    while(ms_elapsed < (ANIMATION_TIME / 2)) {
        if ((SDL_GetTicks() - ticks_snapshot - ms_elapsed) >= ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT) {
            *entity_property += tile_size / ANIMATION_KEY_FRAME_COUNT;
            ms_elapsed += ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT;
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(first_sprite), entity);
            SDL_RenderPresent(renderer);
        }
    }
    while(ms_elapsed < ANIMATION_TIME) {
        if ((SDL_GetTicks() - ticks_snapshot - ms_elapsed) >= ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT) {
            *entity_property += tile_size / ANIMATION_KEY_FRAME_COUNT;
            ms_elapsed += ANIMATION_TIME / ANIMATION_KEY_FRAME_COUNT;
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(second_sprite), entity);
            SDL_RenderPresent(renderer);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) sdl_panic("Couldn't initialize SDL: %s");
    atexit(SDL_Quit);

    if (TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't init TTF: %s", SDL_GetError());
        exit(3);
    }
    atexit(TTF_Quit);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    if (SDL_CreateWindowAndRenderer(display_mode.w, display_mode.h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED,
        &window, &renderer) == -1) sdl_panic("Couldn't create window and renderer: %s");
    atexit(destroy_window_and_renderer);

    char *base_path = SDL_GetBasePath();
    void *buffer = malloc(SDL_utf8strlen(base_path) + 4 + 256);
    char path_separator;
    if (strcmp(SDL_GetPlatform(), "Windows") == EQUAL) {
        printf("\nPlatform: %s\n", SDL_GetPlatform());
        path_separator = '\\';
    }
    else path_separator = '/';
    snprintf((char*)buffer, SDL_utf8strlen(base_path) + SDL_strlen("res?Minecraft.ttf") + 1, "%sres%cMinecraft.ttf", base_path, path_separator);
    TTF_Font *font = TTF_OpenFont(buffer, 16);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open font file Minecraft.ttf in directory res at application's base path: %s", SDL_GetError());
        exit(3);
    }
    snprintf((char*)buffer, SDL_utf8strlen(base_path) + SDL_strlen("res?sprite_sheet.bmp") + 1, "%sres%csprite_sheet.bmp", base_path, path_separator);

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
    int tile_size = display_mode.h / 9;

    printf("Display width: %d\nDisplay height: %d Tile lenght: %d", display_mode.w, display_mode.h, tile_size);

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
