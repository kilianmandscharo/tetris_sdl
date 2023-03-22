#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUITKEY SDLK_ESCAPE

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 40
#define WIDTH BOARD_WIDTH *BLOCK_SIZE
#define HEIGHT BOARD_HEIGHT *BLOCK_SIZE

SDL_Window *screen = NULL;
SDL_Renderer *renderer;
SDL_Event event;

int keypressed;

int board[BOARD_HEIGHT][BOARD_WIDTH];
int shape[4][2];
int shape_type;

uint8_t colors[8][3];

void init();
void game_loop();
void finish();
char get_char();
void draw_board();
void new_random_shape();
void move_shape_down();
void move_shape_left();
void move_shape_right();
void move_shape_one_down();
void draw_shape();
int shape_collision();
void render();
void rotate_shape();
void clear_lines();
bool line_filled(int line);
void move_lines(int line);

int main(int argc, char *args[]) {
  init();
  new_random_shape();
  game_loop();
  finish();
  return 0;
}

void move_lines(int line) {
  for (int row = line; row > 0; row--) {
    for (int col = 0; col < BOARD_WIDTH; col++) {
      board[row][col] = board[row - 1][col];
    }
  }
}

bool line_filled(int line) {
  for (int i = 0; i < BOARD_WIDTH; i++) {
    if (board[line][i] == 0) {
      return false;
    }
  }
  return true;
}

void clear_lines() {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    if (line_filled(i)) {
      move_lines(i);
    }
  }
}

void rotate_shape() {
  double angle = M_PI / 2;
  float s = sin(angle);
  float c = cos(angle);
  float origin_x = shape[0][0];
  float origin_y = shape[0][1];

  for (int i = 0; i < 4; i++) {
    float x = shape[i][0] - origin_x;
    float y = shape[i][1] - origin_y;
    float xnew = c * x - s * y;
    float ynew = s * x + c * y;
    shape[i][0] = xnew + origin_x;
    shape[i][1] = ynew + origin_y;
  }
}

void draw_shape() {
  for (int i = 0; i < 4; i++) {
    SDL_SetRenderDrawColor(renderer, colors[shape_type][0],
                           colors[shape_type][1], colors[shape_type][2], 255);
    SDL_Rect rect;
    rect.h = BLOCK_SIZE;
    rect.w = BLOCK_SIZE;
    rect.y = BLOCK_SIZE * shape[i][1];
    rect.x = BLOCK_SIZE * shape[i][0];
    SDL_RenderFillRect(renderer, &rect);
  }
}

void render() {
  draw_board();
  draw_shape();
  SDL_RenderPresent(renderer);
}

int shape_collision() {
  bool collision = false;
  for (int i = 0; i < 4; i++) {
    if (shape[i][1] == BOARD_HEIGHT - 1 ||
        board[shape[i][1] + 1][shape[i][0]] > 0) {
      collision = true;
      if (shape[i][1] == 1) {
        return 0;
      }
    }
  }
  if (!collision) {
    return 1;
  }
  for (int i = 0; i < 4; i++) {
    int x = shape[i][0];
    int y = shape[i][1];
    board[y][x] = shape_type;
  }
  clear_lines();
  new_random_shape();
  return 1;
}

void move_shape_one_down() {
  for (int i = 0; i < 4; i++) {
    shape[i][1]++;
  }
}

void move_shape_down() {
  bool collision = false;
  while (!collision) {
    for (int i = 0; i < 4; i++) {
      shape[i][1]++;
    }
    for (int i = 0; i < 4; i++) {
      if (shape[i][1] == BOARD_HEIGHT - 1 ||
          board[shape[i][1] + 1][shape[i][0]] > 0) {
        collision = true;
      }
    }
  }
}

void move_shape_left() {
  for (int i = 0; i < 4; i++) {
    if (shape[i][0] == 0 || board[shape[i][1]][shape[i][0] - 1] > 0) {
      return;
    }
  }
  for (int i = 0; i < 4; i++) {
    shape[i][0]--;
  }
}

void move_shape_right() {
  for (int i = 0; i < 4; i++) {
    if (shape[i][0] == BOARD_WIDTH - 1 ||
        board[shape[i][1]][shape[i][0] + 1] > 0) {
      return;
    }
  }
  for (int i = 0; i < 4; i++) {
    shape[i][0]++;
  }
}

void new_random_shape() {
  shape_type = rand() % 7 + 1;
  switch (shape_type) {
  // T shape
  case 1:
    shape[0][0] = 5;
    shape[0][1] = 1;
    shape[1][0] = 4;
    shape[1][1] = 1;
    shape[2][0] = 6;
    shape[2][1] = 1;
    shape[3][0] = 5;
    shape[3][1] = 0;
    break;
  // I shape
  case 2:
    shape[0][0] = 4;
    shape[0][1] = 0;
    shape[1][0] = 5;
    shape[1][1] = 0;
    shape[2][0] = 6;
    shape[2][1] = 0;
    shape[3][0] = 7;
    shape[3][1] = 0;
    break;
  // J shape
  case 3:
    shape[0][0] = 5;
    shape[0][1] = 1;
    shape[1][0] = 4;
    shape[1][1] = 1;
    shape[2][0] = 4;
    shape[2][1] = 0;
    shape[3][0] = 6;
    shape[3][1] = 1;
    break;
  // L shape
  case 4:
    shape[0][0] = 5;
    shape[0][1] = 1;
    shape[1][0] = 4;
    shape[1][1] = 1;
    shape[2][0] = 6;
    shape[2][1] = 1;
    shape[3][0] = 6;
    shape[3][1] = 0;
    break;
  // O shape
  case 5:
    shape[0][0] = 5;
    shape[0][1] = 0;
    shape[1][0] = 5;
    shape[1][1] = 1;
    shape[2][0] = 6;
    shape[2][1] = 0;
    shape[3][0] = 6;
    shape[3][1] = 1;
    break;
  // S shape
  case 6:
    shape[0][0] = 5;
    shape[0][1] = 1;
    shape[1][0] = 5;
    shape[1][1] = 0;
    shape[2][0] = 4;
    shape[2][1] = 1;
    shape[3][0] = 6;
    shape[3][1] = 0;
    break;
  // Z shape
  case 7:
    shape[0][0] = 5;
    shape[0][1] = 1;
    shape[1][0] = 5;
    shape[1][1] = 0;
    shape[2][0] = 4;
    shape[2][1] = 0;
    shape[3][0] = 6;
    shape[3][1] = 1;
    break;
  }
}

void draw_board() {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      if (board[i][j] > 0) {
        int shape_type = board[i][j];
        SDL_SetRenderDrawColor(renderer, colors[shape_type][0],
                               colors[shape_type][1], colors[shape_type][2],
                               255);
      } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
      SDL_Rect rect;
      rect.h = BLOCK_SIZE;
      rect.w = BLOCK_SIZE;
      rect.y = BLOCK_SIZE * i;
      rect.x = BLOCK_SIZE * j;
      SDL_RenderFillRect(renderer, &rect);
    }
  }
}

void init() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_SHOWN, &screen,
                              &renderer);
  if (!screen) {
    printf("InitSetup failed to create window");
    exit(1);
  }
  // init board
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = 0;
    }
  }
  // init colors
  // black
  colors[0][0] = 0;
  colors[0][1] = 0;
  colors[0][2] = 0;
  // red
  colors[1][0] = 255;
  colors[1][1] = 0;
  colors[1][2] = 0;
  // green
  colors[2][0] = 0;
  colors[2][1] = 255;
  colors[2][2] = 0;
  // blue
  colors[3][0] = 0;
  colors[3][1] = 0;
  colors[3][2] = 255;
  // cyan
  colors[4][0] = 0;
  colors[4][1] = 255;
  colors[4][2] = 255;
  // orange
  colors[5][0] = 255;
  colors[5][1] = 165;
  colors[5][2] = 0;
  // magenta
  colors[6][0] = 255;
  colors[6][1] = 0;
  colors[6][2] = 255;
  // yellow
  colors[7][0] = 255;
  colors[7][1] = 255;
  colors[7][2] = 0;
}

void finish() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(screen);
  SDL_Quit();
  exit(0);
}

char get_char() {
  int result = -1;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      result = event.key.keysym.sym;
      break;
    }
  }
  return result;
}

void game_loop() {
  int gameRunning = 1;
  int frame = 0;
  while (gameRunning) {
    // uint64_t start = SDL_GetPerformanceCounter();
    if (frame == 30) {
      frame = 0;
      move_shape_one_down();
    }
    render();
    if (shape_collision() == 0) {
      break;
    }
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        gameRunning = 0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          move_shape_left();
          break;
        case SDLK_RIGHT:
          move_shape_right();
          break;
        case SDLK_DOWN:
          move_shape_down();
          break;
        case SDLK_UP:
          rotate_shape();
          break;
        default:
          break;
        }
      case SDL_KEYUP:
        break;
      }
    }
    // uint64_t end = SDL_GetPerformanceCounter();
    // float elapsedMS =
    //     (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    // SDL_Delay(floor(16.666f - elapsedMS));
    SDL_Delay(16);
    frame++;
  }
}
