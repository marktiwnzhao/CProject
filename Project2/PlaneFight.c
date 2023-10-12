#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include <stdio.h>
#include <stdlib.h>//生成随机数
#include <time.h>//time()

typedef struct image {
    SDL_Surface *Surface;
    SDL_Texture *Texture;
    SDL_Rect Rect;
} Image;

typedef struct font {
    TTF_Font *Font;
    SDL_Surface *Surface;
    SDL_Texture *Texture;
    SDL_Rect Rect;
    SDL_Color Color;
} Font;

//基本数据
#define Height 800
#define Width 480
#define MY_BULLET_NUM 50
#define NPC_1_NUM 20
#define NPC_2_NUM 6
#define NPC_3_NUM 3
#define NPC_BULLET_NUM 12
#define XRAY_NUM 5
#define MY_SPEED 5
#define MY_BULLET_SPEED 8
#define NPC_BULLET_SPEED 4
#define ITEM_SPEED 3
#define NPC_1_SPEED 3
#define NPC_2_SPEED 3
#define NPC_3_SPEED 1
#define BOSS_SPEED 2

int if_pause;
int if_end;
int if_quit;
int score;
int boss_coming;
int r[4];

struct ship {
    Image ship;
    Image life;
    Font life_num_TTF;
    int hp;
    int attack;
    int CD;
    int up;
    int down;
    int left;
    int right;
    int delay;
} my_ship;

struct bomb {
    Image Bomb;
    Font Bomb_num_TTF;
    int Bomb_num;
    int CD;
} Bomb;

struct Item {
    Image item;
    int if_live;
} Item[3];//物品: 0, 回复生命; 1, 子弹增多; 2, 炸弹

struct my_bullet {
    Image bullet;
    int if_live;
} my_bullet[MY_BULLET_NUM];

struct NPC_bullet {
    Image bullet;
    int if_live;
    int motion;
} NPC_bullet[NPC_BULLET_NUM];

struct Xray {
    Image Xray;
    int if_live;
} Xray[XRAY_NUM];

struct NPC_1 {
    Image NPC_1;
    int if_live;
} NPC_1[NPC_1_NUM];

struct NPC_2 {
    Image NPC_2;
    int hp;
} NPC_2[NPC_2_NUM];

struct NPC_3 {
    Image NPC_3;
    int hp;
    int motion;
} NPC_3[NPC_3_NUM];

struct boss {
    Image Boss;
    int hp;
    int Xray;
    int x;
} Boss;

Image BackGround;
Image Boss_hp;

Font Start_TTF;//开始界面文字
Font Score_TTF;//分数文字
Font Pause_TTF;//暂停界面文字
Font End_TTF;//结束界面文字
Font Tips[4];//提示文字: 0, 开始界面; 1, 暂停界面; 2, 结束界面; 3, 游戏界面
Font Round[4];//回合文字

Mix_Chunk *bgm;
Mix_Chunk *bomb_music;
Mix_Chunk *boss_music;
Mix_Chunk *get_bomb;
Mix_Chunk *get_bullet;
Mix_Chunk *get_hp;
Mix_Chunk *lose;
Mix_Chunk *win;
Mix_Chunk *shoot_music;
Mix_Chunk *NPC_1_dead;
Mix_Chunk *NPC_2_dead;

void Load();
void Init();
void Shoot();
void CreNPC_1();
void CreNPC_2();
void CreNPC_3();
void CreBoss();
void NPCShoot(int x, int y);
void UseBomb();
void DropItem(int x, int y);
void Print();
void Fresh();
void Pause(SDL_Event *MainEvent);
void End(SDL_Event *MainEvent);
void Free();
void Quit();

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;

int SDL_main(int argc, char *argv[]) {
    //基本工作
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    //create window
    Window = SDL_CreateWindow("PlaneFight", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height,
                              SDL_WINDOW_SHOWN);
    //create renderer
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event MainEvent;

    while(1) {
        //将文字图片加载
        Load();
        //基本数据初始化
        Init();
        //清屏
        SDL_RenderClear(Renderer);
        //打印背景
        SDL_RenderCopy(Renderer, BackGround.Texture, NULL, &BackGround.Rect);
        //打印开始界面
        SDL_RenderCopy(Renderer, Start_TTF.Texture, NULL, &Start_TTF.Rect);
        SDL_RenderCopy(Renderer, Tips[0].Texture, NULL, &Tips[0].Rect);
        SDL_RenderPresent(Renderer);
        while (SDL_WaitEvent(&MainEvent)) {
            if (MainEvent.type == SDL_KEYDOWN && MainEvent.key.keysym.sym == SDLK_RETURN) {//进入游戏
                Mix_PlayChannel(-1, bgm, -1);
                while (1) {
                    //清屏
                    SDL_RenderClear(Renderer);
                    //打印背景
                    SDL_RenderCopy(Renderer, BackGround.Texture, NULL, &BackGround.Rect);
                    //打印游戏界面
                    Print();
                    SDL_RenderPresent(Renderer);
                    while (SDL_PollEvent(&MainEvent)) {
                        switch (MainEvent.type) {//判断用户操作w, a, s, d, e, q, return
                            case SDL_KEYDOWN:
                                switch (MainEvent.key.keysym.sym) {
                                    case SDLK_p:
                                        if_pause = 1;
                                        break;
                                    case SDLK_SPACE:
                                        Shoot();
                                        break;
                                    case SDLK_e:
                                        UseBomb();
                                        break;
                                    case SDLK_a:
                                        my_ship.left = 1;
                                        break;
                                    case SDLK_d:
                                        my_ship.right = 1;
                                        break;
                                    case SDLK_w:
                                        my_ship.up = 1;
                                        break;
                                    case SDLK_s:
                                        my_ship.down = 1;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case SDL_KEYUP:
                                switch (MainEvent.key.keysym.sym) {
                                    case SDLK_a:
                                        my_ship.left = 0;
                                    case SDLK_d:
                                        my_ship.right = 0;
                                    case SDLK_w:
                                        my_ship.up = 0;
                                    case SDLK_s:
                                        my_ship.down = 0;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    //刷新
                    Fresh();
                    //if暂停->打印pause界面
                    if (if_pause) {
                        Pause(&MainEvent);
                    }
                    if (if_end) {
                        break;
                    }
                    //延迟展现动画
                    SDL_Delay(10);
                }
            } else if (MainEvent.type == SDL_KEYDOWN && MainEvent.key.keysym.sym == SDLK_q) {
                Free();
                Quit();
            }
            if (if_end) {
                //打印end界面
                End(&MainEvent);
                break;
            }
        }
        if (if_quit) {
            break;
        }
        Free();
    }
    Free();
    Quit();
}

//函数区

//加载
void Load() {
    //加载音乐
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048);
    bgm = Mix_LoadWAV("sounds/bgm.wav");
    bomb_music = Mix_LoadWAV("sounds/bomb.wav");
    boss_music = Mix_LoadWAV("sounds/boss.wav");
    get_bomb = Mix_LoadWAV("sounds/get_bomb.wav");
    get_bullet = Mix_LoadWAV("sounds/get_bullet.wav");
    get_hp = Mix_LoadWAV("sounds/get_hp.wav");
    lose = Mix_LoadWAV("sounds/lose.wav");
    NPC_1_dead = Mix_LoadWAV("sounds/NPC_1_dead.wav");
    NPC_2_dead = Mix_LoadWAV("sounds/NPC_2_dead.wav");
    shoot_music = Mix_LoadWAV("sounds/shoot.wav");
    win = Mix_LoadWAV("sounds/win.wav");
    //加载背景
    BackGround.Surface = IMG_Load("images/background.jpg");
    BackGround.Texture = SDL_CreateTextureFromSurface(Renderer, BackGround.Surface);
    BackGround.Rect.x = 0;
    BackGround.Rect.y = 0;
    BackGround.Rect.w = BackGround.Surface->w;
    BackGround.Rect.h = BackGround.Surface->h;
    //加载开始界面文字
    Start_TTF.Font = TTF_OpenFont("font/font.ttf", 70);
    Start_TTF.Color.a = 255;
    Start_TTF.Color.b = 255;
    Start_TTF.Color.g = 255;
    Start_TTF.Color.r = 255;
    Start_TTF.Surface = TTF_RenderUTF8_Blended(Start_TTF.Font, "PlaneFight!", Start_TTF.Color);
    Start_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Start_TTF.Surface);
    Start_TTF.Rect.x = Width / 2 - Start_TTF.Surface->w / 2;
    Start_TTF.Rect.y = Height / 2 - Start_TTF.Surface->h;
    Start_TTF.Rect.w = Start_TTF.Surface->w;
    Start_TTF.Rect.h = Start_TTF.Surface->h;
    //加载暂停界面文字
    Pause_TTF.Font = TTF_OpenFont("font/font.ttf", 70);
    Pause_TTF.Color.a = 255;
    Pause_TTF.Color.b = 255;
    Pause_TTF.Color.g = 255;
    Pause_TTF.Color.r = 255;
    Pause_TTF.Surface = TTF_RenderUTF8_Blended(Start_TTF.Font, "GamePaused.", Pause_TTF.Color);
    Pause_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Pause_TTF.Surface);
    Pause_TTF.Rect.x = Width / 2 - Pause_TTF.Surface->w / 2;
    Pause_TTF.Rect.y = Height / 2 - Pause_TTF.Surface->h;
    Pause_TTF.Rect.w = Pause_TTF.Surface->w;
    Pause_TTF.Rect.h = Pause_TTF.Surface->h;
    //加载结束界面文字
    End_TTF.Font = TTF_OpenFont("font/font.ttf", 70);
    End_TTF.Color.a = 255;
    End_TTF.Color.b = 255;
    End_TTF.Color.g = 255;
    End_TTF.Color.r = 255;
    End_TTF.Surface = TTF_RenderUTF8_Blended(End_TTF.Font, "GameOver!", End_TTF.Color);
    End_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, End_TTF.Surface);
    End_TTF.Rect.x = Width / 2 - End_TTF.Surface->w / 2;
    End_TTF.Rect.y = Height / 2 - End_TTF.Surface->h;
    End_TTF.Rect.w = End_TTF.Surface->w;
    End_TTF.Rect.h = End_TTF.Surface->h;
    //加载界面提示文字
    for (int i = 0; i < 3; i++) {
        Tips[i].Font = TTF_OpenFont("font/font.ttf", 30);
        Tips[i].Color.a = 255;
        Tips[i].Color.b = 34;
        Tips[i].Color.g = 139;
        Tips[i].Color.r = 34;
    }
    //开始界面
    Tips[0].Surface = TTF_RenderUTF8_Blended(Tips[0].Font, "Enter to start; Q to quit", Tips[0].Color);
    //暂停界面
    Tips[1].Surface = TTF_RenderUTF8_Blended(Tips[1].Font, "Enter to back; Q to quit", Tips[1].Color);
    //结束界面
    Tips[2].Surface = TTF_RenderUTF8_Blended(Tips[2].Font, "Enter to start; Q to quit", Tips[2].Color);
    for (int i = 0; i < 3; i++) {
        Tips[i].Texture = SDL_CreateTextureFromSurface(Renderer, Tips[i].Surface);
        Tips[i].Rect.x = Width / 2 - Tips[i].Surface->w / 2;
        Tips[i].Rect.y = Height / 4 * 3 - Tips[i].Surface->h;
        Tips[i].Rect.w = Tips[i].Surface->w;
        Tips[i].Rect.h = Tips[i].Surface->h;
    }
    //游戏界面
    Tips[3].Font = TTF_OpenFont("font/font.ttf", 20);
    Tips[3].Color.a = 255;
    Tips[3].Color.b = 255;
    Tips[3].Color.g = 255;
    Tips[3].Color.r = 255;
    Tips[3].Surface = TTF_RenderUTF8_Blended(Tips->Font, "P to pause the game", Tips[3].Color);
    Tips[3].Texture = SDL_CreateTextureFromSurface(Renderer, Tips[3].Surface);
    Tips[3].Rect.w = Tips[3].Surface->w;
    Tips[3].Rect.h = Tips[3].Surface->h;
    //加载回合提示文字
    for (int i = 0; i < 4; i++) {
        Round[i].Font = TTF_OpenFont("font/font.ttf", 30);
        Round[i].Color.a = 255;
        Round[i].Color.b = 0;
        Round[i].Color.g = 0;
        Round[i].Color.r = 255;
    }
    Round[0].Surface = TTF_RenderUTF8_Blended(Round[0].Font, "Round 1!", Round[0].Color);
    Round[1].Surface = TTF_RenderUTF8_Blended(Round[1].Font, "Round 2!", Round[1].Color);
    Round[2].Surface = TTF_RenderUTF8_Blended(Round[2].Font, "Round 3!", Round[2].Color);
    Round[3].Surface = TTF_RenderUTF8_Blended(Round[3].Font, "Danger!", Round[3].Color);
    for (int i = 0; i < 4; i++) {
        Round[i].Texture = SDL_CreateTextureFromSurface(Renderer, Round[i].Surface);
        Round[i].Rect.x = Width - Round[i].Surface->w;
        Round[i].Rect.y = 0;
        Round[i].Rect.w = Round[i].Surface->w;
        Round[i].Rect.h = Round[i].Surface->h;
    }
    //加载分数
    Score_TTF.Font = TTF_OpenFont("font/font.ttf", 40);
    Score_TTF.Rect.x = 10;
    Score_TTF.Rect.y = 10;
    Score_TTF.Color.a = 255;
    Score_TTF.Color.b = 0;
    Score_TTF.Color.g = 0;
    Score_TTF.Color.r = 255;
    //加载我的飞机
    my_ship.ship.Surface = IMG_Load("images/my_ship.png");
    my_ship.ship.Texture = SDL_CreateTextureFromSurface(Renderer, my_ship.ship.Surface);
    my_ship.ship.Rect.w = my_ship.ship.Surface->w;
    my_ship.ship.Rect.h = my_ship.ship.Surface->h;
    my_ship.ship.Rect.x = Width / 2 - my_ship.ship.Surface->w / 2;
    my_ship.ship.Rect.y = Height - my_ship.ship.Surface->h;

    my_ship.life.Surface = IMG_Load("images/my_life.png");
    my_ship.life.Texture = SDL_CreateTextureFromSurface(Renderer, my_ship.life.Surface);
    my_ship.life.Rect.x = Width - 2 * my_ship.life.Surface->w;
    my_ship.life.Rect.y = Height - 2 * my_ship.life.Surface->h;
    my_ship.life.Rect.w = my_ship.life.Surface->w * 2;
    my_ship.life.Rect.h = my_ship.life.Surface->h * 2;

    my_ship.life_num_TTF.Font = TTF_OpenFont("font/font.ttf", 25);
    my_ship.life_num_TTF.Color.a = 255;
    my_ship.life_num_TTF.Color.b = 255;
    my_ship.life_num_TTF.Color.g = 255;
    my_ship.life_num_TTF.Color.r = 255;
    //加载物品
    Item[0].item.Surface = IMG_Load("images/life_item.png");
    Item[0].item.Texture = SDL_CreateTextureFromSurface(Renderer, Item[0].item.Surface);
    Item[0].item.Rect.w = Item[0].item.Surface->w;
    Item[0].item.Rect.h = Item[0].item.Surface->h;

    Item[1].item.Surface = IMG_Load("images/bullet_item.png");
    Item[1].item.Texture = SDL_CreateTextureFromSurface(Renderer, Item[1].item.Surface);
    Item[1].item.Rect.w = Item[1].item.Surface->w;
    Item[1].item.Rect.h = Item[1].item.Surface->h;

    Item[2].item.Surface = IMG_Load("images/bomb_item.png");
    Item[2].item.Texture = SDL_CreateTextureFromSurface(Renderer, Item[2].item.Surface);
    Item[2].item.Rect.w = Item[2].item.Surface->w;
    Item[2].item.Rect.h = Item[2].item.Surface->h;
    //加载我的子弹
    for (int i = 0; i < MY_BULLET_NUM; i++) {
        my_bullet[i].bullet.Surface = IMG_Load("images/my_bullet.png");
        my_bullet[i].bullet.Texture = SDL_CreateTextureFromSurface(Renderer, my_bullet[i].bullet.Surface);
        my_bullet[i].bullet.Rect.x = 0;
        my_bullet[i].bullet.Rect.y = 0;
        my_bullet[i].bullet.Rect.w = my_bullet[i].bullet.Surface->w;
        my_bullet[i].bullet.Rect.h = my_bullet[i].bullet.Surface->h;
    }
    //加载我的炸弹
    Bomb.Bomb.Surface = IMG_Load("images/bomb.png");
    Bomb.Bomb.Texture = SDL_CreateTextureFromSurface(Renderer, Bomb.Bomb.Surface);
    Bomb.Bomb.Rect.x = 0;
    Bomb.Bomb.Rect.y = Height - Bomb.Bomb.Surface->h;
    Bomb.Bomb.Rect.w = Bomb.Bomb.Surface->w;
    Bomb.Bomb.Rect.h = Bomb.Bomb.Surface->h;

    Bomb.Bomb_num_TTF.Font = TTF_OpenFont("font/font.ttf", 25);
    Bomb.Bomb_num_TTF.Color.a = 255;
    Bomb.Bomb_num_TTF.Color.b = 255;
    Bomb.Bomb_num_TTF.Color.g = 255;
    Bomb.Bomb_num_TTF.Color.r = 255;
    //加载NPC的子弹
    for (int i = 0; i < NPC_BULLET_NUM; i++) {
        NPC_bullet[i].bullet.Surface = IMG_Load("images/NPC_bullet.png");
        NPC_bullet[i].bullet.Texture = SDL_CreateTextureFromSurface(Renderer, NPC_bullet[i].bullet.Surface);
        NPC_bullet[i].bullet.Rect.x = 0;
        NPC_bullet[i].bullet.Rect.y = 0;
        NPC_bullet[i].bullet.Rect.w = NPC_bullet[i].bullet.Surface->w;
        NPC_bullet[i].bullet.Rect.h = NPC_bullet[i].bullet.Surface->h;
    }
    //加载Xray
    for (int i = 0; i < XRAY_NUM; i++) {
        Xray[i].Xray.Surface = IMG_Load("images/Xray.png");
        Xray[i].Xray.Texture = SDL_CreateTextureFromSurface(Renderer, Xray[i].Xray.Surface);
        Xray[i].Xray.Rect.x = 0;
        Xray[i].Xray.Rect.y = 0;
        Xray[i].Xray.Rect.w = Xray[i].Xray.Surface->w;
        Xray[i].Xray.Rect.h = Xray[i].Xray.Surface->h;
    }
    //加载NPC_1
    for (int i = 0; i < NPC_1_NUM; i++) {
        NPC_1[i].NPC_1.Surface = IMG_Load("images/NPC_1.png");
        NPC_1[i].NPC_1.Texture = SDL_CreateTextureFromSurface(Renderer, NPC_1[i].NPC_1.Surface);
        NPC_1[i].NPC_1.Rect.x = 0;
        NPC_1[i].NPC_1.Rect.y = 0;
        NPC_1[i].NPC_1.Rect.w = NPC_1[i].NPC_1.Surface->w;
        NPC_1[i].NPC_1.Rect.h = NPC_1[i].NPC_1.Surface->h;
    }
    //加载NPC_2
    for (int i = 0; i < NPC_2_NUM; i++) {
        NPC_2[i].NPC_2.Surface = IMG_Load("images/NPC_2.png");
        NPC_2[i].NPC_2.Texture = SDL_CreateTextureFromSurface(Renderer, NPC_2[i].NPC_2.Surface);
        NPC_2[i].NPC_2.Rect.x = 0;
        NPC_2[i].NPC_2.Rect.y = 0;
        NPC_2[i].NPC_2.Rect.w = NPC_2[i].NPC_2.Surface->w;
        NPC_2[i].NPC_2.Rect.h = NPC_2[i].NPC_2.Surface->h;
    }
    //加载NPC_3
    for (int i = 0; i < NPC_3_NUM; i++) {
        NPC_3[i].NPC_3.Surface = IMG_Load("images/NPC_3.png");
        NPC_3[i].NPC_3.Texture = SDL_CreateTextureFromSurface(Renderer, NPC_3[i].NPC_3.Surface);
        NPC_3[i].NPC_3.Rect.x = 0;
        NPC_3[i].NPC_3.Rect.y = 0;
        NPC_3[i].NPC_3.Rect.w = NPC_3[i].NPC_3.Surface->w;
        NPC_3[i].NPC_3.Rect.h = NPC_3[i].NPC_3.Surface->h;
    }
    //加载boss
    Boss.Boss.Surface = IMG_Load("images/boss.png");
    Boss.Boss.Texture = SDL_CreateTextureFromSurface(Renderer, Boss.Boss.Surface);
    Boss.Boss.Rect.x = 0;
    Boss.Boss.Rect.y = 0;
    Boss.Boss.Rect.w = Boss.Boss.Surface->w;
    Boss.Boss.Rect.h = Boss.Boss.Surface->h;
    //加载boss血条
    Boss_hp.Surface = IMG_Load("images/boss_hp.png");
    Boss_hp.Texture = SDL_CreateTextureFromSurface(Renderer, Boss_hp.Surface);
    Boss_hp.Rect.x = 0;
    Boss_hp.Rect.y = 0;
    Boss_hp.Rect.w = Boss_hp.Surface->w;
    Boss_hp.Rect.h = Boss_hp.Surface->h;
}
//初始化
void Init() {
    if_pause = 0;
    if_end = 0;
    if_quit = 0;
    score = 0;
    boss_coming = 1;
    my_ship.hp = 15;
    my_ship.left = 0;
    my_ship.right = 0;
    my_ship.up = 0;
    my_ship.down = 0;
    my_ship.attack = 1;
    my_ship.CD = 0;
    my_ship.delay = 0;
    Bomb.Bomb_num = 5;
    Bomb.CD = 0;
    for (int i = 0; i < 3; i++) {
        Item[i].if_live = 0;
    }
    for (int i = 0; i < MY_BULLET_NUM; i++) {
        my_bullet[i].if_live = 0;
    }
    for (int i = 0; i < NPC_BULLET_NUM; i++) {
        NPC_bullet[i].if_live = 0;
        NPC_bullet[i].motion = 0;
    }
    for (int i = 0; i < XRAY_NUM; i++) {
        Xray[i].if_live = 0;
    }
    for (int i = 0; i < NPC_1_NUM; i++) {
        NPC_1[i].if_live = 0;
    }
    for (int i = 0; i < NPC_2_NUM; i++) {
        NPC_2[i].hp = 0;
    }
    for (int i = 0; i < NPC_3_NUM; i++) {
        NPC_3[i].hp = 0;
        NPC_3[i].motion = 0;
    }
    Boss.hp = 150;
    Boss.Xray = 0;
    Boss.x = 0;
    for (int i = 0; i < 4; i++) {
        r[i] = 0;
    }
    srand(time(NULL));
}
//生成敌人
void CreNPC_1() {
    for (int i = 0; i < NPC_1_NUM; i++) {
        if (NPC_1[i].if_live == 0) {
            NPC_1[i].if_live = 1;
            NPC_1[i].NPC_1.Rect.x = rand() % Width - NPC_1[i].NPC_1.Surface->w / 2;
            NPC_1[i].NPC_1.Rect.y = - NPC_1[i].NPC_1.Surface->h;
            break;
        }
    }
}

void CreNPC_2() {
    for (int i = 0; i < NPC_2_NUM; i++) {
        if (NPC_2[i].hp <= 0) {
            NPC_2[i].hp = 4;
            NPC_2[i].NPC_2.Rect.x = rand() % Width - NPC_2[i].NPC_2.Surface->w / 2;
            NPC_2[i].NPC_2.Rect.y = - NPC_2[i].NPC_2.Surface->h;
            break;
        }
    }
}

void CreNPC_3() {
    for (int i = 0; i < NPC_3_NUM; i++) {
        if (NPC_3[i].hp <= 0) {
            NPC_3[i].hp = 7;
            NPC_3[i].NPC_3.Rect.x = rand() % Width - NPC_3[i].NPC_3.Surface->w / 3;
            NPC_3[i].NPC_3.Rect.y = - NPC_3[i].NPC_3.Surface->h;
            if (NPC_3[i].NPC_3.Rect.x < Width / 2 - NPC_3[i].NPC_3.Surface->w) {
                NPC_3[i].motion = 1;
            } else {
                NPC_3[i].motion = -1;
            }
            break;
        }
    }
}

void CreBoss() {
    Mix_PlayChannelTimed(-1, boss_music, 0, -1);
    Boss.Boss.Rect.x = Width / 2 - Boss.Boss.Surface->w / 2;
    Boss.Boss.Rect.y = - Boss.Boss.Surface->h;
}
//开火
void Shoot() {
    if (my_ship.CD == 0) {
        Mix_PlayChannelTimed(-1, shoot_music, 0, -1);
        if (my_ship.attack == 1) {
            for (int i = 0; i < MY_BULLET_NUM; i++) {
                if (my_bullet[i].if_live == 0) {
                    my_bullet[i].if_live = 1;
                    my_bullet[i].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w / 2;
                    my_bullet[i].bullet.Rect.y = my_ship.ship.Rect.y;
                    break;
                }
            }
        } else if (my_ship.attack == 2) {
            for (int i = 0; i < MY_BULLET_NUM - 1; i++) {
                if (my_bullet[i].if_live + my_bullet[i + 1].if_live == 0) {
                    my_bullet[i].if_live = 1;
                    my_bullet[i + 1].if_live = 1;
                    my_bullet[i].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w / 3;
                    my_bullet[i + 1].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w  * 2 / 3;
                    my_bullet[i].bullet.Rect.y = my_ship.ship.Rect.y + 15;
                    my_bullet[i + 1].bullet.Rect.y = my_ship.ship.Rect.y + 15;
                    break;
                }
            }
        } else if (my_ship.attack == 3) {
            for (int i = 0; i < MY_BULLET_NUM - 2; i++) {
                if (my_bullet[i].if_live + my_bullet[i + 1].if_live + my_bullet[i + 2].if_live == 0) {
                    my_bullet[i].if_live = 1;
                    my_bullet[i + 1].if_live = 1;
                    my_bullet[i + 2].if_live = 1;
                    my_bullet[i].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w / 4;
                    my_bullet[i + 1].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w / 2;
                    my_bullet[i + 2].bullet.Rect.x = my_ship.ship.Rect.x + my_ship.ship.Surface->w / 4 * 3;
                    my_bullet[i].bullet.Rect.y = my_ship.ship.Rect.y + 25;
                    my_bullet[i + 1].bullet.Rect.y = my_ship.ship.Rect.y;
                    my_bullet[i + 2].bullet.Rect.y = my_ship.ship.Rect.y + 25;
                    break;
                }
            }
        }
        my_ship.CD = 15;
    }
}
//敌人开火
void NPCShoot(int x, int y) {
    for (int i = 0; i < NPC_BULLET_NUM - 2; i++) {
        if (NPC_bullet[i].if_live + NPC_bullet[i + 1].if_live + NPC_bullet[i + 2].if_live == 0) {
            NPC_bullet[i].if_live = 1;
            NPC_bullet[i + 1].if_live = 1;
            NPC_bullet[i + 2].if_live = 1;
            NPC_bullet[i].motion = -1;
            NPC_bullet[i + 1].motion = 0;
            NPC_bullet[i + 2].motion = 1;
            NPC_bullet[i].bullet.Rect.x = x;
            NPC_bullet[i + 1].bullet.Rect.x = x;
            NPC_bullet[i + 2].bullet.Rect.x = x;
            NPC_bullet[i].bullet.Rect.y = y;
            NPC_bullet[i + 1].bullet.Rect.y = y;
            NPC_bullet[i + 2].bullet.Rect.y = y;
            break;
        }
    }
}
//掉落物品
void DropItem(int x, int y) {
    int s = rand() % 10;
    if (Item[0].if_live + Item[1].if_live + Item[2].if_live == 0) {
        if (s < 5) {//概率0.5
            Item[0].if_live = 1;
            Item[0].item.Rect.x = x;
            Item[0].item.Rect.y = y;
        } else if (s < 7) {//概率0.2
            Item[1].if_live = 1;
            Item[1].item.Rect.x = x;
            Item[1].item.Rect.y = y;
        } else {//概率0.3
            Item[2].if_live = 1;
            Item[2].item.Rect.x = x;
            Item[2].item.Rect.y = y;
        }
    }
}
//使用炸弹
void UseBomb() {
    if (Bomb.Bomb_num && Bomb.CD == 0) {
        Mix_PlayChannelTimed(-1, bomb_music, 0, -1);
        for (int i = 0; i < NPC_1_NUM; i++) {
            if (NPC_1[i].if_live) {
                score += 10;
                NPC_1[i].if_live = 0;
                if (rand() % 1000 < 90) {
                    DropItem(NPC_1[i].NPC_1.Rect.x, NPC_1[i].NPC_1.Rect.y);
                }
            }
        }
        for (int i = 0; i < NPC_2_NUM; i++) {
            if (NPC_2[i].hp > 0) {
                NPC_2[i].hp -= 3;
                if (NPC_2[i].hp <= 0) {
                    score += 20;
                    NPC_2[i].hp = 0;
                    if (rand() % 1000 < 120) {
                        DropItem(NPC_2[i].NPC_2.Rect.x, NPC_2[i].NPC_2.Rect.y);
                    }
                }
            }
        }
        for (int i = 0; i < NPC_3_NUM; i++) {
            if (NPC_3[i].hp > 0) {
                NPC_3[i].hp -= 3;
                if (NPC_3[i].hp <= 0) {
                    score += 40;
                    NPC_3[i].hp = 0;
                    if (rand() % 1000 < 150) {
                        DropItem(NPC_3[i].NPC_3.Rect.x, NPC_3[i].NPC_3.Rect.y);
                    }
                }
            }
        }
        if (r[3]) {
            Boss.hp -= 3;
            if (Boss.hp <= 0) {
                Boss.hp = 0;
                score += 1000;
                if_end = 1;
            }
        }
        Bomb.Bomb_num--;
        Bomb.CD = 20;
    }
}

//打印游戏界面
void Print() {
    //打印炸弹及数量
    SDL_RenderCopy(Renderer, Bomb.Bomb.Texture, NULL, &Bomb.Bomb.Rect);
    char bomb_num_str[15];
    sprintf(bomb_num_str, "X %d", Bomb.Bomb_num);
    Bomb.Bomb_num_TTF.Surface = TTF_RenderUTF8_Blended(Bomb.Bomb_num_TTF.Font, bomb_num_str, Bomb.Bomb_num_TTF.Color);
    Bomb.Bomb_num_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Bomb.Bomb_num_TTF.Surface);
    Bomb.Bomb_num_TTF.Rect.x = Bomb.Bomb.Rect.x + Bomb.Bomb.Surface->w;
    Bomb.Bomb_num_TTF.Rect.y = Bomb.Bomb.Rect.y + (Bomb.Bomb.Surface->h - Bomb.Bomb_num_TTF.Surface->h) / 2;
    Bomb.Bomb_num_TTF.Rect.w = Bomb.Bomb_num_TTF.Surface->w;
    Bomb.Bomb_num_TTF.Rect.h = Bomb.Bomb_num_TTF.Surface->h;
    SDL_RenderCopy(Renderer, Bomb.Bomb_num_TTF.Texture, NULL, &Bomb.Bomb_num_TTF.Rect);
    SDL_FreeSurface(Bomb.Bomb_num_TTF.Surface);
    SDL_DestroyTexture(Bomb.Bomb_num_TTF.Texture);

    //打印生命符号及生命值
    SDL_RenderCopy(Renderer, my_ship.life.Texture, NULL, &my_ship.life.Rect);
    char hp_str[15];
    sprintf(hp_str, "%d X", my_ship.hp);
    my_ship.life_num_TTF.Surface = TTF_RenderUTF8_Blended(my_ship.life_num_TTF.Font, hp_str, my_ship.life_num_TTF.Color);
    my_ship.life_num_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, my_ship.life_num_TTF.Surface);
    my_ship.life_num_TTF.Rect.w = my_ship.life_num_TTF.Surface->w;
    my_ship.life_num_TTF.Rect.h = my_ship.life_num_TTF.Surface->h;
    my_ship.life_num_TTF.Rect.x = my_ship.life.Rect.x - my_ship.life_num_TTF.Surface->w;
    my_ship.life_num_TTF.Rect.y = my_ship.life.Rect.y + (my_ship.life.Rect.h - my_ship.life_num_TTF.Surface->h) / 2;
    SDL_RenderCopy(Renderer, my_ship.life_num_TTF.Texture, NULL, &my_ship.life_num_TTF.Rect);
    SDL_FreeSurface(my_ship.life_num_TTF.Surface);
    SDL_DestroyTexture(my_ship.life_num_TTF.Texture);

    //打印回合
    for (int i = 0; i < 4; i++) {
        if (r[i]) {
            SDL_RenderCopy(Renderer, Round[i].Texture, NULL, &Round[i].Rect);
            break;
        }
    }
    //打印我方飞机
    SDL_RenderCopy(Renderer, my_ship.ship.Texture, NULL, &my_ship.ship.Rect);
    //打印我方子弹
    for (int i = 0; i < MY_BULLET_NUM; i++) {
        if(my_bullet[i].if_live) {
            SDL_RenderCopy(Renderer, my_bullet[i].bullet.Texture, NULL, &my_bullet[i].bullet.Rect);
        }
    }
    //打印NPC的子弹
    for (int i = 0; i < NPC_BULLET_NUM; i++) {
        if (NPC_bullet[i].if_live) {
            SDL_RenderCopy(Renderer, NPC_bullet[i].bullet.Texture, NULL, &NPC_bullet[i].bullet.Rect);
        }
    }
    //打印Xray
    for (int i = 0; i < XRAY_NUM; i++) {
        if(Xray[i].if_live) {
            SDL_RenderCopy(Renderer, Xray[i].Xray.Texture, NULL, &Xray[i].Xray.Rect);
            Xray[i].if_live = 0;
        }
    }
    //打印NPC(包括boss)
    for (int i = 0; i < NPC_1_NUM; i++) {
        if (NPC_1[i].if_live) {
            SDL_RenderCopy(Renderer, NPC_1[i].NPC_1.Texture, NULL, &NPC_1[i].NPC_1.Rect);
        }
    }
    for (int i = 0; i < NPC_2_NUM; i++) {
        if (NPC_2[i].hp > 0) {
            SDL_RenderCopy(Renderer, NPC_2[i].NPC_2.Texture, NULL, &NPC_2[i].NPC_2.Rect);
        }
    }
    for (int i = 0; i < NPC_3_NUM; i++) {
        if (NPC_3[i].hp > 0) {
            SDL_RenderCopy(Renderer, NPC_3[i].NPC_3.Texture, NULL, &NPC_3[i].NPC_3.Rect);
        }
    }
    if (r[3]) {
        SDL_RenderCopy(Renderer, Boss.Boss.Texture, NULL, &Boss.Boss.Rect);
        Boss_hp.Rect.w = (int) (1.0 * Boss_hp.Surface->w * Boss.hp / 150);//boss血条
        SDL_RenderCopy(Renderer, Boss_hp.Texture, NULL, &Boss_hp.Rect);
    }
    //打印道具
    for (int i = 0; i < 3; i++) {
        if (Item[i].if_live) {
            SDL_RenderCopy(Renderer, Item[i].item.Texture, NULL, &Item[i].item.Rect);
        }
    }
    //打印分数及提示符
    char score_str[15];
    sprintf(score_str, "Score: %d", score);
    Score_TTF.Surface = TTF_RenderUTF8_Blended(Score_TTF.Font, score_str, Score_TTF.Color);
    Score_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Score_TTF.Surface);
    Score_TTF.Rect.w = Score_TTF.Surface->w;
    Score_TTF.Rect.h = Score_TTF.Surface->h;
    SDL_RenderCopy(Renderer, Score_TTF.Texture, NULL, &Score_TTF.Rect);
    Tips[3].Rect.x = 10;
    Tips[3].Rect.y = Score_TTF.Rect.y + Score_TTF.Surface->h + 10;
    SDL_RenderCopy(Renderer, Tips[3].Texture, NULL, &Tips[3].Rect);
    SDL_FreeSurface(Score_TTF.Surface);
    SDL_DestroyTexture(Score_TTF.Texture);
}
//刷新
void Fresh() {
    //判断是否结束
    if (Boss.hp <= 0) {
        Mix_PlayChannelTimed(-1, win, 0, -1);
        if_end = 1;
        return;
    }
    if (my_ship.hp <= 0) {
        Mix_PlayChannelTimed(-1, lose, 0, -1);
        if_end = 1;
        return;
    }
    //飞机移动并限制范围
    if (my_ship.left && my_ship.ship.Rect.x >= - my_ship.ship.Surface->w / 2) {
        my_ship.ship.Rect.x -= MY_SPEED;
    }
    if (my_ship.right && my_ship.ship.Rect.x <= Width - my_ship.ship.Surface->w / 2) {
        my_ship.ship.Rect.x += MY_SPEED;
    }
    if (my_ship.up && my_ship.ship.Rect.y >= Height / 2 - my_ship.ship.Surface->h / 2) {
        my_ship.ship.Rect.y -= MY_SPEED;
    }
    if (my_ship.down && my_ship.ship.Rect.y <= Height - my_ship.ship.Surface->h) {
        my_ship.ship.Rect.y += MY_SPEED;
    }

    if (my_ship.CD) {
        my_ship.CD--;
    }
    if (Bomb.CD) {
        Bomb.CD--;
    }
    if (my_ship.delay) {
        my_ship.delay--;
    }
    //我的子弹移动
    for (int i = 0; i < MY_BULLET_NUM; i++) {
        if (my_bullet[i].if_live) {
            if (my_bullet[i].bullet.Rect.y >= 18) {
                my_bullet[i].bullet.Rect.y -= MY_BULLET_SPEED;
            } else {
                my_bullet[i].if_live = 0;
                continue;
            }
            //判断子弹是否击中敌人
            for (int j = 0; j < NPC_1_NUM; j++) {
                if (NPC_1[j].if_live && my_bullet[i].if_live &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w * 2 / 3 >= NPC_1[j].NPC_1.Rect.x &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w / 3 <= NPC_1[j].NPC_1.Rect.x + NPC_1[j].NPC_1.Surface->w &&
                    my_bullet[i].bullet.Rect.y <= NPC_1[j].NPC_1.Rect.y + NPC_1[j].NPC_1.Surface->h &&
                    my_bullet[i].bullet.Rect.y + my_bullet[i].bullet.Surface->h >= NPC_1[j].NPC_1.Rect.y ) {
                    Mix_PlayChannelTimed(-1, NPC_1_dead, 0, -1);
                    my_bullet[i].if_live = 0;
                    NPC_1[j].if_live = 0;
                    score += 10;
                    if (rand() % 1000 < 90) {
                        DropItem(NPC_1[j].NPC_1.Rect.x, NPC_1[j].NPC_1.Rect.y);
                    }
                    break;
                }
            }
            for (int j = 0; j < NPC_2_NUM; j++) {
                if (NPC_2[j].hp && my_bullet[i].if_live &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w * 2 / 3 >= NPC_2[j].NPC_2.Rect.x &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w / 3 <= NPC_2[j].NPC_2.Rect.x + NPC_2[j].NPC_2.Surface->w &&
                    my_bullet[i].bullet.Rect.y <= NPC_2[j].NPC_2.Rect.y + NPC_2[j].NPC_2.Surface->h &&
                    my_bullet[i].bullet.Rect.y + my_bullet[i].bullet.Surface->h >= NPC_2[j].NPC_2.Rect.y) {
                    my_bullet[i].if_live = 0;
                    NPC_2[j].hp--;
                    if (NPC_2[j].hp <= 0) {
                        Mix_PlayChannelTimed(-1, NPC_2_dead, 0, -1);
                        NPC_2[j].hp = 0;
                        score += 20;
                        if (rand() % 1000 < 120) {
                            DropItem(NPC_2[j].NPC_2.Rect.x, NPC_2[j].NPC_2.Rect.y);
                        }
                    }
                    break;
                }
            }
            for (int j = 0; j < NPC_3_NUM; j++) {
                if (NPC_3[j].hp && my_bullet[i].if_live &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w * 2 / 3 >= NPC_3[j].NPC_3.Rect.x &&
                    my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w / 3 <= NPC_3[j].NPC_3.Rect.x + NPC_3[j].NPC_3.Surface->w &&
                    my_bullet[i].bullet.Rect.y <= NPC_3[j].NPC_3.Rect.y + NPC_3[j].NPC_3.Surface->h - 15 &&
                    my_bullet[i].bullet.Rect.y + my_bullet[i].bullet.Surface->h >= NPC_3[j].NPC_3.Rect.y) {
                    my_bullet[i].if_live = 0;
                    NPC_3[j].hp--;
                    if (NPC_3[j].hp <= 0) {
                        Mix_PlayChannelTimed(-1, NPC_2_dead, 0, -1);
                        NPC_3[j].hp = 0;
                        score += 40;
                        if (rand() % 1000 < 150) {
                            DropItem(NPC_3[j].NPC_3.Rect.x, NPC_3[j].NPC_3.Rect.y);
                        }
                    }
                    break;
                }
            }
            if (r[3] && my_bullet[i].if_live &&
                my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w * 2 / 3 >= Boss.Boss.Rect.x &&
                my_bullet[i].bullet.Rect.x + my_bullet[i].bullet.Surface->w / 3 <= Boss.Boss.Rect.x + Boss.Boss.Surface->w &&
                my_bullet[i].bullet.Rect.y <= Boss.Boss.Rect.y + Boss.Boss.Surface->h - 70 &&
                my_bullet[i].bullet.Rect.y + my_bullet[i].bullet.Surface->h >= Boss.Boss.Rect.y) {
                my_bullet[i].if_live = 0;
                Boss.hp--;
                if (Boss.hp <= 0) {
                    Mix_PlayChannelTimed(-1, win, 0, -1);
                    Boss.hp = 0;
                    score += 1000;
                    if_end = 1;
                }
            }
        }
    }
    //生成敌人
    int s = rand() % 1000;
    if (score < 400) {
        //round 1
        r[0] = 1;
        r[1] = 0;
        r[2] = 0;
        r[3] = 0;
        if (s < 20) {
            CreNPC_1();
        }
    } else if (score < 900) {
        //round 2;
        r[0] = 0;
        r[1] = 1;
        r[2] = 0;
        r[3] = 0;
        if (s < 15) {
            CreNPC_1();
        } else if (s < 20) {
            CreNPC_2();
        }
    } else if (score < 1500) {
        //round 3
        r[0] = 0;
        r[1] = 0;
        r[2] = 1;
        r[3] = 0;
        if (s < 12) {
            CreNPC_1();
        } else if (s < 17) {
            CreNPC_2();
        } else if (s < 20) {
            CreNPC_3();
        }
    } else {
        //boss
        r[0] = 0;
        r[1] = 0;
        r[2] = 0;
        r[3] = 1;
        if (r[3] && boss_coming) {
            CreBoss();
            boss_coming = 0;//只创造一个boss
        }
        if (s < 10) {
            CreNPC_1();
        } else if (s < 13) {
            CreNPC_2();
        } else if (s < 15) {
            CreNPC_3();
        }
    }
    //NPC移动
    for (int i = 0; i < NPC_1_NUM; i++) {
        if (NPC_1[i].if_live) {
            if (NPC_1[i].NPC_1.Rect.y <= Height + NPC_1[i].NPC_1.Surface->h / 6) {
                NPC_1[i].NPC_1.Rect.y += NPC_1_SPEED;
            } else {
                NPC_1[i].if_live = 0;
                continue;
            }
            if (NPC_1[i].NPC_1.Rect.x + NPC_1[i].NPC_1.Surface->w * 3 / 4 >= my_ship.ship.Rect.x + my_ship.ship.Surface->w / 10 &&
                NPC_1[i].NPC_1.Rect.x + NPC_1[i].NPC_1.Surface->w / 4 <= my_ship.ship.Rect.x + my_ship.ship.Surface->w * 9 / 10 &&
                NPC_1[i].NPC_1.Rect.y + NPC_1[i].NPC_1.Surface->h >= my_ship.ship.Rect.y + my_ship.ship.Surface->h / 6 &&
                NPC_1[i].NPC_1.Rect.y <= my_ship.ship.Rect.y + my_ship.ship.Surface->h * 6 / 5) {
                Mix_PlayChannelTimed(-1, NPC_1_dead, 0, -1);
                NPC_1[i].if_live = 0;
                my_ship.hp--;
                if (my_ship.attack > 1) {
                    my_ship.attack--;
                }
                if (my_ship.hp <= 0) {
                    Mix_PlayChannelTimed(-1, lose, 0, -1);
                    my_ship.hp = 0;
                    if_end = 1;
                }
            }
        }
    }
    for (int i = 0; i < NPC_2_NUM; i++) {
        if (NPC_2[i].hp) {
            if (NPC_2[i].NPC_2.Rect.y <= Height + NPC_2[i].NPC_2.Surface->h / 9) {
                NPC_2[i].NPC_2.Rect.y += NPC_2_SPEED;
            } else {
                NPC_2[i].hp = 0;
                continue;
            }
            if (NPC_2[i].NPC_2.Rect.x + NPC_2[i].NPC_2.Surface->w * 3 / 4 >= my_ship.ship.Rect.x + my_ship.ship.Surface->w / 6 &&
                NPC_2[i].NPC_2.Rect.x + NPC_2[i].NPC_2.Surface->w / 4 <= my_ship.ship.Rect.x + my_ship.ship.Surface->w * 5 / 6 &&
                NPC_2[i].NPC_2.Rect.y + NPC_2[i].NPC_2.Surface->h >= my_ship.ship.Rect.y + 10 &&
                NPC_2[i].NPC_2.Rect.y <= my_ship.ship.Rect.y + my_ship.ship.Surface->h) {
                Mix_PlayChannelTimed(-1, NPC_2_dead, 0, -1);
                NPC_2[i].hp = 0;
                my_ship.hp -= 2;
                if (my_ship.attack > 1) {
                    my_ship.attack--;
                }
                if (my_ship.hp <= 0) {
                    Mix_PlayChannelTimed(-1, lose, 0, -1);
                    my_ship.hp = 0;
                    if_end = 1;
                }
            }
        }
    }
    for (int i = 0; i < NPC_3_NUM; i++) {
        if (NPC_3[i].hp) {
            NPC_3[i].NPC_3.Rect.x += NPC_3[i].motion * NPC_3_SPEED;
            NPC_3[i].NPC_3.Rect.y += NPC_3_SPEED;
            if (NPC_3[i].NPC_3.Rect.y > Height) {
                NPC_3[i].hp = 0;
                continue;
            }
            if (NPC_3[i].NPC_3.Rect.x < 0) {
                NPC_3[i].motion = 1;
            }
            if (NPC_3[i].NPC_3.Rect.x > Width - NPC_3[i].NPC_3.Surface->w) {
                NPC_3[i].motion = -1;
            }
            if (NPC_3[i].NPC_3.Rect.y < Height / 3) {
                int s = rand() % 1000;
                if (s < 4) {
                    NPCShoot(NPC_3[i].NPC_3.Rect.x + NPC_3[i].NPC_3.Surface->w / 2, NPC_3[i].NPC_3.Rect.y + NPC_3[i].NPC_3.Surface->h);
                }
            }
            if (NPC_3[i].NPC_3.Rect.x + NPC_3[i].NPC_3.Surface->w * 9 / 10 >= my_ship.ship.Rect.x + my_ship.ship.Surface->w / 6 &&
                NPC_3[i].NPC_3.Rect.x + NPC_3[i].NPC_3.Surface->w / 10 <= my_ship.ship.Rect.x + my_ship.ship.Surface->w * 5 / 6 &&
                NPC_3[i].NPC_3.Rect.y + NPC_3[i].NPC_3.Surface->h >= my_ship.ship.Rect.y + 10 &&
                NPC_3[i].NPC_3.Rect.y <= my_ship.ship.Rect.y + my_ship.ship.Surface->h) {
                Mix_PlayChannelTimed(-1, NPC_2_dead, 0, -1);
                NPC_3[i].hp = 0;
                my_ship.hp -= 3;
                if (my_ship.attack > 1) {
                    my_ship.attack--;
                }
                if (my_ship.hp <= 0) {
                    Mix_PlayChannelTimed(-1, lose, 0, -1);
                    my_ship.hp = 0;
                    if_end = 1;
                }
            }
        }
    }
    if (r[3] && Boss.hp) {
        if (Boss.Boss.Rect.y < 0) {
            Boss.Boss.Rect.y += BOSS_SPEED;
        }
        int s = rand() % 1000;
        if (s < 5) {
            NPCShoot(Boss.Boss.Rect.x + Boss.Boss.Surface->w / 5, Boss.Boss.Surface->h / 2);
            NPCShoot(Boss.Boss.Rect.x + Boss.Boss.Surface->w * 4 / 5 , Boss.Boss.Surface->h / 2);
        } else if (s < 8) {
            if (Boss.Xray == 0) {
                Boss.Xray = 100;
                for (int i = 0; i < XRAY_NUM; i++) {
                    if (Xray[i].if_live == 0) {
                        Xray[i].if_live = 1;
                        Xray[i].Xray.Rect.x = Boss.Boss.Rect.x + rand() % Boss.Boss.Surface->w;
                        Xray[i].Xray.Rect.y = Boss.Boss.Rect.y + Boss.Boss.Surface->h / 2;
                        Boss.x = Xray[i].Xray.Rect.x;
                        break;
                    }
                }
            }
        }
        if (Boss.Xray) {
            for (int i = 0; i < XRAY_NUM; i++) {
                if (Xray[i].if_live == 0) {
                    Xray[i].if_live = 1;
                    Xray[i].Xray.Rect.x = Boss.x;
                    Xray[i].Xray.Rect.y = Boss.Boss.Rect.y + Boss.Boss.Surface->h / 2;
                    break;
                }
            }
            Boss.Xray--;
        }
    }
//NPC子弹移动
    for (int i = 0; i < NPC_BULLET_NUM; i++) {
        if (NPC_bullet[i].if_live) {
            switch (NPC_bullet[i].motion) {
                case -1:
                    NPC_bullet[i].bullet.Rect.x -= (NPC_BULLET_SPEED - 2);
                    NPC_bullet[i].bullet.Rect.y += NPC_BULLET_SPEED;
                    break;
                case 0:
                    NPC_bullet[i].bullet.Rect.y += NPC_BULLET_SPEED;
                    break;
                case 1:
                    NPC_bullet[i].bullet.Rect.x += (NPC_BULLET_SPEED - 2);
                    NPC_bullet[i].bullet.Rect.y += NPC_BULLET_SPEED;
                    break;
                default:
                    break;
            }
            if (NPC_bullet[i].bullet.Rect.y >= Height) {
                NPC_bullet[i].if_live = 0;
            }
            if (NPC_bullet[i].bullet.Rect.x < 0) {
                NPC_bullet[i].motion = 1;
            }
            if (NPC_bullet[i].bullet.Rect.x > Width - NPC_bullet[i].bullet.Surface->w) {
                NPC_bullet[i].motion = -1;
            }
            if (NPC_bullet[i].bullet.Rect.x + NPC_bullet[i].bullet.Surface->w * 3 / 4 >= my_ship.ship.Rect.x + my_ship.ship.Surface->w / 6 &&
                NPC_bullet[i].bullet.Rect.x + NPC_bullet[i].bullet.Surface->w / 4 <= my_ship.ship.Rect.x + my_ship.ship.Surface->w * 5 / 6 &&
                NPC_bullet[i].bullet.Rect.y + NPC_bullet[i].bullet.Surface->h >= my_ship.ship.Rect.y + my_ship.ship.Surface->h / 4 &&
                NPC_bullet[i].bullet.Rect.y + NPC_bullet[i].bullet.Surface->h / 4 <= my_ship.ship.Rect.y + my_ship.ship.Surface->h) {
                NPC_bullet[i].if_live = 0;
                my_ship.hp--;
                if (my_ship.attack > 1) {
                    my_ship.attack--;
                }
                if (my_ship.hp <= 0) {
                    Mix_PlayChannelTimed(-1, lose, 0, -1);
                    my_ship.hp = 0;
                    if_end = 1;
                }
            }
        }
    }
//Xray的击中
    for (int i = 0; i < XRAY_NUM; i++) {
        if (Xray[i].if_live) {
            if (Xray[i].Xray.Rect.x + Xray[i].Xray.Surface->w / 2 >= my_ship.ship.Rect.x + my_ship.ship.Surface->w / 8 &&
                Xray[i].Xray.Rect.x + Xray[i].Xray.Surface->w / 2 <= my_ship.ship.Rect.x + my_ship.ship.Surface->w * 7 / 8) {
                if (my_ship.delay == 0) {
                    my_ship.hp--;
                    if (my_ship.attack > 1) {
                        my_ship.attack--;
                    }
                    my_ship.delay = 30;
                    if (my_ship.hp <= 0) {
                        Mix_PlayChannelTimed(-1, lose, 0, -1);
                        my_ship.hp = 0;
                        if_end = 1;
                    }
                }
            }
        }
    }
//得到物品
    for (int i = 0; i < 3; i++) {
        if (Item[i].if_live) {
            Item[i].item.Rect.y += ITEM_SPEED;
            if (Item[i].item.Rect.y >= Height) {
                Item[i].if_live = 0;
            }
            if (Item[i].item.Rect.x + Item[i].item.Surface->w >= my_ship.ship.Rect.x &&
                Item[i].item.Rect.x <= my_ship.ship.Rect.x + my_ship.ship.Surface->w &&
                Item[i].item.Rect.y + Item[i].item.Surface->h >= my_ship.ship.Rect.y &&
                Item[i].item.Rect.y <= my_ship.ship.Rect.y + my_ship.ship.Surface->h) {
                Item[i].if_live = 0;
                switch (i) {
                    case 0:
                        if (my_ship.hp < 15) {
                            Mix_PlayChannelTimed(-1, get_hp, 0, -1);
                            my_ship.hp++;
                        }
                        break;
                    case 1:
                        if (my_ship.attack < 3) {
                            Mix_PlayChannelTimed(-1, get_bullet, 0, -1);
                            my_ship.attack++;
                        }
                        break;
                    case 2:
                        Mix_PlayChannelTimed(-1, get_bomb, 0, -1);
                        Bomb.Bomb_num++;
                    default:
                        break;
                }
            }
        }
    }
}
//暂停界面
void Pause(SDL_Event *MainEvent) {
    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, BackGround.Texture, NULL, &BackGround.Rect);
    char score_str[15];
    sprintf(score_str, "Score: %d", score);
    Score_TTF.Surface = TTF_RenderUTF8_Blended(Score_TTF.Font, score_str, Score_TTF.Color);
    Score_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Score_TTF.Surface);
    Score_TTF.Rect.w = Score_TTF.Surface->w;
    Score_TTF.Rect.h = Score_TTF.Surface->h;
    SDL_RenderCopy(Renderer, Score_TTF.Texture, NULL, &Score_TTF.Rect);
    SDL_FreeSurface(Score_TTF.Surface);
    SDL_DestroyTexture(Score_TTF.Texture);
    SDL_RenderCopy(Renderer, Pause_TTF.Texture, NULL, &Pause_TTF.Rect);
    SDL_RenderCopy(Renderer, Tips[1].Texture, NULL, &Tips[1].Rect);
    SDL_RenderPresent(Renderer);
    while (SDL_WaitEvent(MainEvent)) {
        if (MainEvent->type == SDL_KEYDOWN) {
            if (MainEvent->key.keysym.sym == SDLK_RETURN) {
                if_pause = 0;
                break;
            } else if (MainEvent->key.keysym.sym == SDLK_q) {
                if_end = 1;
                break;
            }
        }
    }
}
//结束界面
void End(SDL_Event *MainEvent) {
    SDL_RenderClear(Renderer);
    SDL_RenderCopy(Renderer, BackGround.Texture, NULL, &BackGround.Rect);
    char score_str[15];
    sprintf(score_str,  "Score: %d", score);
    Score_TTF.Surface = TTF_RenderUTF8_Blended(Score_TTF.Font, score_str, Score_TTF.Color);
    Score_TTF.Texture = SDL_CreateTextureFromSurface(Renderer, Score_TTF.Surface);
    Score_TTF.Rect.w = Score_TTF.Surface->w;
    Score_TTF.Rect.h = Score_TTF.Surface->h;
    SDL_RenderCopy(Renderer, Score_TTF.Texture, NULL, &Score_TTF.Rect);
    SDL_FreeSurface(Score_TTF.Surface);
    SDL_DestroyTexture(Score_TTF.Texture);
    SDL_RenderCopy(Renderer, End_TTF.Texture, NULL, &End_TTF.Rect);
    SDL_RenderCopy(Renderer, Tips[2].Texture, NULL, &Tips[2].Rect);
    SDL_RenderPresent(Renderer);
    while (SDL_WaitEvent(MainEvent)) {
        if (MainEvent->type == SDL_KEYDOWN) {
            if (MainEvent->key.keysym.sym == SDLK_RETURN) {
                if_end = 0;
                break;
            } else if (MainEvent->key.keysym.sym == SDLK_q) {
                if_quit = 1;
                break;
            }
        }
    }
}
//free内存
void Free() {
    SDL_FreeSurface(my_ship.ship.Surface);
    SDL_DestroyTexture(my_ship.ship.Texture);
    SDL_FreeSurface(my_ship.life.Surface);
    SDL_DestroyTexture(my_ship.life.Texture);

    TTF_CloseFont(my_ship.life_num_TTF.Font);

    for (int i = 0; i < 3; i++) {
        SDL_FreeSurface(Item[i].item.Surface);
        SDL_DestroyTexture(Item[i].item.Texture);
    }

    for (int i = 0; i < MY_BULLET_NUM; i++) {
        SDL_FreeSurface(my_bullet[i].bullet.Surface);
        SDL_DestroyTexture(my_bullet[i].bullet.Texture);
    }

    SDL_FreeSurface(Bomb.Bomb.Surface);
    SDL_DestroyTexture(Bomb.Bomb.Texture);

    TTF_CloseFont(Bomb.Bomb_num_TTF.Font);

    for (int i = 0; i < NPC_BULLET_NUM; i++) {
        SDL_FreeSurface(NPC_bullet[i].bullet.Surface);
        SDL_DestroyTexture(NPC_bullet[i].bullet.Texture);
    }

    for (int i = 0; i < XRAY_NUM; i++) {
        SDL_FreeSurface(Xray[i].Xray.Surface);
        SDL_DestroyTexture(Xray[i].Xray.Texture);
    }

    for (int i = 0; i < NPC_1_NUM; i++) {
        SDL_FreeSurface(NPC_1[i].NPC_1.Surface);
        SDL_DestroyTexture(NPC_1[i].NPC_1.Texture);
    }
    for (int i = 0; i < NPC_2_NUM; i++) {
        SDL_FreeSurface(NPC_2[i].NPC_2.Surface);
        SDL_DestroyTexture(NPC_2[i].NPC_2.Texture);}

    for (int i = 0; i < NPC_3_NUM; i++) {
        SDL_FreeSurface(NPC_3[i].NPC_3.Surface);
        SDL_DestroyTexture(NPC_3[i].NPC_3.Texture);
    }

    SDL_FreeSurface(Boss.Boss.Surface);
    SDL_DestroyTexture(Boss.Boss.Texture);

    SDL_FreeSurface(BackGround.Surface);
    SDL_DestroyTexture(BackGround.Texture);

    SDL_FreeSurface(Boss_hp.Surface);
    SDL_DestroyTexture(Boss_hp.Texture);

    TTF_CloseFont(Start_TTF.Font);
    SDL_FreeSurface(Start_TTF.Surface);
    SDL_DestroyTexture(Start_TTF.Texture);

    TTF_CloseFont(Score_TTF.Font);

    TTF_CloseFont(Pause_TTF.Font);
    SDL_FreeSurface(Pause_TTF.Surface);
    SDL_DestroyTexture(Pause_TTF.Texture);

    TTF_CloseFont(End_TTF.Font);
    SDL_FreeSurface(End_TTF.Surface);
    SDL_DestroyTexture(End_TTF.Texture);

    for (int i = 0; i < 4; i++) {
        TTF_CloseFont(Tips[i].Font);
        TTF_CloseFont(Round[i].Font);
        SDL_FreeSurface(Tips[i].Surface);
        SDL_FreeSurface(Round[i].Surface);
        SDL_DestroyTexture(Tips[i].Texture);
        SDL_DestroyTexture(Round[i].Texture);
    }

    Mix_FreeChunk(bgm);
    Mix_FreeChunk(bomb_music);
    Mix_FreeChunk(boss_music);
    Mix_FreeChunk(get_bomb);
    Mix_FreeChunk(get_bullet);
    Mix_FreeChunk(get_hp);
    Mix_FreeChunk(lose);
    Mix_FreeChunk(win);
    Mix_FreeChunk(shoot_music);
    Mix_FreeChunk(NPC_1_dead);
    Mix_FreeChunk(NPC_2_dead);
    Mix_CloseAudio();
}

void Quit() {
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
}