#include <stdio.h>

#include "raylib.h"
#include "raymath.h"

void DrawImageViewer(Texture2D texture, Rectangle view, Vector2 offset) {
    Rectangle source = {0};
    Rectangle dest = {0};

    //@formatter:off
    bool inside_view = true;
    dest.x = fmaxf(view.x + offset.x, view.x);
    dest.y = fmaxf(view.y + offset.y, view.y);
    if ((dest.x >= view.x + view.width) || (dest.y>= view.y+dest.y)) {
        inside_view = false;
    } else {
        dest.width  = fminf(offset.x >= 0 ? view.width  - offset.x  : texture.width,
                            offset.x >= 0 ? texture.width           : texture.width  + offset.x);
        dest.height = fminf(offset.y >= 0 ? view.height - offset.y  : view.height,
                            offset.y >= 0 ? texture.height          : texture.height + offset.y);
    }

    source.x = fmaxf(0.0f, -offset.x); // if offset.x is negative use -offset.x
    source.y = fmaxf(0.0f, -offset.y);
    source.width =  fminf(dest.width,  offset.x >= 0 ? texture.width  : texture.width  - offset.x);
    source.height = fminf(dest.height, offset.y >= 0 ? texture.height : texture.height - offset.y);
    //@formatter:on

    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(texture, source, dest, origin, 0, WHITE);
    DrawRectangleLinesEx(view, 1, DARKGRAY);

    DrawText(TextFormat("(%.2f, %.2f)", offset.x, offset.y), view.x - 50, view.y - 15, 10, DARKGRAY);
    DrawText(TextFormat("(%.2f, %.2f)", offset.x + view.width, offset.y + view.height),
             view.x + view.width - 50, view.y + view.height + 10, 10, DARKGRAY);
}

typedef struct {
    bool north, west, south, est;
    bool home;
} ArrowsKeysStatus;

void DrawArrowKeys(Vector2 origin, ArrowsKeysStatus status) {
    // nord
    DrawTriangle(
            (Vector2) {17.0f + origin.x, 0.0f + origin.y},
            (Vector2) {12.0f + origin.x, 10.0f + origin.y},
            (Vector2) {22.0f + origin.x, 10.0f + origin.y},
            status.north ? DARKGRAY : LIGHTGRAY);
    // ovest
    DrawTriangle(
            (Vector2) {0.0f + origin.x, 18.0f + origin.y},
            (Vector2) {10.0f + origin.x, 23.0f + origin.y},
            (Vector2) {10.0f + origin.x, 13.0f + origin.y},
            status.west ? DARKGRAY : LIGHTGRAY);
    // south
    DrawTriangle(
            (Vector2) {17.0f + origin.x, 38.0f + origin.y},
            (Vector2) {22.0f + origin.x, 28.0f + origin.y},
            (Vector2) {12.0f + origin.x, 28.0f + origin.y},
            status.south ? DARKGRAY : LIGHTGRAY);

    // est
    DrawTriangle(
            (Vector2) {34.0f + origin.x, 18.0f + origin.y},
            (Vector2) {24.0f + origin.x, 13.0f + origin.y},
            (Vector2) {24.0f + origin.x, 23.0f + origin.y},
            status.est ? DARKGRAY : LIGHTGRAY);
    DrawCircle(17 + origin.x, 18 + origin.y, 5, status.home ? DARKGRAY : LIGHTGRAY);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib - panning and zooming");
    Image parrots = LoadImage("resources/parrots.png");
    Texture2D texture = LoadTextureFromImage(parrots);
    UnloadImage(parrots);

    float view_width = 600;
    float view_height = 350;
    Rectangle view = {screenWidth / 2 - view_width / 2, screenHeight / 2 - view_height / 2, view_width, view_height};

    Vector2 offset = {0.0f};
    SetTargetFPS(60);
    ArrowsKeysStatus status = {false};
    while (!WindowShouldClose()) {
//        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
//            Vector2 delta = GetMouseDelta();
//            offset = Vector2Subtract(offset, delta);
//        }

        status.north = status.west = status.south = status.est = false;
        status.home = false;
        if (IsKeyDown(KEY_HOME)) {
            offset.x = offset.y = 0;
            status.home = true;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            offset.x += 2;
            status.est = true;
        }
        if (IsKeyDown(KEY_LEFT)) {
            offset.x -= 2;
            status.west = true;
        }
        if (IsKeyDown(KEY_UP)) {
            offset.y -= 2;
            status.north = true;
        }
        if (IsKeyDown(KEY_DOWN)) {
            offset.y += 2;
            status.south = true;
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawImageViewer(texture, view, offset);
            DrawArrowKeys((Vector2) {view.x + view.width + 20, view.y + view.height - 50}, status);
            DrawText("use arrows key to pan image, home to reset", view.x, 10, 15, DARKGRAY);
//            DrawText("use home to reset", view.x, 30, 15, DARKGRAY);
            DrawFPS(10, 10);

        }
        EndDrawing();
    }

    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    return 0;
}