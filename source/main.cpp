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
//texture render sets text into a renderer
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



int rand_range(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}


int main(int argc, char** argv)
{
    
    int random = 0;
    SDL_Color colors[] = {
        { 128, 128, 128, 0 }, // gray
        { 255, 255, 255, 0 }, // white
        { 255, 0, 0, 0 },     // red
        { 0, 255, 0, 0 },     // green
        { 0, 0, 255, 0 },     // blue
        { 255, 255, 0, 0 },   // brown
        { 0, 255, 255, 0 },   // cyan
        { 255, 0, 255, 0 },   // purple
    };
    Result rc=0;
    romfsInit();
    chdir("romfs:/");

    // define varibles
    SDL_Texture *sdllogo_tex = NULL, *helloworld_tex = NULL, *helloworld_texs = NULL, *rock_tex = NULL, *forthe_tex = NULL, *cheezit_tex = NULL, *box_tex = NULL, *win_tex = NULL, *loose_tex = NULL;
    SDL_Rect sdl_pos = { 0, 1080 - 380, 0, 0}, rock_pos = { 0, 0, 0, 0}, cheezits = { 0, 0, 0, 0}, box_pos = { 1920 -376, 0, 0, 0}, win_pos = { 0, 0, 0, 0}, loose_pos = { 0, 0, 0, 0};
    Mix_Music *music = NULL;
    SDL_Event event;
    Mix_Chunk *sound[1] = { NULL },*sounds[1] = { NULL };
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    Mix_Init(MIX_INIT_OGG);
    TTF_Init();

    int done = 0, w = 1920, h = 1080;
    int speed = 0, move = 0, cspeed = 0, bspeed = 0, screen = 0, vroom = 0;
    int num = 0, max = 9000, numspeed = 10, winer = 0, loser = 0;
    int timer = 0;

    SDL_Window* window = SDL_CreateWindow("sdl2+mixer+image+ttf demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Surface *sdllogo = IMG_Load("data/god.png"); //first image 
    if (sdllogo) {
        sdl_pos.w = sdllogo->w;
        sdl_pos.h = sdllogo->h;
        sdllogo_tex = SDL_CreateTextureFromSurface(renderer, sdllogo);
        SDL_FreeSurface(sdllogo);
    }

    SDL_Surface *box_img = IMG_Load("data/CheezitLoader.png"); //second image
    if (box_img) {
        box_pos.w = box_img->w;
        box_pos.h = box_img->h;
        box_tex = SDL_CreateTextureFromSurface(renderer, box_img); //third image
        SDL_FreeSurface(box_img);
    }



    SDL_Surface *rocklogo = IMG_Load("data/CheezitIntroDrop2.png"); //fourth image
    if (rocklogo) {
        rock_pos.w = rocklogo->w;
        rock_pos.h = rocklogo->h;
        rock_tex = SDL_CreateTextureFromSurface(renderer, rocklogo);
        SDL_FreeSurface(rocklogo);
    }
    
    SDL_Surface *cheezit = IMG_Load("data/cheezit.png"); 
    if (cheezit) {
        cheezits.w = cheezit->w;
        cheezits.h = cheezit->h;
        cheezit_tex = SDL_CreateTextureFromSurface(renderer, cheezit);
        SDL_FreeSurface(cheezit);
    }

    SDL_Surface *win = IMG_Load("data/Winner.png"); 
    if (win) {
        win_pos.w = win->w;
        win_pos.h = win->h;
        win_tex = SDL_CreateTextureFromSurface(renderer, win);
        SDL_FreeSurface(win);
    }

    SDL_Surface *loose = IMG_Load("data/lose.png"); 
    if (loose) {
        loose_pos.w = loose->w;
        loose_pos.h = loose->h;
        loose_tex = SDL_CreateTextureFromSurface(renderer, loose);
        SDL_FreeSurface(loose);
    }



    // load font from romfs
    TTF_Font* font = TTF_OpenFont("data/ComicSansMS3.ttf", 36);

    // render text as texture
    
    // no need to keep the font loaded
    SDL_Rect helloworld_rect = { 0, SCREEN_H - 36, 0, 0 };
    helloworld_tex = render_text(renderer, "Press B to restart", font, { 255, 255, 255, 0 }, &helloworld_rect);
    SDL_Rect helloworld_rects = { 0, SCREEN_H - 36, 0, 0 };
    helloworld_texs = render_text(renderer, "Press A to continue", font, { 255, 255, 255, 0 }, &helloworld_rects);

    TTF_CloseFont(font);
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

    int nuke = 0;
    

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    Mix_AllocateChannels(5);
    Mix_OpenAudio(48000, AUDIO_S16, 2, 4096);

    // load music and sounds from files
    music = Mix_LoadMUS("data/specture.ogg");
    sounds[0] = Mix_LoadWAV("data/won.wav");
    sound[0] = Mix_LoadWAV("data/loser.wav");
    random = rand()%1000 + 1;
    if (music)
        Mix_PlayMusic(music, -1);
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
                    
                    if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }
                        if (event.jbutton.button == JOY_A) {
                            // (a) button down
                            
                                if (screen == 0){
                                    screen = 1;
                                    
                                }if (screen == 2){
                                    cspeed = 0;
                                    bspeed = 0;
                                    vroom = 100;
                                }
                                if (screen == 3){
                                    if(winer == 1){
                                        
                                    }
                                    WebCommonConfig config;
                                    WebCommonReply reply;
                                    // Create the config. There's a number of web*Create() funcs, see libnx web.h.
                                    // webPageCreate/webNewsCreate requires running under a host Application which has HtmlDocument content. When the host program is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
                                    rc = webPageCreate(&config, "https://www.youtube.com/embed/FTzAwe7CQ84");
                                    printf("webPageCreate(): 0x%x\n", rc);
                                    if (R_SUCCEEDED(rc)) {
                                        // At this point you can use any webConfigSet* funcs you want.
                                        rc = webConfigSetWhitelist(&config, "^http*"); // Set the whitelist, adjust as needed. If you're only using a single domain, you could remove this and use webNewsCreate for the above (see web.h for webNewsCreate).
                                        printf("webConfigSetWhitelist(): 0x%x\n", rc);
                                        if (R_SUCCEEDED(rc)) { // Launch the applet and wait for it to exit.
                                            printf("Running webConfigShow...\n");
                                            rc = webConfigShow(&config, &reply); // If you don't use reply you can pass NULL for it.
                                            printf("webConfigShow(): 0x%x\n", rc);
                                        }
                                    }
                                }
                                
                        }
                        if (event.jbutton.button == JOY_B) {
                            if (screen == 3){
                                    cspeed = 10;
                                    bspeed = 50;
                                    cheezits.x = 0;
                                    vroom = 0;
                                    if(loser == 1){
                                        //SDL_DestroyTexture(loose_tex);
                                        
                                        Mix_FreeChunk(sound[0]);
                                        loser = 0;
                                    }
                                    if(winer == 1){
                                        //SDL_DestroyTexture(win_tex);
                                        
                                        Mix_FreeChunk(sounds[0]);
                                        winer = 0;
                                    }
                                    screen = 2;
                                    
                                }
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

        if (rock_tex){
            SDL_RenderCopy(renderer, rock_tex, NULL, &rock_pos);
            
        }
        if(screen == 1){
        cspeed = 10;
        bspeed = 50;
        SDL_DestroyTexture(rock_tex);
       
        screen = 2;
        }
        if(screen == 2){
            
        SDL_RenderCopy(renderer, box_tex, NULL, &box_pos);
        SDL_RenderCopy(renderer, cheezit_tex, NULL, &cheezits);
        
        cheezits.y += cspeed;
        cheezits.x += vroom;
        box_pos.y += bspeed;
        if(cheezits.y > 1080-100){
            cspeed = -10;
            
        }
        if(cheezits.y < -10){
            cspeed = 10;
            
        }

        if(box_pos.y > 1080-100){
            bspeed = -50;
            
        }
        if(box_pos.y < -10){
            bspeed = 50;
            
        }
        if(cheezits.x >= box_pos.x){//if its at the x value
            if(cheezits.y >= box_pos.y ){ //if the cheezit is below the y value 
                if(cheezits.y <= box_pos.y + box_pos.h){//if its above the y value
                    
                    winer = 1;
                    screen = 3;
                   
                }else{
                    loser = 1;
                    screen = 3;
                }
            }else{
            loser = 1;
            screen = 3;
            }
            
        }
        }
        if(screen == 3){
            if (winer == 1){
            SDL_RenderCopy(renderer, win_tex, NULL, &win_pos);
            SDL_DestroyTexture(box_tex);
            SDL_DestroyTexture(cheezit_tex);
            Mix_PlayChannel(-1, sounds[0], 0);
            SDL_RenderCopy(renderer, helloworld_texs, NULL, &helloworld_rects);
            
            }
        if (loser == 1){
            SDL_RenderCopy(renderer, loose_tex, NULL, &loose_pos);
            Mix_PlayChannel(-1, sound[0], 0);
            SDL_RenderCopy(renderer, helloworld_tex, NULL, &helloworld_rect);
        }
        }

        
        SDL_RenderPresent(renderer);











        
    }
    if (sdllogo_tex){SDL_DestroyTexture(sdllogo_tex);}
        
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;




}