#include <stdio.h>
#include <windows.h> //清屏函数
#include <conio.h> //_kbhit()函数与_getch()函数
#include <stdlib.h>//生成随机数
#include <time.h>//time()函数

int MAX_x = 20; //行
int MAX_y = 60; //列

void Start();//打印初始界面
void BeforePlay(int *, int *, int *);//用户输入基本数据
void Init(int *, int *, int [][MAX_y], int *, int *, int *);//初始化
void GetIn(int *, int *, int *);//获取键盘输入
void Move(int, int *, int *, int [][MAX_y]);//判断移动方向
void PrintMap(int, int [][MAX_y]);//打印画面
void IfPause(int *, int *);//判断是否暂停
void IfEnd(int *, int);//判断是否结束本局
int Speedup(int, int);//加速

int main() {
    int speed = 0;//蛇的初始速度
    int len = 0;
    int map[60][60] = {0};
    int command = 0;
    int if_pause = 0;
    int if_end = 0;
    int if_quit = 0;
    while(1) {
        Start();

        BeforePlay(&MAX_x, &MAX_y, &speed);

        Init(&command, &len, map, &if_end, &if_pause, &if_quit);
        while (1) {
            GetIn(&command, &if_pause, &if_end);
            if (if_pause) {
                IfPause(&if_pause, &if_end);
            }
            Move(command, &len, &if_end, map);
            if (if_end) {
                IfEnd(&if_quit, len);
                break;
            }
            PrintMap(len, map);
            int time = Speedup(speed, len);
            Sleep(time);
        }
        if(if_quit) {
            break;
        }
    }
    return 0;
}

//函数区：
void Start() {
    system("cls");
    printf("Snake Game\n");
    printf("'s' to start the game. Have fun!\n");
    //轮询直至输入s
    while (1) {
        if (kbhit() && getch() == 's') {
            break;
        }
    }
}

void BeforePlay(int *x, int *y, int *speed) {
    printf("Please enter the width of game space(10 <= & <= 60 , integer): ");
    scanf("%d", y);
    printf("\n");
    printf("Please enter the height of game space(10 <= & <= 20, integer): ");
    scanf("%d", x);
    printf("\n");
    printf("Please enter the speed of snake moving(100 <= & <= 1000, 1000 is the slowest, integer): ");
    scanf("%d", speed);
    printf("\n");
}

void Init(int *command, int *len, int map[][MAX_y], int *if_end, int *if_pause, int *if_quit) {
    //初始化地图
    for (int i = 0; i < MAX_x; i++) {
        for (int j = 0; j < MAX_y; j++) {
            map[i][j] = 0;
        }
    }
    //初始化方向
    *command = 2;
    //初始化蛇长
    *len = 3;
    //初始化蛇的位置
    map[2][1] = 1;
    map[1][1] = 2;
    map[0][1] = 3;
    //初始化食物的位置
    map[3][3] = -1;
    //将三个判断位为置为0
    *if_pause = 0;
    *if_end = 0;
    *if_quit = 0;
}

void GetIn(int *command, int *if_pause, int *if_end) {
    int temp = -1;
    if(_kbhit()) {
        switch (_getch()) {
            case 'a' : temp = 1; break;//向左
            case 'w' : temp = 0; break;//向上
            case 'd' : temp = 3; break;//向右
            case 's' : temp = 2; break;//向下
            case 'p' : *if_pause = 1; break;//暂停
            case 'q' : *if_end = 1; break;//结束本局
            default : break;
        }
        if (temp != -1 && abs(*command - temp) != 2) {//蛇不能反向
            *command = temp;
        }
    }
}

void Move(int command, int *len, int *if_end, int map[][MAX_y]) {
    int head_x = 0;
    int head_y = 0;
    for (int i = 0; i < MAX_x; i++) {
        for (int j = 0; j < MAX_y; j++) {
            if (map[i][j] == *len) {//蛇尾
                map[i][j] = 0;
            } else if (map[i][j] > 1) {//蛇身
                map[i][j]++;
            } else if (map[i][j] == 1) {//蛇头
                map[i][j]++;
                //确定蛇头的新位置
                switch(command) {
                    case 0 : head_x = i - 1; head_y = j; break;
                    case 1 : head_x = i; head_y = j - 1; break;
                    case 2 : head_x = i + 1; head_y = j; break;
                    case 3 : head_x = i; head_y = j + 1; break;
                    default : break;
                }
            }
        }
    }

    if (map[head_x][head_y] == -1) {//蛇吃到食物
        map[head_x][head_y] = 1;
        (*len)++;//蛇长加一
        srand(time(NULL));
        while(1) {
            int x = rand() % MAX_x;
            int y = rand() % MAX_y;
            if (map[x][y] == 0) {
                map[x][y] = -1;
                break;
            }
        }
    } else if (map[head_x][head_y] > 0 || head_x < 0 || head_x >= MAX_x ||
               head_y < 0 || head_y >= MAX_y) {
        *if_end = 1;//碰到边界或吃到自身，游戏结束
    } else {
        map[head_x][head_y] = 1;
    }
}


void PrintMap(int len, int map[][MAX_y]) {
    system("cls");
    //打印提示
    printf("Snake Game\n");
    printf("Current score: %d\n", len - 3);
    printf("Tips: 'a' is left, 'd' is right, 'w' is up, 's' is down\n"
           "      'p' to pause the game or 'q' to end the game\n");
    //打印上边框
    for (int i = 0; i <= MAX_y + 1; i++) {
        if (i == 0) {
            printf("/");
        } else if (i == MAX_y + 1) {
            printf("\\");
        } else {
            printf("-");
        }
    }
    printf("\n");

    //打印左右边框、蛇、食物、和地图
    for (int i = 0; i < MAX_x; i++) {
        for (int j = 0; j <= MAX_y + 1; j++) {
            if (j == 0 || j == MAX_y + 1) {
                printf("|");
            } else if (map[i][j - 1] > 1) {
                printf("*");
            } else if (map[i][j - 1] == 1) {
                printf("&");
            } else if (map[i][j - 1] == -1) {
                printf("$");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    //打印下边框
    for (int i = 0; i <= MAX_y + 1; i++) {
        if (i == 0) {
            printf("\\");
        } else if (i == MAX_y + 1) {
            printf("/");
        } else {
            printf("-");
        }
    }
    printf("\n");
}


void IfPause(int *if_pause, int *if_end) {
    printf("You pause the game\n");
    printf("Tips: 'c' to continue the game or 'q' to end the game");
    while (1) {
        if(_kbhit()) {
            if (_getch() == 'c') {
                *if_pause = 0;
                break;
            } else if (_getch() == 'q') {
                *if_end = 1;
                break;
            }
        }
    }
}

void IfEnd(int *if_quit, int len) {
    system("cls");
    printf("Snake Game\n");
    printf("Game Over!\n");
    printf("Your final score is %d\n", len - 3);
    printf("Tips: 'q' to quit the game or 'r' to restart the game");
    while (1) {
        if (_kbhit()) {
            if (_getch() == 'q') {
                *if_quit = 1;//退出游戏
                break;
            } else if (_getch() == 'r') {
                break;//重开
            }
        }
    }
}

int Speedup(int speed, int len) {
    if(speed - (len - 3) * 50 > 100) {
        return speed - (len - 3) * 50;
    } else {
        return speed;
    }
}

