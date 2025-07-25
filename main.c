#include "raylib.h"

int main (void)
{
  const int screen_width = 480;
  const int screen_height = 480;

  InitWindow(screen_width, screen_height, "GIF maker");
  SetTargetFPS(60);

  while(!WindowShouldClose())
    {
      BeginDrawing();
      ClearBackground(WHITE);
      EndDrawing();
    }

  CloseWindow();
  return 0;
}

