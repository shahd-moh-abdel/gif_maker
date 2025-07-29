
#include "raylib.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480
#define GRID_SIZE 16
#define MAX_COLOR_COUNT 24

int main (void)
{

  Color colors[MAX_COLOR_COUNT] = {
    LIGHTGRAY,GRAY,DARKGRAY,YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN, SKYBLUE,BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN, WHITE, BLACK, MAGENTA
  };
  int color_selected = 0;
  int color_mouse_hover  = 0; // for later

  
  
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GIF maker");
  int CELL_SIZE = SCREEN_WIDTH / GRID_SIZE;

  

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
      Vector2 mouse_pos = GetMousePosition();
      int grid_x = mouse_pos.x / CELL_SIZE;
      int grid_y = mouse_pos.y / CELL_SIZE;

      if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
	  grid[grid_y][grid_x] = colors[color_selected];
      }
      if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON)){
	grid[grid_y][grid_x] = WHITE;
      }
      if(IsKeyPressed(KEY_C)){
	for(int y = 0; y < GRID_SIZE; y++)
	  {
	    for(int x = 0; x < GRID_SIZE; x++)
	      {
		grid[x][y] = WHITE;
	      }
      
	  }
      }

      //color handling with keys
      if(IsKeyPressed(KEY_LEFT)) color_selected--;
      if(IsKeyPressed(KEY_RIGHT)) color_selected++;

      if (color_selected >= MAX_COLOR_COUNT) color_selected = MAX_COLOR_COUNT - 1 ;
      else if (color_selected < 0) color_selected = 0;
      
      BeginDrawing();
      ClearBackground(WHITE);

      
      for(int y = 0; y < GRID_SIZE; y++)
	{
	  for(int x = 0; x < GRID_SIZE; x++)
	    {
	      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, grid[y][x] );
	    }
      
	}
  

                 
      
      EndDrawing();
    }

  CloseWindow();
  return 0;
}

