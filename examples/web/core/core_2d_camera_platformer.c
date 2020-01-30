/*******************************************************************************************
*
*   raylib [core] example - 2d camera platformer
*
*   This example has been created using raylib 2.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Example contributed by arvyy (@arvyy) and reviewed by Ramon Santamaria (@raysan5)
*
*   Copyright (c) 2019 arvyy (@arvyy)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#define G 400
#define PLAYER_JUMP_SPD 350.f
#define PLAYER_HOR_SPD 200.f

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 450;

static Player player = { 0 };

static EnvItem envItems[] = {
    {{ 0, 0, 1000, 400 }, 0, LIGHTGRAY },
    {{ 0, 400, 1000, 200 }, 1, GRAY },
    {{ 300, 200, 400, 10 }, 1, GRAY },
    {{ 250, 300, 100, 10 }, 1, GRAY },
    {{ 650, 300, 100, 10 }, 1, GRAY }
};

static int envItemsLength = 0;

static Camera2D camera = { 0 };

static int cameraOption = 0;
static int cameraUpdatersLength = 0;

const char *cameraDescriptions[] = {
    "Follow player center",
    "Follow player center, but clamp to map edges",
    "Follow player center; smoothed",
    "Follow player center horizontally; updateplayer center vertically after landing",
    "Player push camera on getting too close to screen edge"
};

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

// Store pointers to the multiple update camera functions
void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
    UpdateCameraCenter,
    UpdateCameraCenterInsideMap,
    UpdateCameraCenterSmoothFollow,
    UpdateCameraEvenOutOnLanding,
    UpdateCameraPlayerBoundsPush
};

//----------------------------------------------------------------------------------
// Program Main Entry Point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.canJump = false;
    
    envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    cameraUpdatersLength = sizeof(cameraUpdaters)/sizeof(cameraUpdaters[0]);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definitions
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    float deltaTime = GetFrameTime();
    
    UpdatePlayer(&player, envItems, envItemsLength, deltaTime);

    camera.zoom += ((float)GetMouseWheelMove()*0.05f);
    
    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
    
    if (IsKeyPressed(KEY_R)) 
    {
        camera.zoom = 1.0f;
        player.position = (Vector2){ 400, 280 };
    }

    if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1)%cameraUpdatersLength;

    // Call update camera function by its pointer
    cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);
       
            for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);

            Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
            DrawRectangleRec(playerRect, RED);
           
        EndMode2D();

        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- Right/Left to move", 40, 40, 10, DARKGRAY);
        DrawText("- Space to jump", 40, 60, 10, DARKGRAY);
        DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, DARKGRAY);
        DrawText("- C to change camera mode", 40, 100, 10, DARKGRAY);
        DrawText("Current camera mode:", 20, 120, 10, BLACK);
        DrawText(cameraDescriptions[cameraOption], 40, 140, 10, DARKGRAY);
        
    EndDrawing();
    //----------------------------------------------------------------------------------
}


void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump) 
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++) 
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x && 
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y < p->y + player->speed*delta) 
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rect.y;
        }
    }
    
    if (!hitObstacle) 
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    } 
    else player->canJump = true;
}

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){ width/2, height/2 };
    camera->target = player->position;
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = player->position;
    camera->offset = (Vector2){ width/2, height/2 };
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
    
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }
    
    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);
    
    if (max.x < width) camera->offset.x = width - (max.x - width/2);
    if (max.y < height) camera->offset.y = height - (max.y - height/2);
    if (min.x > 0) camera->offset.x = width/2 - min.x;
    if (min.y > 0) camera->offset.y = height/2 - min.y;
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;
    
    camera->offset = (Vector2){ width/2, height/2 };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);
    
    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;
    
    camera->offset = (Vector2){ width/2, height/2 };
    camera->target.x = player->position.x;
    
    if (eveningOut)
    {
        if (evenOutTarget > camera->target.y) 
        {
            camera->target.y += evenOutSpeed*delta;
            
            if (camera->target.y > evenOutTarget) 
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        } 
        else 
        {
            camera->target.y -= evenOutSpeed*delta;
            
            if (camera->target.y < evenOutTarget) 
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
    } 
    else 
    {
        if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
        {
            eveningOut = 1;
            evenOutTarget = player->position.y;
        }
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height) 
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*width, (1 - bbox.y)*0.5f*height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*width, (1 + bbox.y)*0.5f*height }, *camera);
    camera->offset = (Vector2){ (1 - bbox.x)*0.5f * width, (1 - bbox.y)*0.5f*height };

    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}
