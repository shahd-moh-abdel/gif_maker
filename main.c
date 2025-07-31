#include <stdlib.h>
#include <string.h>
#include <gif_lib.h>
#include <stdio.h>
#include "raylib.h"

#define SCREEN_WIDTH 780
#define SCREEN_HEIGHT 480
#define GRID_SIZE 16
#define MAX_COLOR_COUNT 24
#define MAX_FRAMES 12

// a new struct for frames
typedef struct {
  Color grid[GRID_SIZE][GRID_SIZE];
} Frame;



void save_frames_as_pngs(Frame* frames, int frame_count, int cell_size) {
  for(int i = 0; i < frame_count; i++){
    Image frame_image = {
      .data = frames[i].grid,
      .width = GRID_SIZE,
      .height = GRID_SIZE,
      .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
      .mipmaps = 1
    };

     Image scaled_image = ImageCopy(frame_image);
    ImageResizeNN(&scaled_image, GRID_SIZE * cell_size, GRID_SIZE * cell_size);
    char file_name[256];

    sprintf(file_name, "temp/frame_%03d.png", i);
    ExportImage(scaled_image, file_name);
    UnloadImage(scaled_image);
									     
  }
}


void create_gif_with_ffmeg(const char* output_file_name, int frame_delay) {

  char command[512];
  sprintf(command,
	  "ffmpeg -y -framerate %d -i temp/frame_%%03d.png "
	  "-vf \"scale=%d:%d:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse\" "
	  "%s",
	  100/frame_delay,
	  GRID_SIZE * 64,
	  GRID_SIZE * 64,
	  output_file_name);

  system(command);
  TraceLog(LOG_INFO, "gif saved");
  
}


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

  //frames storage
  Frame* frames = (Frame*)malloc(MAX_FRAMES * sizeof(Frame));
  int current_frame = 0;
  int total_frames = 1;

  

  Color grid[GRID_SIZE][GRID_SIZE];

  for(int y = 0; y < GRID_SIZE; y++)
    {
      for(int x = 0; x < GRID_SIZE; x++)
	{
	  frames[current_frame].grid[x][y] = WHITE;
	  grid[x][y]= WHITE;
	}
      
    }
  
  SetTargetFPS(60);

  RenderTexture2D canvas = LoadRenderTexture(SCREEN_WIDTH - 300, SCREEN_HEIGHT);

  while(!WindowShouldClose())
    {
      Vector2 mouse_pos = GetMousePosition();

      if(IsKeyPressed(KEY_A) && current_frame > 0) {
	current_frame--;
	
	memcpy(grid, frames[current_frame].grid, sizeof(grid));
      }

      if(IsKeyPressed(KEY_D) && current_frame < total_frames - 1){
	current_frame++;

	memcpy(grid, frames[current_frame].grid, sizeof(grid));
      }
      if(IsKeyPressed(KEY_N)&& total_frames < MAX_FRAMES){
	memcpy(frames[total_frames].grid, grid, sizeof(grid));
	current_frame = total_frames;
	total_frames++;
      }
      if(IsKeyPressed(KEY_DELETE) && total_frames > 1){
	current_frame = total_frames - 1;
	total_frames--;
      }
      if(IsKeyPressed(KEY_G)) {
	//save_as_gif();
	save_frames_as_pngs(frames, total_frames, CELL_SIZE);
	create_gif_with_ffmeg("animate.gif", 20);
      }
      
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

      memcpy(frames[current_frame].grid, grid, sizeof(grid));
      
      BeginTextureMode(canvas);
      
      for(int y = 0; y < GRID_SIZE; y++)
	{
	  for(int x = 0; x < GRID_SIZE; x++)
	    {
	      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1 , CELL_SIZE -1 , grid[x][y] );

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

      DrawText(TextFormat("Frame: %d / %d", current_frame + 1, total_frames), 10, SCREEN_HEIGHT - 30, 20, WHITE);

      
      EndDrawing();
    }

  free(frames);
  UnloadRenderTexture(canvas);

  CloseWindow();
  return 0;
}



