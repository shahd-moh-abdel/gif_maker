
#include "raylib.h"

#define SCREEN_WIDTH 780
#define SCREEN_HEIGHT 480
#define GRID_SIZE 8
#define MAX_COLOR_COUNT 24

int main (void)
{

  Color colors[MAX_COLOR_COUNT] = {
    LIGHTGRAY,GRAY,DARKGRAY,YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN, SKYBLUE,BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN, WHITE, BLACK, MAGENTA
  };
  int color_selected = 0;
  int color_mouse_hover  = 0; // for later

  //define color recs
  Rectangle colors_recs[MAX_COLOR_COUNT] = { 0 };

  int colors_per_row = 8;
  int rect_width = 30;
  int rect_height = 30;
  int spacing = 4;
  int start_x = 14;
  int start_y = 14;

  for (int i = 0; i < MAX_COLOR_COUNT; i++){
    int row = i / colors_per_row;
    int col = i % colors_per_row;
    colors_recs[i].x = start_x + (rect_width + spacing ) * col;
    colors_recs[i].y =  start_y + (rect_height + spacing ) * row;
    colors_recs[i].width = rect_width;
    colors_recs[i].height = rect_height;
      }
  
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GIF maker");
  int CELL_SIZE = SCREEN_HEIGHT / GRID_SIZE;

  

  Color grid[GRID_SIZE][GRID_SIZE];

  for(int y = 0; y < GRID_SIZE; y++)
    {
      for(int x = 0; x < GRID_SIZE; x++)
	{
	  grid[x][y] = WHITE;
	}
      
    }
  
  SetTargetFPS(60);

  RenderTexture2D canvas = LoadRenderTexture(SCREEN_WIDTH - 300, SCREEN_HEIGHT);

  while(!WindowShouldClose())
    {
      Vector2 mouse_pos = GetMousePosition();


      
      if(
	 mouse_pos.x > 300 && mouse_pos.x < SCREEN_WIDTH
	 && mouse_pos.y > 0 && mouse_pos.y < SCREEN_HEIGHT){
	
	int grid_x = (mouse_pos.x - 300) / CELL_SIZE;
	int grid_y = mouse_pos.y / CELL_SIZE;
	
	if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
	  grid[grid_x][grid_y] = colors[color_selected];
	}
	if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON)){
	  grid[grid_x][grid_y] = WHITE;
	}
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

      //color handling
      //with keys 
      if(IsKeyPressed(KEY_LEFT)) color_selected--;
      if(IsKeyPressed(KEY_RIGHT)) color_selected++;
      if(IsKeyPressed(KEY_UP)) color_selected -= 8;
      if(IsKeyPressed(KEY_DOWN)) color_selected += 8;
      if (color_selected >= MAX_COLOR_COUNT) color_selected = MAX_COLOR_COUNT - 1 ;
      else if (color_selected < 0) color_selected = 0;
      

      //with mouse
      for(int i = 0 ; i < MAX_COLOR_COUNT ; i++){
	if(CheckCollisionPointRec(mouse_pos, colors_recs[i])){
	  color_mouse_hover = i;
	  break;
	} else color_mouse_hover = -1;
      }
      if((color_mouse_hover >= 0 ) && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
	  color_selected = color_mouse_hover;
	}
      
      BeginTextureMode(canvas);
      
      for(int y = 0; y < GRID_SIZE; y++)
	{
	  for(int x = 0; x < GRID_SIZE; x++)
	    {
	      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, grid[x][y] );

	    }
      
	}
 

      EndTextureMode();   

      
      BeginDrawing();
      ClearBackground(SKYBLUE);

      //draw colors
      for(int i = 0; i < MAX_COLOR_COUNT; i++){
	DrawRectangleRec(colors_recs[i] , colors[i]);
	if(i == color_selected) {
	  DrawRectangleLinesEx(colors_recs[i], 3 , WHITE);
	}
	
      }
      
      DrawTextureRec(canvas.texture, (Rectangle){0, 0, (float)canvas.texture.width, (float)-canvas.texture.height},(Vector2){300, 0}, WHITE);


      
      EndDrawing();
    }

  CloseWindow();
  return 0;
}

