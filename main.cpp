#include "raylib.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cstdlib> 
#include <string>
#include <cmath>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif



// Set up random locations for the dumb buildings

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
constexpr int SCREEN_WIDTH = 2400;
constexpr int SCREEN_HEIGHT = 1350;
constexpr float GRAVITY = 9.81f;
constexpr int BULLET_INITIAL_SPEED = 823;
constexpr float SCALE_FACTOR = 10.0;


static bool gameOver = false;
static bool pause = false;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float time;
} Bullet;



typedef struct AimLine {
    Vector2 origin;
    Vector2 notTheOrigin;
} AimLine;

Vector2 buildingOnePos = { (float)50, (float)394 };
Vector2 buildingTwoPos = { (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
Vector2 playerPos = { (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
AimLine aimLine = {};
Rectangle targetPos = {buildingTwoPos.x + 90, 500 - buildingTwoPos.y, 10, 10};
double magic = 0;
bool isFired = false;
bool hitXTheXTarget = false;
static double gunAngle = {};

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void UpdateBullet(void);
static void DrawAimLine(void);
static void DrawGame(void);  
static void DrawBullet(Bullet* bullet);    // Draw game (one frame)
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
static double DegToRad(double x);
static void BulletMovement(void);
static double GetChangeOfX(double origin, double radius, double change);
static double GetChangeOfY(double origin, double radius, double change);
Bullet CreateBullet(Vector2 startPos, float angleInDegrees);

Bullet bullet = CreateBullet({300, 300}, 30);
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "if you see this, gimme a job");

    InitGame();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        UpdateDrawFrame();
    }

    CloseWindow();

    return 0;
}

void InitGame(void)
{
    std::cout << "In init" << '\n';
    int lbx = 1300, ubx = 1900, lby = -100, uby = 100; 
    buildingTwoPos.x = (rand() % (ubx - lbx + 1)) + lbx;
    buildingTwoPos.y = (rand() % (uby - lby + 1)) + lby;
    std::cout << buildingTwoPos.x << '\n';
    targetPos.x = buildingTwoPos.x + 90;
    targetPos.y = 574 + buildingTwoPos.y;
    targetPos.height = 20;
    targetPos.width = 5;
    aimLine.origin = { buildingOnePos.x + 100, buildingOnePos.y - 50 };
    aimLine.notTheOrigin = { buildingOnePos.x + 400, buildingOnePos.y };
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}



Bullet CreateBullet(Vector2 startPos, float angleInDegrees) {
    Bullet* bullet = { 0 };
    bullet->position = startPos;
    bullet->time = 0.0f;
    
    // Convert angle to radians
    float angleRad = angleInDegrees * DEG2RAD;
    
    // Initialize velocity components (scaled down for screen coordinates)
    bullet->velocity.x = (BULLET_INITIAL_SPEED * cosf(angleRad)) / SCALE_FACTOR;
    bullet->velocity.y = -(BULLET_INITIAL_SPEED * sinf(angleRad)) / SCALE_FACTOR; // Negative because y-axis is inverted
    
    return *bullet;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            if(isFired)
            {
                bullet = CreateBullet({100, 300}, 45.0f);

            } else
            {
                DrawAimLine();
            }

        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

double DegToRad(double x)
{
    return ((x * 3.14)/180);
}

double GetChangeOfX(double origin, double radius, double change)
{
    return origin + (radius * cos(DegToRad(change)));
}

double GetChangeOfY(double origin, double radius, double change)
{
    return origin + (radius * sin(DegToRad(change)));
}

void DrawAimLine(void)
{

    double radius = 300;
    if (IsKeyDown(KEY_UP)) 
    {
        aimLine.notTheOrigin.y = GetChangeOfY(aimLine.origin.y, radius, -.5+magic);
        if(aimLine.notTheOrigin.y - aimLine.origin.y > 0)
        {
            aimLine.notTheOrigin.x = GetChangeOfX(aimLine.origin.x, radius, -.5+magic);
            //std::cout <<  GetChangeOfX(aimLine.origin.x, radius, -2) << '\n';
        } else if(aimLine.notTheOrigin.y - aimLine.origin.y < 0)
        {
            aimLine.notTheOrigin.x = GetChangeOfX(aimLine.origin.x, radius, -.5+magic);
            //std::cout <<  GetChangeOfX(aimLine.origin.x, radius, -2) << '\n';
        }
        magic = magic - .5;
    }
    if (IsKeyDown(KEY_DOWN)) 
    {
        aimLine.notTheOrigin.y = GetChangeOfY(aimLine.origin.y, radius, .5+magic);
        if(aimLine.notTheOrigin.y - aimLine.origin.y > 0)
        {
            aimLine.notTheOrigin.x = GetChangeOfX(aimLine.origin.x, radius, .5+magic);
        } else if(aimLine.notTheOrigin.y - aimLine.origin.y < 0)
        {
            aimLine.notTheOrigin.x = GetChangeOfX(aimLine.origin.x, radius, .5+magic);
        }
        magic = magic + .5;
    }
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gunAngle = atan(((aimLine.origin.y - aimLine.notTheOrigin.y)/(aimLine.notTheOrigin.x - aimLine.origin.x)));
        gunAngle = 3.14 - gunAngle;
        std::cout << aimLine.origin.y << '\n';
        std::cout << aimLine.notTheOrigin.y << '\n';
        std::cout << aimLine.notTheOrigin.x << '\n';
        std::cout << aimLine.origin.x << '\n';
        std::cout << "gun angle is " << gunAngle << '\n';
        isFired = true;
        std::cout << "PLEAE";
    }
}

void DrawBullet(Bullet* bullet) {
    DrawCircleV(bullet->position, 2.0f, RED);
}

// Draw game (one frame)
void DrawGame()
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
        DrawRectangle(0, SCREEN_HEIGHT - 256, SCREEN_WIDTH, 256, GREEN);


        // static const int screenWidth = 2400;
        // static const int screenHeight = 1350;
        std::string text = std::to_string(aimLine.notTheOrigin.y);
        
        // Convert the std::string to a const char* for DrawText
        // DrawText(text.c_str(), 200, 200, 20, BLACK);
        DrawRectangle(buildingOnePos.x, buildingOnePos.y, 200, 700, GRAY);
        DrawText(text.c_str(), 70, 414, 20, BLACK); 

        text = std::to_string(buildingTwoPos.x) + " " + std::to_string(buildingTwoPos.y);
        //so the y is the top of the building so when we do our random number and add or subtract,
        //we'll have to do the same thing to the building size
        DrawRectangle(buildingTwoPos.x, 594 + buildingTwoPos.y, 200, 500 - buildingTwoPos.y, YELLOW);
        DrawText(text.c_str(), 1620, 614, 30, BLACK);    
        //DrawLine(bullet.endOfTrail.x, bullet.endOfTrail.y, bullet.position.x, bullet.position.y, BLACK);
        DrawLine(aimLine.origin.x, aimLine.origin.y, aimLine.notTheOrigin.x, aimLine.notTheOrigin.y, BLACK);
        if(isFired)
            DrawCircleV(bullet.position, 2.0f, RED);

        DrawRectangleRec(targetPos, PURPLE);

        if (!gameOver)
        {
            
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}



          