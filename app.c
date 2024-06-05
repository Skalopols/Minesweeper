//#include <GL/glew.h>
#define _CRT_SECURE_NO_WARNINGS
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <locale.h>

#define MENU_SIZE 75
//hello
int mapW = -1;
int mapH = -1;
int mines_count = -1;

typedef struct {
    short mine;
    short flag;
    short open;
    int cntAround;
} TCell;

TCell** map;

// Функция для выделения памяти под карту
void initializeMap(int mapW, int mapH) {
    map = (TCell**)malloc(mapW * sizeof(TCell*));
    for (int i = 0; i < mapW; i++) {
        map[i] = (TCell*)malloc(mapH * sizeof(TCell));
    }
}

// Функция для освобождения памяти, выделенной под карту
void freeMap(int mapW) {
    for (int i = 0; i < mapW; i++) {
        free(map[i]);
    }
    free(map);
}

int mines;
int closedCell;
short fail;

short IsCellInMap(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < mapW) && (y < mapH);
}

int count_flag(int x, int y) {
    int flag = 0;
    for (int dx = -1; dx < 2; dx++) {
        for (int dy = -1; dy < 2; dy++) {
            if (IsCellInMap(x + dx, y + dy)) {
                if (map[x + dx][y + dy].flag == 1) {
                    flag++;
                }
            }
        }
    }
    return flag;
}

int count_open(int x, int y) {
    int open = 0;
    for (int dx = -1; dx < 2; dx++) {
        for (int dy = -1; dy < 2; dy++) {
            if (IsCellInMap(x + dx, y + dy)) {
                if (map[x + dx][y + dy].open == 0) {
                    open++;
                }
            }
        }
    }
    return open;
}

void Game_New() {
    srand(time(NULL));

    // Очистка карты перед размещением мин
    for (int i = 0; i < mapW; i++) {
        for (int j = 0; j < mapH; j++) {
            map[i][j].mine = 0;
            map[i][j].flag = 0;
            map[i][j].open = 0;
            map[i][j].cntAround = 0;
        }
    }

    mines = mines_count;
    closedCell = mapW * mapH;
    fail = 0;

    for (int i = 0; i < mines; i++) {
        int x = rand() % mapW;
        int y = rand() % mapH;
        if (map[x][y].mine) {
            i--; // Если мина уже здесь, повторяем итерацию
        }
        else {
            map[x][y].mine = 1;

            // Обновление счетчиков соседних клеток
            for (int dx = -1; dx < 2; dx++) {
                for (int dy = -1; dy < 2; dy++) {
                    if (IsCellInMap(x + dx, y + dy)) {
                        map[x + dx][y + dy].cntAround += 1;
                    }
                }
            }
        }
    }
}

void Line(float x1, float y1, float x2, float y2) {
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
}

void ShowCount(int a) {
    glLineWidth(3);
    glColor3f(1, 1, 0);
    glBegin(GL_LINES);
    if ((a != 1) && (a != 4)) Line(0.3, 0.85, 0.7, 0.85);
    if ((a != 0) && (a != 1) && (a != 7)) Line(0.3, 0.5, 0.7, 0.5);
    if ((a != 1) && (a != 4) && (a != 7)) Line(0.3, 0.15, 0.7, 0.15);

    if ((a != 5) && (a != 6)) Line(0.7, 0.5, 0.7, 0.85);
    if (a != 2) Line(0.7, 0.5, 0.7, 0.15);

    if ((a != 1) && (a != 2) && (a != 3) && (a != 7)) Line(0.3, 0.5, 0.3, 0.85);
    if ((a == 0) || (a == 2) || (a == 6) || (a == 8)) Line(0.3, 0.5, 0.3, 0.15);


    glEnd();
}

void ShowMine() {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0, 0, 0);
    glVertex2f(0.3, 0.3);
    glVertex2f(0.7, 0.3);
    glVertex2f(0.7, 0.7);
    glVertex2f(0.3, 0.7);
    glEnd();
}

void ShowField() {
    glBegin(GL_TRIANGLE_STRIP);

    glColor3f(0.8, 0.8, 0.8); glVertex2f(0, 1);
    glColor3f(0.7, 0.7, 0.7); glVertex2f(1, 1); glVertex2f(0, 0);
    glColor3f(0.6, 0.6, 0.6); glVertex2f(1, 0);

    glEnd();
}

void ShowFieldOpen() {
    glBegin(GL_TRIANGLE_STRIP);

    glColor3f(0.3, 0.7, 0.3); glVertex2f(0, 1);
    glColor3f(0.3, 0.6, 0.3); glVertex2f(1, 1); glVertex2f(0, 0);
    glColor3f(0.3, 0.5, 0.3); glVertex2f(1, 0);

    glEnd();
}

void ShowFlag() {
    glBegin(GL_TRIANGLES);

    glColor3f(1, 0, 0);
    glVertex2f(0.25, 0.75);
    glVertex2f(0.85, 0.5);
    glVertex2f(0.25, 0.25);

    glEnd();
    glLineWidth(5);
    glBegin(GL_LINES);

    glColor3f(0, 0, 0);
    glVertex2f(0.25, 0.75);
    glVertex2f(0.25, 0.0);

    glEnd();
}

void OpenFields(int x, int y) {
    if (!IsCellInMap(x, y) || map[x][y].open) return;
    map[x][y].open = 1;
    closedCell--;
    if (map[x][y].cntAround == 0) {
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++)
                if (IsCellInMap(x + dx, y + dy)) OpenFields(x + dx, y + dy);
        }
    }
    if (map[x][y].mine) {
        fail = 1;
        for (int i = 0; i < mapW; i++) {
            for (int j = 0; j < mapH; j++) {
                map[i][j].open = 1;
            }
        }
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    printf("%f %f\n", xpos, ypos);
    int x = (int)(xpos / 50);
    int y = mapH - 1 - (int)((ypos - MENU_SIZE) / 50);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && ypos > MENU_SIZE) {
        if (fail == 1) {
            Game_New();
        }
        else if (IsCellInMap(x, y) && !map[x][y].flag) {
            //map[x][mapH - y - 1].open = 1;
            if (map[x][y].cntAround == count_flag(x, y) && map[x][y].open == 1) {
                for (int dx = -1; dx < 2; dx++) {
                    for (int dy = -1; dy < 2; dy++) {
                        if (IsCellInMap(x + dx, y + dy)) {
                            if (map[x + dx][y + dy].flag != 1) {
                                OpenFields(x + dx, y + dy);
                            }
                        }


                    }
                }
            }

            OpenFields(x, y);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && ypos < MENU_SIZE) {
        if ((0.5 * mapW * 50 * 0.3 <= xpos && xpos <= 0.5 * mapW * 50 * 0.7) && (10 <= ypos && ypos <= 55)) {
            printf("Left button\n");
        }
        else if ((0.5 * mapW * 50 * 1.3 <= xpos && xpos <= 0.5 * mapW * 50 * 1.7) && (10 <= ypos && ypos <= 55)) {
            printf("Right button\n");
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (IsCellInMap(x, y) && map[x][y].open == 0)
            map[x][y].flag = !map[x][y].flag;
        /*if (IsCellInMap(x, y) && map[x][y].open == 1) {
            Help(x, y);
        }*/
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        Game_New();

}

void Game_Show() {
    glLoadIdentity();
    glScalef(2.0 / mapW, 2.0 * (mapH * 50) / (mapH * 50 + MENU_SIZE) / mapH, 1);
    glTranslatef(-mapW * 1 / 2.0, -mapH * 1 / (2.0 * (mapH * 50) / (mapH * 50 + MENU_SIZE)), 0);

    for (int j = 0; j < mapH; j++) {
        for (int i = 0; i < mapW; i++) {
            glPushMatrix();
            glTranslatef(i, j, 0);
            if (map[i][j].open) {
                ShowFieldOpen();
                if (map[i][j].mine)
                    ShowMine();
                else if (map[i][j].cntAround > 0)
                    ShowCount(map[i][j].cntAround);
            }
            else {
                ShowField();
                if (map[i][j].flag) ShowFlag();
            }
            glPopMatrix();
        }
    }
}


void Menu_Show() {
    float menu_border = 1.0 - (MENU_SIZE / ((mapH * 50.0 + MENU_SIZE) * 0.5));
    float coeff = ((mapH * 50.0 + MENU_SIZE) * 0.5);
    glColor3f(0.74, 0.74, 0.74);
    if (fail == 1) {
        glColor3f(1, 0, 0);
    }
    if (fail == 2) {
        glColor3f(0, 1, 0);
    }
    glLoadIdentity();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(1.0, 1.0);
    glVertex2f(1.0, (float)menu_border);
    glVertex2f(-1.0, (float)menu_border);
    glVertex2f(-1.0, 1.0);
    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.7, 1.0 - ((MENU_SIZE - 20) / coeff));
    glVertex2f(-0.3, 1.0 - ((MENU_SIZE - 20) / coeff));
    glVertex2f(-0.3, 1.0 - ((MENU_SIZE - (MENU_SIZE - 10)) / coeff));
    glVertex2f(-0.7, 1.0 - ((MENU_SIZE - (MENU_SIZE - 10)) / coeff));
    glEnd();

    glColor3f(0.5, 0.5, 0.3);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.7, 1.0 - ((MENU_SIZE - 20) / coeff));
    glVertex2f(0.3, 1.0 - ((MENU_SIZE - 20) / coeff));
    glVertex2f(0.3, 1.0 - ((MENU_SIZE - (MENU_SIZE - 10)) / coeff));
    glVertex2f(0.7, 1.0 - ((MENU_SIZE - (MENU_SIZE - 10)) / coeff));
    glEnd();
}

int Help(int x, int y) {
    if (!IsCellInMap(x, y)) return 0;
    if (map[x][y].cntAround == count_open(x, y) && map[x][y].open == 1) {
        printf("Help function\n");
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (IsCellInMap(x + dx, y + dy)) {
                    if (map[x + dx][y + dy].open == 0 && map[x + dx][y + dy].flag == 0) {
                        map[x + dx][y + dy].flag = 1;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

HWND bt_1, bt_2, bt_3, bt_4, bt_5, getMinesCount,
minesCountTyped, mapSelectedNumber, statusText;
short allBtCounter = 0;
char stringMinesCount[1000];

LRESULT WINAPI StartProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY) {
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    }
    else if (message == WM_COMMAND) {
        if (lparam == bt_1) {
            printf("Выбранный размер: 5х5\n");
            SetWindowTextA(mapSelectedNumber, "5x5");
            mapW = mapH = 5;
            allBtCounter++;
        }
        if (lparam == bt_2) {
            printf("Выбранный размер: 10х10\n");
            SetWindowTextA(mapSelectedNumber, "10x10");
            mapW = mapH = 10;
            allBtCounter++;
        }
        if (lparam == bt_3) {
            printf("Выбранный размер: 15х15\n");
            SetWindowTextA(mapSelectedNumber, "15x15");
            mapW = mapH = 15;
            allBtCounter++;
        }
        if (lparam == bt_4) {
            printf("Выбранный размер: 20х20\n");
            SetWindowTextA(mapSelectedNumber, "20x20");
            mapW = mapH = 20;
            allBtCounter++;
        }
        if (lparam == getMinesCount)
        {
            GetWindowTextA(lparam, stringMinesCount, 100);
            mines_count = atoi(stringMinesCount);
        }
        if (lparam == bt_5) {
            if (mapW == -1 || mapH == -1) SetWindowTextA(statusText, "Выберите размер карты!");
            else if (mines_count == -1) SetWindowTextA(statusText, "Введите число мин!");
            else if (mines_count == 0) SetWindowTextA(statusText, "Число мин не может равняться нулю!");
            else if (mines_count <= 0) SetWindowTextA(statusText, "Число мин не может равняться или быть меньше нуля!");
            else if (mines_count >= 25 && mapW == 5) SetWindowTextA(statusText, "Число мин не может быть больше или равно числу клеток!");
            else if (mines_count >= 100 && mapW == 10) SetWindowTextA(statusText, "Число мин не может быть больше или равно числу клеток!");
            else if (mines_count >= 225 && mapW == 15) SetWindowTextA(statusText, "Число мин не может быть больше или равно числу клеток!");
            else if (mines_count >= 400 && mapW == 20) SetWindowTextA(statusText, "Число мин не может быть больше или равно числу клеток!");
            else
            {
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
        }
        if (allBtCounter > 0) {
            EnableWindow(bt_1, FALSE);
            EnableWindow(bt_2, FALSE);
            EnableWindow(bt_3, FALSE);
            EnableWindow(bt_4, FALSE);
        }
    }
    else return DefWindowProcA(hwnd, message, wparam, lparam);
}

void SolveMinesweeper() {
    int change;
    do {
        change = 0;
        for (int x = 0; x < mapW; x++) {
            for (int y = 0; y < mapH; y++) {
                if (map[x][y].open && map[x][y].cntAround > 0) {
                    int flagCount = count_flag(x, y);
                    int closedCount = count_open(x, y);
                    if (flagCount == map[x][y].cntAround) {
                        for (int dx = -1; dx < 2; dx++) {
                            for (int dy = -1; dy < 2; dy++) {
                                if (IsCellInMap(x + dx, y + dy) && !map[x + dx][y + dy].open && !map[x + dx][y + dy].flag) {
                                    Sleep(100);
                                    OpenFields(x + dx, y + dy);
                                    change = 1;
                                }
                            }
                        }
                    }
                    else if (flagCount + closedCount == map[x][y].cntAround) {
                        for (int dx = -1; dx < 2; dx++) {
                            for (int dy = -1; dy < 2; dy++) {
                                if (IsCellInMap(x + dx, y + dy) && !map[x + dx][y + dy].open && !map[x + dx][y + dy].flag) {
                                    map[x + dx][y + dy].flag = 1;
                                    mines--;
                                    change = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (change && closedCell > 0);
}

int main(void)
{
    setlocale(LC_ALL, "Russian");

    WNDCLASSA wcl;
    memset(&wcl, 0, sizeof(WNDCLASSA));
    wcl.lpszClassName = "start window";
    wcl.lpfnWndProc = StartProc;
    RegisterClassA(&wcl);

    HWND hwnd, mapChooseText, mapSelectedText, minesCountText;
    hwnd = CreateWindowA("start window", "Miner Start", WS_OVERLAPPEDWINDOW,
        100, 300, 350, 270, NULL, NULL, NULL, NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    bt_1 = CreateWindowA("BUTTON", "5x5", WS_VISIBLE | WS_CHILD,
        10, 40, 50, 50, hwnd, NULL, NULL, NULL);
    bt_2 = CreateWindowA("BUTTON", "10x10", WS_VISIBLE | WS_CHILD,
        65, 40, 50, 50, hwnd, NULL, NULL, NULL);
    bt_3 = CreateWindowA("BUTTON", "15x15", WS_VISIBLE | WS_CHILD,
        120, 40, 50, 50, hwnd, NULL, NULL, NULL);
    bt_4 = CreateWindowA("BUTTON", "20x20", WS_VISIBLE | WS_CHILD,
        175, 40, 50, 50, hwnd, NULL, NULL, NULL);
    bt_5 = CreateWindowA("BUTTON", "Играть", WS_VISIBLE | WS_CHILD,
        270, 160, 55, 40, hwnd, NULL, NULL, NULL);
    getMinesCount = CreateWindowA("edit", " ", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT | ES_NUMBER,
        175, 140, 50, 20, hwnd, NULL, NULL, NULL);
    mapChooseText = CreateWindowA("static", "Выберите размер карты:", WS_VISIBLE | WS_CHILD,
        10, 20, 300, 20, hwnd, NULL, NULL, NULL);
    mapSelectedText = CreateWindowA("static", "Выбранный размер карты:", WS_VISIBLE | WS_CHILD,
        10, 100, 300, 20, hwnd, NULL, NULL, NULL);
    mapSelectedNumber = CreateWindowA("static", " - ", WS_VISIBLE | WS_CHILD,
        200, 100, 300, 20, hwnd, NULL, NULL, NULL);
    minesCountText = CreateWindowA("static", "Введите число мин:", WS_VISIBLE | WS_CHILD,
        10, 140, 150, 20, hwnd, NULL, NULL, NULL);
    statusText = CreateWindowA("static", " ", WS_VISIBLE | WS_CHILD,
        10, 175, 250, 30, hwnd, NULL, NULL, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (allBtCounter == 0) return 0;

    // Выделение памяти под карту
    initializeMap(mapW, mapH);

    GLFWwindow* window;
    if (!glfwInit()) return -1;

    window = glfwCreateWindow(mapW * 50, mapH * 50 + MENU_SIZE, "Minesweeper", NULL, NULL);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    if (!window) {
        glfwTerminate();
        freeMap(mapW);
        return -1;
    }

    glfwMakeContextCurrent(window);
    Game_New();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if (mines == closedCell) {
            fail = 2;
        }
        Game_Show();
        Menu_Show();
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, key_callback);
        //SolveMinesweeper();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //CHECK VICTORY
    glfwTerminate();
    freeMap(mapW);
    return 0;
}
