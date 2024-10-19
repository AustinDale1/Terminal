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




constexpr int SCREEN_WIDTH = 2400;
constexpr int SCREEN_HEIGHT = 1350;
constexpr float GRAVITY = 9.81f;
constexpr int BULLET_INITIAL_SPEED = 823;
constexpr float SCALE_FACTOR = 100.0;
constexpr float TIME_STEP = 1.0f / 60.0f;


static bool gameOver = false;
static bool pause = false;

class Bullet {
    public:          
        Vector2 position;
        Vector2 velocity;
        float time;   
        void FireBullet(Vector2 startPos, float angleRad)
        {
            position = startPos;
            this->time = 0.0f;
            
            this->velocity.x = (BULLET_INITIAL_SPEED * cosf(angleRad)) / SCALE_FACTOR;
            this->velocity.y = -(BULLET_INITIAL_SPEED * sinf(angleRad)) / SCALE_FACTOR;    
        }
    Bullet() { 
    }
};

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
int bullets = 3;
int score = 0;

static void InitGame(void);   
static void UpdateGame(Bullet& bullet);   
static void UpdateBullet(Bullet& bullet, float deltaTime);
static void DrawAimLine(Bullet& bullet);
static void DrawGame(Bullet& bullet);  
static void UpdateDrawFrame(Bullet& bullet); 
static double DegToRad(double x);
static void NextLevel();

int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "if you see this, gimme a job");

    InitGame();

    SetTargetFPS(60);

    Bullet bullet;

    while (!WindowShouldClose())
    {
        UpdateDrawFrame(bullet);
        if(!gameOver && hitXTheXTarget)
        {
            NextLevel();
        }
    }

    CloseWindow();

    return 0;
}

void NextLevel()
{
    hitXTheXTarget = false;
    score = score + bullets;
    int lbx = 1300, ubx = 1900, lby = -100, uby = 100; 
    buildingTwoPos.x = (rand() % (ubx - lbx + 1)) + lbx;
    buildingTwoPos.y = (rand() % (uby - lby + 1)) + lby;
    targetPos.x = buildingTwoPos.x + 90;
    targetPos.y = 574 + buildingTwoPos.y;
    targetPos.height = 20;
    targetPos.width = 5;
    aimLine.origin = { buildingOnePos.x + 100, buildingOnePos.y - 50 };
    aimLine.notTheOrigin = { buildingOnePos.x + 400, buildingOnePos.y };
    bullets = 3;
}

void UpdateBullet(Bullet& bullet, float deltaTime) {
    if(hitXTheXTarget)
    {
        if(!gameOver)
        std::cout << "in first if kill" << '\n';
        isFired = false;
        //score = score + 1 + bullets;
        std::cout << "Score is " << score << "   + " << bullets << '\n';
        return;
    } else if(bullets <= 0)
    {
        isFired = false;
        gameOver = true;
        return;
    }else if(bullet.position.x > SCREEN_WIDTH || bullet.position.y > SCREEN_HEIGHT)
    {
        isFired = false;
        bullets = bullets - 1;
        if(bullets <= 0)
        {
            gameOver = true;
        }
        bullet.FireBullet({buildingOnePos.x + 100, buildingOnePos.y - 50}, gunAngle);
        return;
    } 
    
    bullet.time += deltaTime;
    bullet.position.x += bullet.velocity.x * deltaTime * SCALE_FACTOR;
    float gravity_term = 0.5f * (GRAVITY / (SCALE_FACTOR * SCALE_FACTOR)) * (bullet.time * bullet.time);
    bullet.position.y += (bullet.velocity.y * deltaTime + gravity_term) * SCALE_FACTOR;
    bullet.velocity.y += (GRAVITY / SCALE_FACTOR) * deltaTime;
    
}

void InitGame(void)
{
    std::cout << "In init" << '\n';
    int lbx = 1300, ubx = 1900, lby = -100, uby = 100; 
    buildingTwoPos.x = (rand() % (ubx - lbx + 1)) + lbx;
    buildingTwoPos.y = (rand() % (uby - lby + 1)) + lby;
    targetPos.x = buildingTwoPos.x + 90;
    targetPos.y = 574 + buildingTwoPos.y;
    targetPos.height = 20;
    targetPos.width = 5;
    aimLine.origin = { buildingOnePos.x + 100, buildingOnePos.y - 50 };
    aimLine.notTheOrigin = { buildingOnePos.x + 400, buildingOnePos.y };
    bullets = 3;
    score = 0;
}

void UpdateDrawFrame(Bullet& bullet)
{
    UpdateGame(bullet);
    DrawGame(bullet);
}

void UpdateGame(Bullet& bullet)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // if(isFired)
            // {
            UpdateBullet(bullet, GetFrameTime());
            if(!isFired)
            {
                DrawAimLine(bullet);
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


void DrawAimLine(Bullet& bullet)
{

    double radius = 300;
    if (IsKeyDown(KEY_UP)) 
    {
        magic = magic + .5;
    }
    if (IsKeyDown(KEY_DOWN)) 
    {
        magic = magic - .5;
    }
    aimLine.notTheOrigin.x = aimLine.origin.x + radius * cos(DegToRad(magic));
    aimLine.notTheOrigin.y = aimLine.origin.y - radius * sin(DegToRad(magic));

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gunAngle = DegToRad(magic);        
        bullet.FireBullet({buildingOnePos.x + 100, buildingOnePos.y - 50}, gunAngle);
        isFired = true;
    }
}

void DrawGame(Bullet& bullet)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            if(CheckCollisionCircleRec(bullet.position, 5.0, targetPos))
            {
                hitXTheXTarget = true;
                bullet.position = {buildingOnePos.x + 100, buildingOnePos.y - 50};
                bullet.velocity = {0, 0};
                isFired = false;
            }
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
            DrawRectangle(0, SCREEN_HEIGHT - 256, SCREEN_WIDTH, 256, GREEN);
            DrawRectangle(buildingOnePos.x, buildingOnePos.y, 200, 700, GRAY);
            DrawRectangle(buildingTwoPos.x, 594 + buildingTwoPos.y, 200, 500 - buildingTwoPos.y, YELLOW);
            DrawLine(aimLine.origin.x, aimLine.origin.y, aimLine.notTheOrigin.x, aimLine.notTheOrigin.y, BLACK);
            if(isFired && !gameOver)
            {
                DrawCircleV(bullet.position, 5.0f, RED);
            }
            DrawRectangleRec(targetPos, PURPLE);
            
            std::string text = "Score: " + std::to_string(score);
            DrawText(text.c_str(), 20, 20, 20, BLACK); 

            text = "Bullets: " + std::to_string(bullets);
            DrawText(text.c_str(), 120, 20, 20, BLACK); 


            
            if (pause) DrawText("GAME PAUSED", SCREEN_WIDTH/2 - MeasureText("GAME PAUSED", 40)/2, SCREEN_HEIGHT/2 - 40, 40, GRAY);
        }
        else 
        {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }
    EndDrawing();
}



          