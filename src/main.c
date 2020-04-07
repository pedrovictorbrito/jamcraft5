#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define MOVE_DOWN(ENTITY, SPRITE_ONE, SPRITE_TWO) _move(&ticks_snapshot[0], ENTITY, &ENTITY.y, 10, renderer, sprite_sheet, SPRITE_ONE, SPRITE_TWO)

typedef enum {
    MOTHER_FACING_DOWN_CRYING = 0,
    MOTHER_FACING_DOWN_PLAIN = 20
} Sprite;

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

void _move(Uint32 *ticks_snapshot, SDL_Rect *entity, int *entity_property, int pixels_to_move, SDL_Renderer *renderer, SDL_Texture *sprite_sheet, Sprite first_sprite, Sprite second_sprite) {
    while(SDL_GetTicks() - *(ticks_snapshot + 1) < 500) {
        if ((SDL_GetTicks() - *ticks_snapshot - *(ticks_snapshot + 1)) >= 125) {
            *entity_property += pixels_to_move;
            ticks_snapshot[1] += 125;
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(first_sprite), entity);
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
        }
    }
    while(SDL_GetTicks() - *ticks_snapshot < 100) {
        if ((SDL_GetTicks() - *ticks_snapshot - *(ticks_snapshot + 1)) >= 125) {
            *entity_property += pixels_to_move;
            ticks_snapshot[1] += 125;
            SDL_RenderCopy(renderer, sprite_sheet, get_sprite(second_sprite), entity);
            SDL_RenderPresent(renderer);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* sprite_sheet_surface;
    SDL_Texture* sprite_sheet;
    SDL_Rect mother = {
        .x = 0,
        .y = 0,
        .w = 1280 / 16,
        .h = 720 / 9
    };
    SDL_Event event;
    Uint8 *keyboard_state;
    char* base_path;
    char* str_buffer;
    Uint32 ticks_snapshot[2];

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize TTF: %s", TTF_GetError());
        SDL_Quit();
    }

    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED,
        &window, &renderer) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

    base_path = SDL_GetBasePath();
    str_buffer = (char*) malloc(SDL_utf8strlen(base_path) + 4 + 256);
    snprintf(str_buffer, SDL_utf8strlen(base_path) + SDL_strlen("res?sprite_sheet.bmp") + 1, "%sres%csprite_sheet.bmp", base_path, PATH_SEPARATOR);

    sprite_sheet_surface = SDL_LoadBMP_RW(SDL_RWFromFile(str_buffer, "rb"), 1);

    if (!sprite_sheet_surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create sprite sheet surface from bitmap file: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 3;
    }

    SDL_SetColorKey(sprite_sheet_surface, SDL_TRUE, SDL_MapRGB(sprite_sheet_surface->format, 6, 6, 6));

    sprite_sheet = SDL_CreateTextureFromSurface(renderer, sprite_sheet_surface);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

    while (SDL_TRUE) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;

        SDL_RenderClear(renderer);

        keyboard_state = SDL_GetKeyboardState(NULL);

        ticks_snapshot[0] = SDL_GetTicks();
        ticks_snapshot[1] = 0;

        if (keyboard_state[SDL_SCANCODE_DOWN]) {
            MOVE_DOWN(mother, MOTHER_FACING_DOWN_CRYING, MOTHER_FACING_DOWN_PLAIN);
        /*
        else if (keyboard_state[SDL_SCANCODE_UP]) {
            mother.y -= 80;
            spriteSelect.x = 40;
            SDL_RenderCopy(renderer, sprite_sheet, &spriteSelec, &mother);
        }
        else if (keyboard_state[SDL_SCANCODE_RIGHT]) {
            mother.x += 80;
            spriteSelect.x = 80;
            SDL_RenderCopy(renderer, sprite_sheet, &spriteSelect, &mother);
        }
        else if (keyboard_state[SDL_SCANCODE_LEFT]) {
            mother.x -= 80;
            spriteSelect.x = 100;
            SDL_RenderCopy(renderer, sprite_sheet, &spriteSelect, &mother);
        } */
        else if (keyboard_state[SDL_SCANCODE_ESCAPE]) break;
        else SDL_RenderCopy(renderer, sprite_sheet, get_sprite(MOTHER_FACING_DOWN_CRYING), &mother);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_free(base_path);
    free(str_buffer);
    SDL_Quit();

    return 0;
}
