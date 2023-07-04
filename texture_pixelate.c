#include <stdlib.h>
#include <time.h>

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

// (x, y) are the 'pixel' coordinates
// imSrc is the source image
// imDst is the destination image
void pixel(Image *imSrc, Image *imDst, unsigned x, unsigned y, unsigned pixelSize) {
    int totRed, totBlue, totGreen;
    totRed = totBlue = totGreen = 0;
    unsigned pixelSquare = pixelSize * pixelSize;

    //@formatter:off
    // average color on pixelSize * pixelSize area
    for (int oy = 0; oy < pixelSize; ++oy) {
        for (int ox = 0; ox < pixelSize; ++ox) {
            unsigned p = (y * pixelSize + oy) * imSrc->width * 4 + (x * pixelSize + ox) * 4;
            totRed   += ((unsigned char *) imSrc->data)[p];
            totGreen += ((unsigned char *) imSrc->data)[p + 1];
            totBlue  += ((unsigned char *) imSrc->data)[p + 2];
        }
    }

    // set color on pixelSize * pixelSize area
    for (int oy = 0; oy < pixelSize; ++oy) {
        for (int ox = 0; ox < pixelSize; ++ox) {
            unsigned p = (y * pixelSize + oy) * imDst->width * 4 + (x * pixelSize + ox) * 4;
            ((unsigned char *) imDst->data)[p]     = totRed / pixelSquare;
            ((unsigned char *) imDst->data)[p + 1] = totGreen / pixelSquare;
            ((unsigned char *) imDst->data)[p + 2] = totBlue / pixelSquare;
        }
    }
    //@formatter:on
}

Image GenPixelate(Image *imOrig, int pixelSize) {
    int imNew_width = (imOrig->width / pixelSize) * pixelSize;
    int imNew_height = (imOrig->height / pixelSize) * pixelSize;
    Image imNew = GenImageColor(imNew_width, imNew_height, BLUE);
    for (unsigned y = 0; y < imNew.height / pixelSize; y++) {
        for (unsigned x = 0; x < imNew.width / pixelSize; x++) {
            pixel(imOrig, &imNew, x, y, pixelSize);
        }
    }
    return imNew;
}

//
// animate pixelate
//
typedef struct {
    unsigned x, y;
} Coord;

typedef struct {
    bool finished;
    int pixelSize;
    Image image;
    Coord *coords;
    unsigned idx;
} PixelateAnim;

void PixelateInit(PixelateAnim *anim, Image *imOrig, int pixelSize) {
    anim->pixelSize = pixelSize;
    anim->finished = false;
    int imNew_width = (imOrig->width / pixelSize) * pixelSize;
    int imNew_height = (imOrig->height / pixelSize) * pixelSize;
    anim->image = GenImageColor(imNew_width, imNew_height, BLUE);

    for (unsigned y = 0; y < anim->image.height / pixelSize; ++y) {
        for (unsigned x = 0; x < anim->image.width / pixelSize; ++x) {
            for (int oy = 0; oy < pixelSize; ++oy) {
                for (int ox = 0; ox < pixelSize; ++ox) {
                    unsigned src = (y * pixelSize + oy) * imOrig->width * 4 + (x * pixelSize + ox) * 4;
                    unsigned dst = (y * pixelSize + oy) * anim->image.width * 4 + (x * pixelSize + ox) * 4;
                    ((unsigned char *) anim->image.data)[dst] = ((unsigned char *) imOrig->data)[src];
                    ((unsigned char *) anim->image.data)[dst + 1] = ((unsigned char *) imOrig->data)[src + 1];
                    ((unsigned char *) anim->image.data)[dst + 2] = ((unsigned char *) imOrig->data)[src + 2];
                }
            }
        }
    }

    anim->coords = (Coord *) malloc((anim->image.width / pixelSize) * (anim->image.height / pixelSize) * sizeof(Coord));
    for (unsigned y = 0; y < anim->image.height / pixelSize; ++y) {
        for (unsigned x = 0; x < anim->image.width / pixelSize; ++x) {
            anim->coords[y * anim->image.width / pixelSize + x].x = x;
            anim->coords[y * anim->image.width / pixelSize + x].y = y;
        }
    }
    anim->idx = 0;
}

void PixelateUnload(PixelateAnim *anim) {
    UnloadImage(anim->image);
    free(anim->coords);
    anim->coords = NULL;
}

void PixelateStep(PixelateAnim *anim) {
    int nr_pixel_for_step = ((anim->image.width / anim->pixelSize) * (anim->image.height / anim->pixelSize)) / 100;
    if (nr_pixel_for_step < 1) nr_pixel_for_step = 1;
    for (int i = 0; i < nr_pixel_for_step && !anim->finished; ++i) {
        if (anim->finished) return;

        int p = GetRandomValue(anim->idx,
                               (anim->image.width / anim->pixelSize) * (anim->image.height / anim->pixelSize) - 1);

        Coord tmp;
        tmp.x = anim->coords[anim->idx].x;
        tmp.y = anim->coords[anim->idx].y;

        anim->coords[anim->idx].x = anim->coords[p].x;
        anim->coords[anim->idx].y = anim->coords[p].y;

        anim->coords[p].x = tmp.x;
        anim->coords[p].y = tmp.y;

        pixel(&(anim->image), &(anim->image), anim->coords[anim->idx].x, anim->coords[anim->idx].y, anim->pixelSize);
        anim->idx++;
        if (anim->idx == (anim->image.width / anim->pixelSize) * (anim->image.height / anim->pixelSize)) {
            anim->finished = true;
        }
    }
}

//
// main
//
int main(void) {
    SetRandomSeed(time(NULL));
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib - pixelate");

    Image imOrig = LoadImage("resources/parrots.png");
    ImageFormat(&imOrig, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Texture2D txOrig = LoadTextureFromImage(imOrig);

    int pixelSize = 20;
    int newPixelSize = 20;

    Image imPixelated = GenPixelate(&imOrig, pixelSize);
    Texture2D txPixelated = LoadTextureFromImage(imPixelated);
    UnloadImage(imPixelated);

    bool animate = false;
    PixelateAnim *anim = NULL;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (!animate) {
            if (anim != NULL) {
                PixelateUnload(anim);
                free(anim);
                anim = NULL;
            }
            if (pixelSize != newPixelSize) {
                UnloadTexture(txPixelated);
                imPixelated = GenPixelate(&imOrig, pixelSize);
                txPixelated = LoadTextureFromImage(imPixelated);
                UnloadImage(imPixelated);
                pixelSize = newPixelSize;
            }
        }

        if (animate) {
            if (anim == NULL) {
                anim = (PixelateAnim *) malloc(sizeof(PixelateAnim));
                PixelateInit(anim, &imOrig, pixelSize);
                UnloadTexture(txPixelated);
                txPixelated = LoadTextureFromImage(anim->image);
            } else if (!anim->finished) {
                PixelateStep(anim);
                UnloadTexture(txPixelated);
                txPixelated = LoadTextureFromImage(anim->image);
            } else if (anim->pixelSize != newPixelSize) {
                PixelateUnload(anim);
                PixelateInit(anim, &imOrig, pixelSize);
                UnloadTexture(txPixelated);
                txPixelated = LoadTextureFromImage(anim->image);
            }
            pixelSize = newPixelSize;
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawText("pixelate an image - change the pixel size", 40, 10, 20, DARKGRAY);
            Vector2 origin = {20, 45};
            DrawTexture(txPixelated, origin.x, origin.y, WHITE);
            DrawRectangleLines(origin.x, origin.y, txPixelated.width, txPixelated.height, BLACK);

            // gui
            int x = 620;
            newPixelSize = GuiSliderBar(
                    (Rectangle) {x, 80, 120, 20}, "Pixel Size", TextFormat("%d", pixelSize),
                    pixelSize, 4, imOrig.width / 8);
            if (animate) {
                if (anim->finished) {
                    DrawText("finished", x, 145, 10, BLACK);
                } else {
                    DrawText("running", x, 145, 10, BLACK);
                }
            }
            animate = GuiCheckBox((Rectangle) {x, 120, 20, 20}, "Animate", animate);
        }
        EndDrawing();
    }

    UnloadImage(imOrig);
    UnloadTexture(txOrig);
    CloseWindow();
    return 0;
}