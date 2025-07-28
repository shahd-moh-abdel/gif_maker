
#include "raylib.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480
#define GRID_SIZE 16
#define CELL_SIZE 30

int main (void)
{
  
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GIF maker");

  Color grid[GRID_SIZE][GRID_SIZE];

  for(int y = 0; y < GRID_SIZE; y++)
    {
      for(int x = 0; x < GRID_SIZE; x++)
	{
	  grid[x][y] = WHITE;
	}
      
    }
  
  SetTargetFPS(60);

  while(!WindowShouldClose())
    {
      BeginDrawing();
      ClearBackground(BLACK);

      
      for(int y = 0; y < GRID_SIZE; y++)
	{
	  for(int x = 0; x < GRID_SIZE; x++)
	    {
	      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, grid[y][x] );
	    }
      
	}
  

                 
      
      EndDrawing();
    }

  CloseWindow();
  return 0;
}

