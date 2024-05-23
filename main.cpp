#include <iostream>
#include <cassert>
#include <SDL2/SDL.h>
#include <unordered_map>

#include "./style.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

#define CELL_WIDTH ((float)SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float)SCREEN_HEIGHT / BOARD_HEIGHT)

#define AGENTS_COUNT 4
#define FOOD_COUNT 4
#define WALLS_COUNT 4

Uint8 hex_to_dec(char x) {
  if ('0' <= x && x <= '9')
    return x - '0';
  if ('a' <= x && x <= 'f')
    return x - 'a' + 10;
  if ('A' <= x && x <= 'F')
    return x - 'A' + 10;
  std::cerr << "ERROR: incorrect hex character" << std::endl;
  exit(1);
}

Uint8 parse_hex_byte(const char *byte) {
  return hex_to_dec(*byte) * 0x10 + hex_to_dec(*(byte + 1));
}

void sdl_set_color_hex(SDL_Renderer *renderer, const char *hex) {
  size_t hex_len = strlen(hex);
  assert(hex_len == 6);

  SDL_SetRenderDrawColor(renderer,
                         parse_hex_byte(hex),
                         parse_hex_byte(hex + 2),
                         parse_hex_byte(hex + 4),
                         255);
}

void draw_grid(SDL_Renderer *renderer) {
  sdl_set_color_hex(renderer, GRID_COLOR);

  for (int col = 1; col < BOARD_WIDTH; ++col) {
    SDL_RenderDrawLine(renderer,
                       col * CELL_WIDTH, 0,
                       col * CELL_WIDTH, SCREEN_HEIGHT);
  }

  for (int row = 1; row < BOARD_HEIGHT; ++row) {
    SDL_RenderDrawLine(renderer,
                       0, row * CELL_HEIGHT,
                       SCREEN_WIDTH, row * CELL_HEIGHT);
  }

}

typedef enum {
  DIR_RIGHT = 0,
  DIR_DOWN,
  DIR_LEFT,
  DIR_UP,
} Direction;

// FIXME: fix this bowl of shit
float agent_directions[4][2] = {
    // DIR_RIGHT
    {0.7, 0.43}, // {1.0, 0.5},
    // DIR_DOWN
    {0.43, 0.7}, // {0.5, 1.0},
    // DIR_LEFT
    {0.1, 0.43}, // {0.0, 0.5},
    // DIR_UP
    {0.43, 0.1}  // {0.5, 0.0}
};

typedef int State;

typedef enum {
  SEE_NOTHING = 0,
  SEE_AGENT,
  SEE_FOOD,
  SEE_WALL
} Env;

typedef enum {
  ACTION_SLEEP = 0,
  ACTION_STEP,
  ACTION_EAT,
  ACTION_ATTACK,
  ACTION_ROTATE

} Action;

// float is weights
typedef  std::unordered_map<Env, std::unordered_map<Action, float>> BrainCells;
typedef struct {
  BrainCells cells;
  
} Brain;

// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state
// state enviroment action next_state

typedef struct {
  int pos_x, pos_y;
  Direction dir;
  int hunger;
  int hp;
  State state;
} Agent;

typedef struct {
  bool eaten;
  int pos_x;
  int pos_y;
} Food;

typedef struct {
  int pos_x;
  int pos_y;
} Wall;

typedef struct {
  Agent agents[AGENTS_COUNT];
  Food food[FOOD_COUNT];
  Wall walls[WALLS_COUNT];
} Game;

Agent agents[AGENTS_COUNT];

int random_int_range(int from, int to) {
  return rand() % (to - from) + from;
}

Direction random_dir() {
  return (Direction) random_int_range(0, 4);
}

Agent random_agent() {
  Agent agent = {0};
  agent.pos_x = random_int_range(0, BOARD_WIDTH);
  agent.pos_y = random_int_range(0, BOARD_HEIGHT);
  agent.dir = random_dir();
  agent.hunger = 100;
  agent.hp = 100;
  return agent;
}

void draw_single_agent(SDL_Renderer *renderer, Agent agent) {
  sdl_set_color_hex(renderer, AGENT_COLOR);

#define AGENTS_PADDING 20

  SDL_Rect rect = {
      (int) floorf(agent.pos_x * CELL_WIDTH + AGENTS_PADDING),
      (int) floorf(agent.pos_y * CELL_HEIGHT + AGENTS_PADDING),
      (int) floorf(CELL_WIDTH - 2 * AGENTS_PADDING),
      (int) floorf(CELL_HEIGHT - 2 * AGENTS_PADDING),
  };

  SDL_RenderFillRect(renderer, &rect);

  int agents_width = (int) floorf(CELL_WIDTH - 2 * AGENTS_PADDING);
  int agents_height = (int) floorf(CELL_HEIGHT - 2 * AGENTS_PADDING);

  float dir_x = agent_directions[agent.dir][0] * CELL_WIDTH + agent.pos_x * CELL_WIDTH;
  float dir_y = agent_directions[agent.dir][1] * CELL_HEIGHT + agent.pos_y * CELL_HEIGHT;

  SDL_Rect dir_rect = {
      (int) floorf(dir_x),
      (int) floorf(dir_y),
      (int) floorf(agents_width / 3),
      (int) floorf(agents_height / 3),
  };

  SDL_RenderFillRect(renderer, &dir_rect);
}

void draw_game(SDL_Renderer *renderer, const Game *game) {
  for (int i = 0; i < AGENTS_COUNT; ++i) {
    draw_single_agent(renderer,game->agents[i]);
  }

  for (int i = 0; i < FOOD_COUNT; ++i) {
    int padding = 30;
    SDL_Rect rect = {
        (int) floorf((game->food[i].pos_x ) * CELL_WIDTH + padding),
        (int) floorf((game->food[i].pos_y ) * CELL_HEIGHT + padding),
        (int) floorf(CELL_WIDTH - 2 * padding),
        (int) floorf(CELL_HEIGHT - 2 * padding),
    };
    sdl_set_color_hex(renderer, FOOD_COLOR);
    SDL_RenderFillRect(renderer, &rect);

  }

  for (int i = 0; i < WALLS_COUNT; ++i) {
    SDL_Rect rect = {
        (int) floorf(game->walls[i].pos_x * CELL_WIDTH),
        (int) floorf(game->walls[i].pos_y * CELL_HEIGHT),
        (int) floorf(CELL_WIDTH),
        (int) floorf(CELL_HEIGHT),
    };
    sdl_set_color_hex(renderer, WALL_COLOR);
    SDL_RenderFillRect(renderer, &rect);

  }
}

void init_game(Game *game) {
  for (size_t c = 0; c < AGENTS_COUNT; ++c) {
    game->agents[c] = random_agent();
  }

  for (size_t c = 0; c < FOOD_COUNT; ++c) {
    game->food[c].pos_x = random_int_range(0, BOARD_WIDTH);
    game->food[c].pos_y = random_int_range(0, BOARD_HEIGHT);
  }

  for (size_t c = 0; c < WALLS_COUNT; ++c) {
    game->walls[c].pos_x = random_int_range(0, BOARD_WIDTH);
    game->walls[c].pos_y = random_int_range(0, BOARD_HEIGHT);
  }
}

void step_game(Game *game) {
//TODO steping game is not implemented
}

Game game = {0};

int main(int argc, char *argv[]) {
  init_game(&game);
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL error: " << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_Window *const window = SDL_CreateWindow("GAME OF LIFE",
                                              0, 0,
                                              SCREEN_WIDTH, SCREEN_HEIGHT,
                                              SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    std::cerr << "SDL error: " << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_Renderer *const renderer = SDL_CreateRenderer(window, -1,
                                                    SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "SDL error: " << SDL_GetError() << std::endl;
    exit(1);
  }

  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  bool quit = false;
  while (!quit) {

    // Start handling events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        quit = true;
      }
        break;

      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
        case SDLK_SPACE: {
          step_game(&game);
        }
          break;
        }
      }
        break;
      }

      sdl_set_color_hex(renderer, BACKGROUND_COLOR);

      SDL_RenderClear(renderer);

      draw_grid(renderer);
      draw_game(renderer, &game);

      SDL_RenderPresent(renderer);

    }
  }

  SDL_Quit();
  return 0;
}