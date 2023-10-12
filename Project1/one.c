#include <stdio.h>
#include <windows.h> //清屏函数
#include <conio.h> //_kbhit()函数与_getch()函数
#include <stdlib.h>
#include <time.h>

#define MAX_x 10 //行
#define MAX_y 40 //列
//定义地图大小

void Init(int *, int *, int [][MAX_y]);//初始化
int GetIn(int);//获取键盘输入
int Move(int, int *, int [][MAX_y]);//移动方向
void PrintMap(int [][MAX_y]);//打印画面

int main() {
    int map[MAX_x][MAX_y] = {0};
    int command = 0;
    int len = 0;
    Init(&command, &len, map);
    while (1) {
        command = GetIn(command);
        if (Move(command, &len, map)) {
            PrintMap(map);
        } else {
            break;
        }
        Sleep(1000);
    }
    return 0;
}

//函数区：
void Init(int *command, int *len, int map[][MAX_y]) {
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
    map[4][4] = -1;
}

int GetIn(int command) {
    int temp = -1;
    if(_kbhit()) {
        switch (_getch()) {
            case 'a' : temp = 1; break;//向左
            case 'w' : temp = 0; break;//向上
            case 'd' : temp = 3; break;//向右
            case 's' : temp = 2; break;//向下
            default : break;
        }
        if (temp != -1 && abs(command - temp) != 2) { //反向会死
            command = temp;
        }
    }
    return command;
}

int Move(int command, int *len, int map[][MAX_y]) {
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
    if (map[head_x][head_y] == -1) {//
        map[head_x][head_y] = 1;
        (*len)++;//
        //
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
        return 0;
    } else {
        map[head_x][head_y] = 1;
    }
    return 1;
}

void PrintMap(int map[][MAX_y]) {
    system("cls");//

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



