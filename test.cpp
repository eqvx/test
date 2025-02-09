#include "serek.h"

std::vector<Rectangle> recs;

std::vector<Color> colors = { WHITE, RAYWHITE, GRAY, LIGHTGRAY, DARKGRAY, GREEN, DARKGREEN, YELLOW, BLUE, DARKBLUE, BROWN, DARKBROWN, RED, PURPLE, DARKPURPLE };

bool cl_noclipenabled = false;
float cl_speedmultiplier = 2;

bool isCollidingRecs(Rectangle& collider, std::vector<Rectangle>& collidables) {

    for (const Rectangle& collidable: collidables) {
        bool colliding = CheckCollisionRecs(collider, collidable);

        if (colliding)
            return true;
    }

    return false;
}

inline void renderRects() {
    for (const Rectangle& rec: recs) {
        DrawRectangleRec(rec, rec.color);
    }
}

Rectangle createRect(float width, float height, float x, float y, Color color, bool push) {
    Rectangle rect = {};
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    rect.color = color;

    if (push) {
        recs.push_back(rect);
    }

    return rect;
}

int main()
{

    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;
    
    std::cout << GetScreenWidth() << "\n";

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, ":skul:");

    InitAudioDevice();
    
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    int radius = 10;

    Texture2D PlayerTexture = LoadTexture("assets/a.png");
    Image windowImage = LoadImage("assets/a.png");
    SetWindowIcon(windowImage);
    UnloadImage(windowImage);

    ToggleFullscreen();

    PlayerTexture.height = 30;
    PlayerTexture.width = 30;
    float posX = 0;
    float posY = 0;

    Sound oof = LoadSound("assets/oof.mp3");
    Sound tada = LoadSound("assets/tada.mp3");
    SetMasterVolume((float)0.1);
    float speed = 100 * cl_speedmultiplier;

    bool enraged = false;
    Color clr = WHITE;

    for (int i = 0; i <= GetRandomValue(250, 3000); i++) {
        createRect((float)GetRandomValue(20, 60), (float)GetRandomValue(20, 60), (float)GetRandomValue(0, 2560), (float)GetRandomValue(0, 1080), colors[GetRandomValue(0, (int)colors.size() - 1)], true);
    }

    Rectangle hitbox = createRect(20, 20, 0, 0, RED, false);
    bool showHitbox = false;

    float oldPosX = 0;
    float oldPosY = 0;
    
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        speed = 100 * cl_speedmultiplier;

        if (IsKeyPressed(KEY_F))
            showHitbox = not showHitbox;
        
        if (IsKeyPressed(KEY_E)) {
            enraged = not enraged;
            cl_noclipenabled = enraged;

            if (enraged) {
                clr = RED;
                cl_speedmultiplier = 5;
            }

            if (!enraged) {
                clr = WHITE;
                cl_speedmultiplier = 2;
            }
        }
        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();
        if (IsKeyDown(KEY_R)) {
            std::thread worker([] {
                for (int i = 0; i < recs.size(); i++)
                    recs.erase(recs.begin() + i);
                });
            worker.join();

            for (int i = 0; i <= GetRandomValue(250, 3000); i++) {
                createRect((float)GetRandomValue(20, 60), (float)GetRandomValue(20, 60), (float)GetRandomValue(0, 2560), (float)GetRandomValue(0, 1080), colors[GetRandomValue(0, (int)colors.size() - 1)], true);
            }
        }
        
        float deltaTime = GetFrameTime();
        if (IsKeyDown(KEY_W) && posY >= 0)
            posY -= speed * deltaTime;
        if (IsKeyDown(KEY_S) && posY + 30 < GetScreenHeight())
            posY += speed * deltaTime;

        if (IsKeyDown(KEY_A) && posX >= 0)
            posX -= speed * deltaTime;
        if (IsKeyDown(KEY_D) && posX + 30 < GetScreenWidth())
            posX += speed * deltaTime;
        else if (IsKeyDown(KEY_D) and not IsSoundPlaying(tada))
            PlaySound(tada);
        /* Player Hitbox */
        hitbox.x = posX + 5;
        hitbox.y = posY + 5;
        BeginDrawing();
        //GuiButton(createRect(30, 30, 200, 200, DARKBLUE, false), "#191#Show Message!");
        ClearBackground(SKYBLUE);
        
        renderRects();

        std::string a = "k: " + std::to_string((float)recs.size() / 250);

        DrawText(a.c_str(), 2, 40, 20, RED);

        if (!cl_noclipenabled) {
            bool colliding = isCollidingRecs(hitbox, recs);

            if (colliding) {
                /*posX = oldPosX;
                posY = oldPosY;*/

                posX = 0;
                posY = 0;

                if (not IsSoundPlaying(oof))
                    PlaySound(oof);
            }
        }
        else {
            DrawText("[NOCLIP]", 2, 20, 20, RED);
        }
            
        /* Player Texture */

        DrawTexture(PlayerTexture, (int)posX, (int)posY, clr);

        if (showHitbox)
            DrawRectangleRec(hitbox, hitbox.color);

        DrawFPS(0, 0);
        EndDrawing();


        oldPosX = posX;
        oldPosY = posY;
    }

    UnloadTexture(PlayerTexture);
    UnloadSound(oof);
    UnloadSound(tada);

    CloseWindow();        
    return 0;
}