#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "raylib.h"

#define SCREEN_WIDTH 780
#define SCREEN_HEIGHT 480
#define GRID_SIZE 20
#define MAX_COLOR_COUNT 24
#define MAX_FRAMES 24

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
    ImageRotate(&scaled_image, 90);
    ImageFlipHorizontal(&scaled_image);
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
    LIGHTGRAY,GRAY,DARKGRAY,YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN, SKYBLUE,BLUE, DARKBLUE,MAGENTA, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN, BLACK, WHITE
  };

  int color_selected = 0;
  int color_mouse_hover  = 0; 
  bool show_save_message = false;
  int save_message_counter = 0;
  
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

  //define controls recs {x, y, width, height}
  Rectangle save_rec = {10, 125, 133.5, 30};
  Rectangle clear_rec = {153.5,125, 133.5, 30};
  Rectangle frame_rec = {10, 165 , 133.5, 30};
  Rectangle prev_frame_rec = {153.5, 165, 64.75, 30};
  Rectangle next_frame_rec = {222.25, 165, 64.75, 30};
  Rectangle add_frame_rec = {10, 205, 133.5, 30};
  Rectangle delete_frame_rec = {153.5, 205, 133.5, 30};
  
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
      //prev frame
      if((IsKeyPressed(KEY_A) && current_frame > 0) || (CheckCollisionPointRec( mouse_pos, prev_frame_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && current_frame > 0) ) {
	current_frame--;
	
	memcpy(grid, frames[current_frame].grid, sizeof(grid));
      }


      //next frame
      if((IsKeyPressed(KEY_D) && current_frame < total_frames - 1) || (CheckCollisionPointRec( mouse_pos, next_frame_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && current_frame <  total_frames - 1)){
	current_frame++;

	memcpy(grid, frames[current_frame].grid, sizeof(grid));
      }

      //new frame
      if((IsKeyPressed(KEY_N)&& total_frames < MAX_FRAMES) ||  (CheckCollisionPointRec( mouse_pos, add_frame_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && current_frame < MAX_FRAMES)){
	memcpy(frames[total_frames].grid, grid, sizeof(grid));
	current_frame = total_frames;
	total_frames++;
      }

      //delete frame
      if((IsKeyPressed(KEY_DELETE) && total_frames > 1) || (CheckCollisionPointRec( mouse_pos, delete_frame_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && total_frames > 1)){
	current_frame = total_frames - 2;
	total_frames--;
      }

      //save gif
      if(IsKeyPressed(KEY_G) || (CheckCollisionPointRec( mouse_pos, save_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
	save_frames_as_pngs(frames, total_frames, CELL_SIZE);
	create_gif_with_ffmeg("animate.gif", 20);
	show_save_message = true;
      }

      
      //draw and erase 
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


      //clear
      if(IsKeyPressed(KEY_C) ||( CheckCollisionPointRec(mouse_pos, clear_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
	for(int y = 0; y < GRID_SIZE; y++)
	  {
	    for(int x = 0; x < GRID_SIZE; x++)
	      {
		grid[x][y] = WHITE;
	      }
      
	  }
      }

      //show save message for 2 seconds
      if(show_save_message){
	save_message_counter++;
	if(save_message_counter > 200){
	  show_save_message = false;
	  save_message_counter = 0;
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
      ClearBackground(BLACK);

      //colors
      DrawRectangle(10,10, 277, 105, RAYWHITE);
      //draw colors
      for(int i = 0; i < MAX_COLOR_COUNT; i++){
	DrawRectangleRec(colors_recs[i] , colors[i]);
	if(i == color_selected && i == MAX_COLOR_COUNT - 1){
	  DrawRectangleLinesEx(colors_recs[i], 3 , DARKGRAY);
	}else if(i == color_selected) {
	  DrawRectangleLinesEx(colors_recs[i], 3 , WHITE);
	}
	
      }

      //
      DrawRectangleRec(save_rec, WHITE);
      DrawText(TextFormat("Save GIF"),14, save_rec.y + 5, 20, BLACK);
      
      DrawRectangleRec(clear_rec, WHITE);
      DrawText(TextFormat("Clear Frame"),clear_rec.x + 4, clear_rec.y + 5, 20, BLACK);
      
      DrawRectangleRec(frame_rec, WHITE);
      DrawText(TextFormat("Frame:%d/%d", current_frame + 1, total_frames), frame_rec.x + 4, frame_rec.y + 5, 20, BLACK);

      DrawRectangleRec(prev_frame_rec, WHITE);
      DrawText(TextFormat("<"), prev_frame_rec.x + 32, prev_frame_rec.y + 5, 20, BLACK);
      
      DrawRectangleRec(next_frame_rec, WHITE);
      DrawText(TextFormat(">"), next_frame_rec.x + 32, next_frame_rec.y + 5, 20, BLACK);

      DrawRectangleRec(add_frame_rec, WHITE);
      DrawText(TextFormat("Add"), add_frame_rec.x + 4, add_frame_rec.y + 5, 20, BLACK);
      
      DrawRectangleRec(delete_frame_rec, WHITE);
      DrawText(TextFormat("Delete"), delete_frame_rec.x + 4, delete_frame_rec.y + 5, 20, BLACK);

      DrawTextureRec(canvas.texture, (Rectangle){0, 0, (float)canvas.texture.width, (float)-canvas.texture.height},(Vector2){300, 0}, WHITE);


      //draw recs when hover
      if(CheckCollisionPointRec(mouse_pos, clear_rec)){
	DrawRectangleLinesEx(clear_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, save_rec)){
	DrawRectangleLinesEx(save_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, frame_rec)){
	DrawRectangleLinesEx(frame_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, prev_frame_rec)){
	DrawRectangleLinesEx(prev_frame_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, next_frame_rec)){
	DrawRectangleLinesEx(next_frame_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, add_frame_rec)){
	DrawRectangleLinesEx(add_frame_rec, 2, RED);
      }
      if(CheckCollisionPointRec(mouse_pos, delete_frame_rec)){
	DrawRectangleLinesEx(delete_frame_rec, 2, RED);
      }

      if(show_save_message){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});
        DrawRectangle(0, (SCREEN_HEIGHT/2) - 40, SCREEN_WIDTH, 80, DARKGREEN);
	DrawText("GIF SAVED", (SCREEN_WIDTH / 2) - 80,( SCREEN_HEIGHT/2) - 20, 36, WHITE );
      }
      
      EndDrawing();
    }

  free(frames);
  UnloadRenderTexture(canvas);

  CloseWindow();
  return 0;
}
