#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_MOTHER_SPRITES 6

struct GlobalState {
    Uint32 ticksEllapsed;
    const Uint32* keyboardState;
    Uint32 lastArrowBtnPressed;
};

enum MotherSprites { FRONT1, FRONT2, BACK1, BACK2, LEFT, RIGHT };

SDL_Texture* getMotherTexture(struct GlobalState globalState, SDL_Texture* textures[]) {
    if (globalState.lastArrowBtnPressed == SDL_SCANCODE_DOWN) {
        if ((globalState.ticksEllapsed / 1000) & 1 == 1) return textures[FRONT1];
        else return textures[FRONT2];
    }
    if (globalState.lastArrowBtnPressed == SDL_SCANCODE_UP) {
        if ((globalState.ticksEllapsed / 1000) & 1 == 1) return textures[BACK1];
        else return textures[BACK2];
    }
    if (globalState.lastArrowBtnPressed == SDL_SCANCODE_LEFT) return textures[LEFT];
    else return textures[RIGHT];
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* motherSurfaces[NUM_OF_MOTHER_SPRITES];
    SDL_Texture* motherTextures[NUM_OF_MOTHER_SPRITES];
    SDL_Surface* spriteSheet, * frameCountSurface;
    SDL_Rect motherSpriteDimensions = {
        .x = 0,
        .y = 0,
        .w = 15,
        .h = 14
    }, mother = {
        .x = 0,
        .y = 0,
        .w = 1280 / 16,
        .h = 720 / 9
    }, FPScounter = {
        .x = 1280 / 16 * 15,
        .y = 0,
        .w = 1280 / 16,
        .h = 720 / 9
    };
    SDL_Texture* motherFrontTexture, *currentFrame;
    SDL_Event event;
    register int frameCount = 0;
    char* frameCountStr = (char*)malloc(3 * sizeof(char));
    const Uint8 *keyboardState;
    char* basePath;
    char* resPath ;
    struct GlobalState globalState;

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
    
    spriteSheet = SDL_LoadBMP_RW(SDL_RWFromFile(resPath, "rb"), 1); 

    if (!spriteSheet) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create sprite sheet surface from bitmap file: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 3;
    }

    for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++, motherSpriteDimensions.x += 15) {
        motherSurfaces[sprite] = spriteSheet;
        SDL_SetClipRect(motherSurfaces[sprite], &motherSpriteDimensions);
    }
    for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++) {
        motherTextures[sprite] = SDL_CreateTextureFromSurface(renderer, motherSurfaces[sprite]);
    }

    //mother_front = surface;
    //SDL_BlitSurface(surface, &sprite, mother_front, NULL);
    //TTF_Init();
    // Font courtesy of Craftron Gaming
    sprintf_s(resPath, SDL_strlen(basePath) + SDL_strlen("res\\Minecraft.ttf") + 1, "%sres\\Minecraft.ttf", basePath);
    TTF_Font* font = TTF_OpenFont(resPath, 16);
    SDL_Color white = { 255, 255, 255 };

    //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

    //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

    for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++) {
        if (!motherTextures[sprite]) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++) {
                SDL_FreeSurface(motherSurfaces[sprite]);
            }
            SDL_Quit();
            return 3;
        }
    }
    for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++) {
        SDL_FreeSurface(motherSurfaces[sprite]);
    }

    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        globalState.ticksEllapsed = SDL_GetTicks();
        sprintf_s(frameCountStr, 3, "%d", globalState.ticksEllapsed / 1000);
        frameCountSurface = TTF_RenderText_Solid(font, frameCountStr, white);
        currentFrame = SDL_CreateTextureFromSurface(renderer, frameCountSurface);
        globalState.keyboardState = SDL_GetKeyboardState(NULL);
        if (globalState.keyboardState[SDL_SCANCODE_DOWN]) {
            mother.y++;
            globalState.lastArrowBtnPressed = SDL_SCANCODE_DOWN;
        }
        if (globalState.keyboardState[SDL_SCANCODE_UP]) {
            mother.y--;
            globalState.lastArrowBtnPressed = SDL_SCANCODE_UP;
        }
        if (globalState.keyboardState[SDL_SCANCODE_LEFT]) {
            mother.x--;
            globalState.lastArrowBtnPressed = SDL_SCANCODE_LEFT;
        }
        if (globalState.keyboardState[SDL_SCANCODE_RIGHT]) {
            mother.x++;
            globalState.lastArrowBtnPressed = SDL_SCANCODE_RIGHT;
        }
        if (globalState.keyboardState[SDL_SCANCODE_ESCAPE]) break;
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,
            currentFrame,
            NULL,
            &FPScounter);
        SDL_RenderCopy(renderer, getMotherTexture(globalState, motherTextures), NULL, &mother);
        SDL_RenderPresent(renderer);
    }

    for (int sprite = FRONT1; sprite < NUM_OF_MOTHER_SPRITES; sprite++)
        SDL_DestroyTexture(motherTextures[sprite]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //SDL_free(basePath);
    //free(resPath);
    SDL_Quit();

    return 0;
}