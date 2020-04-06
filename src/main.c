#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    /*
    SDL_Surface* motherSurfaces[NUM_OF_MOTHER_SPRITES];
    SDL_Texture* motherTextures[NUM_OF_MOTHER_SPRITES];
    */
    SDL_Texture* spriteSheet;
    SDL_Surface* spriteSheetSurface, * frameCountSurface;
    SDL_Rect spriteSelect = {
        .x = 0,
        .y = 0,
        .w = 20,
        .h = 15
    }, mother = {
        .x = 0,
        .y = 0,
        .w = 1280 / 16,
        .h = 720 / 9
    };
    SDL_Event event;
    const Uint8 *keyboardState;
    char* basePath;
    char* resPath ;
    Uint32 ticksElapsed;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize TTF: %s", TTF_GetError());
        SDL_Quit();
    }

    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_RESIZABLE, &window, &renderer) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }
    
    basePath = SDL_GetBasePath();
    resPath = (char*) malloc(SDL_utf8strlen(basePath) + 256 + 4);
    sprintf_s(resPath, SDL_utf8strlen(basePath) + SDL_strlen("res\\spriteSheet.bmp") + 1,"%sres\\spriteSheet.bmp", basePath);
    
    spriteSheetSurface = SDL_LoadBMP_RW(SDL_RWFromFile(resPath, "rb"), 1); 

    if (!spriteSheetSurface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create sprite sheet surface from bitmap file: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 3;
    }

    SDL_SetColorKey(spriteSheetSurface, SDL_TRUE, SDL_MapRGB(spriteSheetSurface->format, 255, 255, 255));

    spriteSheet = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);

    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        ticksElapsed = SDL_GetTicks();
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        keyboardState = SDL_GetKeyboardState(NULL);
        if (keyboardState[SDL_SCANCODE_DOWN]) {
            mother.y++;
            spriteSelect.x = 0;
            SDL_RenderCopy(renderer, spriteSheet, &spriteSelect, &mother);
        }
        else if (keyboardState[SDL_SCANCODE_UP]) {
            mother.y--;
            spriteSelect.x = 40;
            SDL_RenderCopy(renderer, spriteSheet, &spriteSelect, &mother);
        }
        else if (keyboardState[SDL_SCANCODE_RIGHT]) {
            mother.x++;
            spriteSelect.x = 80;
            SDL_RenderCopy(renderer, spriteSheet, &spriteSelect, &mother);
        }
        else if (keyboardState[SDL_SCANCODE_LEFT]) {
            mother.x--;
            spriteSelect.x = 100;
            SDL_RenderCopy(renderer, spriteSheet, &spriteSelect, &mother);
        }
        else if (keyboardState[SDL_SCANCODE_ESCAPE]) break;
        else SDL_RenderCopy(renderer, spriteSheet, &spriteSelect, &mother);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_free(basePath);
    free(resPath);
    SDL_Quit();

    return 0;
}