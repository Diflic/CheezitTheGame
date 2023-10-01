#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <SDL.h>
#include "C:\devkitPro\portlibs\switch\include\SDL2\SDL_image.h"
#include <time.h>
#include <unistd.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <switch.h>


// some switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

#define SCREEN_W 1280
#define SCREEN_H 720


SDL_Texture * render_text(SDL_Renderer *renderer, const char* text, TTF_Font *font, SDL_Color color, SDL_Rect *rect) 
{
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}
SDL_Texture * render_img(SDL_Renderer *renderer, const char* image, SDL_Rect *rect) 
{
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = IMG_Load(image);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->w = surface->w;
    rect->h = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}





int main(int argc, char** argv)
{
    
    Result rc=0;
    romfsInit();
    chdir("romfs:/");

    // define varibles
    SDL_Texture *helloworld_tex = NULL, *cheezit_tex = NULL, *cLogo_tex = NULL, *Backdrop = NULL, *select_tex = NULL , *it_tex = NULL, *kit_tex = NULL;
    //SDL_Texture *boom_tex0 = NULL, *boom_tex1 = NULL, *boom_tex2 = NULL, *boom_tex3 = NULL, *boom_tex4 = NULL, *boom_tex5 = NULL, *boom_tex6 = NULL;
    //SDL_Texture *boom_tex7 = NULL, *boom_tex8 = NULL, *boom_tex9 = NULL, *boom_tex10 = NULL, *boom_tex11 = NULL, *boom_tex12 = NULL, *boom_tex13 = NULL;
    //SDL_Texture *boom_tex14 = NULL, *boom_tex15 = NULL, *boom_tex16 = NULL, *boom_tex17 = NULL;
    
    Mix_Music *music = NULL;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    Mix_Init(MIX_INIT_OGG);
    TTF_Init();

    int done = 0, w = 1920, h = 1080;
    int Screen = 0, ScreenNumber = 0;
    int move = 0, advance = 0;
    

    SDL_Window* window = SDL_CreateWindow("sdl2+mixer+image+ttf demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    
   
    // load font from romfs
    TTF_Font* font = TTF_OpenFont("data/ComicSansMS3.ttf", 36);

    // render text as texture
    SDL_Rect helloworld_rect = { 0, h - 740 - 38 + 210, 0, 0 };
    helloworld_tex = render_text(renderer, "press a to beggin", font, { 0, 0, 0, 255 }, &helloworld_rect);
    

    //cheezit images
    SDL_Rect Cheezit = { 500, h - 740, 600, 600 };
    cheezit_tex = render_img(renderer, "data/Cheezit.png", &Cheezit);

    SDL_Rect cheezitLogo = { 960 - 250, h - 740, 0,0};
    cLogo_tex = render_img(renderer, "data/cheezitLogo.png", &cheezitLogo);

    SDL_Rect Droprect = { 0, 0, 0,0};
    Backdrop = render_img(renderer, "data/CheezitIntroDrop2.png", &Droprect);

    SDL_Rect select = { 0, 0, 0,0};
    select_tex = render_img(renderer, "data/GameSelect.png", &select);

    SDL_Rect cheezit0 = { 0, 0, 0, 0};
    it_tex = render_img(renderer, "data/0.png", &cheezit0);

    SDL_Rect cheez0 = { 0, h - 500, 0, 0};
    kit_tex = render_img(renderer, "data/0.png", &cheez0);





    
        


    // no need to keep the font loaded
   

    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    
    
    
        


    
    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_AllocateChannels(5);
    Mix_OpenAudio(48000, AUDIO_S16, 2, 4096);
    


    music = Mix_LoadMUS("data/specture.ogg");
    if (music){
        Mix_PlayMusic(music, -1);
    }


    while (!done) {
       
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }
                        if (event.jbutton.button == JOY_A) {
                            // (a) button down
                            if(Screen == 0){
                                advance = 1;
                            }
                        }
                        if (event.jbutton.button == JOY_RIGHT) {
                            // (a) button down
                        

                            

                        }
                    }
                        
                    }
                    break;

            }
           
        
        // SDL_SetRenderDrawColor(renderer, color[0].r, color[0].g, color[0].b, color[0].a); //background
        SDL_RenderClear(renderer);

        // // fill window bounds
        // SDL_SetRenderDrawColor(renderer, 232, 126, 0, 255); // darkest shade of orange
        // SDL_GetWindowSize(window, &w, &h);
        // SDL_Rect f = {0, 0, w, h};
        // SDL_RenderFillRect(renderer, &f);

        
       

        // if (sdllogo_tex){
        //     SDL_RenderCopy(renderer, sdllogo_tex, NULL, &sdl_pos);
        // }
        // tells me how images move
        // sdl_pos.x += yeet;
        // if(sdl_pos.x > 1980 - 780){
        //     yeet = -10;
        //     if (sound[0])
        //             Mix_PlayChannel(-1, sound[0], 0);
        // }
        // if(sdl_pos.x < -10){
        //     yeet = 10;
        //     if (sound[0])
        //             Mix_PlayChannel(-1, sound[0], 0);
        // }
        Screen = ScreenNumber;


        
        // SDL_RenderCopy(renderer, Backdrop, NULL, &Droprect);
        // Mix_HaltChannel(-1);
        // Mix_FreeMusic(music);
        //SDL_DestroyTexture(select_tex);
        
      
            SDL_RenderCopy(renderer, Backdrop, NULL, &Droprect);
            if(advance == 1){
                ScreenNumber = 1;
            }
        
        if(Screen == 1){
            
        }

        SDL_RenderPresent(renderer);







        
    }
    //if (sdllogo_tex){SDL_DestroyTexture(sdllogo_tex);}
    if (cLogo_tex){SDL_DestroyTexture(cLogo_tex);}
    //if (sdllogo_tex){SDL_DestroyTexture(sdllogo_tex);}
        
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;




}