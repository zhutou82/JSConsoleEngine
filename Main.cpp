#define UNICODE
#include "PCH.h"
#include <chrono>
#include <thread>
#include <any>
#include <fstream>
#include <cstdlib>
#include "MyProfiler.h"
#include "MyBinalizer.h"
#include <Windows.h>
#include <memory>
#include <thread>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <time.h>       /* time */

constexpr int SCREEN_WIDTH = 80;
constexpr int SCREEN_HEIGHT = 30;
constexpr int SCREEN_PIXEL = SCREEN_WIDTH * SCREEN_HEIGHT;

constexpr int FIELDWIDTH = 79;
constexpr int FIELDHEIGHT = 29;
constexpr int FIELD_PIXEL = FIELDWIDTH * FIELDHEIGHT;

constexpr int FIELF_START_OFFSET_X = 0;
constexpr int FIELF_START_OFFSET_Y = 0;

int SNACK_START_POS_X = 30;
int SNACK_START_POS_Y = 15;


wchar_t* screen = nullptr;
int g_dt = 16;

bool IsCollidedWithTile(int coordX, int coordY, wchar_t tile)
{
    if (screen[coordY * SCREEN_WIDTH + coordX] == tile)
        return true;
    //for (int i = 0; i < SCREEN_HEIGHT ; ++i)
    //{
    //    for (int j = 0; j < SCREEN_WIDTH; ++j)
    //    {
    //        
    //        screen[i * SCREEN_WIDTH + j];
    //    }
    //}
    return false;
}

int main() 
{
    screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) screen[i] = L' ';

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwZBytesWritten = 0;

    //Field info
    unsigned char * filed = new unsigned char[FIELD_PIXEL];
    for (int i = 0; i < FIELDHEIGHT; i++) // Board Boundary
        for (int j = 0; j < FIELDWIDTH; j++)
            filed[i * FIELDWIDTH + j] = (j == 0 || j == FIELDWIDTH - 1 || i == 0 || i == FIELDHEIGHT - 1) ? 9 : 0;

    int snackStartLength = 5;
    std::vector<unsigned char> snack(snackStartLength, 8);
    bool gameOver = false;

    //snack info
    int snackCurPosX = SNACK_START_POS_X;
    int snackCurPosY = SNACK_START_POS_Y;
    
    int snackMoveSpeedX = 1;
    int snackMoveSppedY = 1;
    

    //random food position
    srand(time(NULL));
    int foodPosX = 40;  rand() % (FIELDWIDTH - 1 - FIELF_START_OFFSET_X) + 1;
    int foodPosY = 15;  rand() % (FIELDHEIGHT - 1 - FIELF_START_OFFSET_Y) + 1;
    
    //color info
    WORD snackColor = FOREGROUND_GREEN;
    WORD boundaryColor = FOREGROUND_RED;
    WORD foodColor = FOREGROUND_BLUE;

    //tile info
    wchar_t snackTile = L'*';
    wchar_t fieldTile = L'#';
    wchar_t foodTile = L'F';
    wchar_t emptyTile = L' ';

    while (!gameOver)
    {
        //Game Time
        std::this_thread::sleep_for(std::chrono::milliseconds(g_dt));
        
        // Draw Field
        for (int i = 0; i < FIELDHEIGHT; i++)
            for (int j = 0; j < FIELDWIDTH; j++)
            {
                screen[(i + FIELF_START_OFFSET_Y) * SCREEN_WIDTH + (j + FIELF_START_OFFSET_X)] = L" ABCDEFG*#"[filed[i * FIELDWIDTH + j]];
                if (filed[i * FIELDWIDTH + j] == 9)
                {
                    WriteConsoleOutputAttribute(hConsole, &boundaryColor, 1, COORD{ (SHORT)(j + FIELF_START_OFFSET_X), (SHORT)(i + FIELF_START_OFFSET_Y) }, &dwZBytesWritten);
                }
            }
        //Draw food
        screen[foodPosY * SCREEN_WIDTH + foodPosX] = foodTile;
        if (IsCollidedWithTile(snackCurPosX, snackCurPosY, fieldTile))
        {
            gameOver = true;
        }
        else if (IsCollidedWithTile(snackCurPosX, snackCurPosY, foodTile))
        {
            //draw empty to the food location
            screen[foodPosY * SCREEN_WIDTH + foodPosX] = emptyTile;
            //create genrate a new foold location 
            foodPosX = rand() % (FIELDWIDTH - 1 - FIELF_START_OFFSET_X) + 1;
            foodPosY = rand() % (FIELDHEIGHT - 1 - FIELF_START_OFFSET_Y) + 1;
            //increase the snack length
            //++SNACK_START_LENGTH;
            ++snackStartLength;
        }
        
        for (int i = 0; i < snackStartLength; ++i)
        {
            WriteConsoleOutputAttribute(hConsole, &snackColor, 1, COORD{ (SHORT)(snackCurPosX - i), (SHORT)snackCurPosY }, &dwZBytesWritten);
            screen[snackCurPosY * SCREEN_WIDTH + snackCurPosX - i] = snackTile;
        }
        snackCurPosX += snackMoveSpeedX;
        //Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_PIXEL, { 0, 0 }, &dwZBytesWritten);
    }




    delete screen;
    CloseHandle(hConsole);
}
