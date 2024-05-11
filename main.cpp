#include <iostream>
#include <cassert>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

#define CELL_WIDTH ((float)SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float)SCREEN_HEIGHT / BOARD_HEIGHT)

#define AGENTS_COUNT 5

#define BACKGROUND_COLOR "181818"
#define GRID_COLOR "303030"

Uint8 hex_to_dec(char x) {
  if ('0' <= x && x <= '9') return x - '0';
  if ('a' <= x && x <= 'f') return x - 'a' + 10; 
  if ('A' <= x && x <= 'F') return x - 'A' + 10;
  std::cerr << "ERROR: incorrect hex character" << std::endl;
  exit(1);
}


Uint8 parse_hex_byte(const char* byte) {
  return hex_to_dec(*byte) * 0x10 + hex_to_dec(*(byte + 1));
}

void sdl_set_color_hex(SDL_Renderer* renderer, const char* hex) {
  size_t hex_len = strlen(hex);
  assert(hex_len == 6);

  SDL_SetRenderDrawColor(renderer, 
                         parse_hex_byte(hex),
			 parse_hex_byte(hex + 2),
			 parse_hex_byte(hex + 4),
			 255);
}

void draw_grid(SDL_Renderer* renderer) {
  sdl_set_color_hex(renderer, GRID_COLOR);

  for (int col = 1; col < BOARD_WIDTH; ++col) {
    SDL_RenderDrawLine(renderer,
                       col*CELL_WIDTH	, 0,
		       col*CELL_WIDTH, SCREEN_HEIGHT); 
  }

  for (int row = 1; row < BOARD_HEIGHT; ++row) {
    SDL_RenderDrawLine(renderer,
                       0, row*CELL_HEIGHT,
		       SCREEN_WIDTH, row*CELL_HEIGHT); 
  }

}

typedef enum {
  DIR_RIGHT = 0,
  DIR_DOWN,
  DIR_LEFT,
  DIR_UP,
} Direction;

typedef struct {
  int pos_x, pos_y;
  Direction dir;
  int hunger;
  int hp;
} Agent;

typedef enum {
  ACTION_SLEEP = 0,	
  ACTION_STEP,
  ACTION_EAT,
  ACTION_ATTACK,
 
} Action; 


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

void init_agents() {
  for (size_t c = 0; c < AGENTS_COUNT; ++c) {
    agents[c] = random_agent();
  }
}

void draw_single_agent(SDL_Renderer* renderer, Agent agent) {
  assert(0 && "TODO: draw_agent() is not implemented");
  //SDL_SetRenderDrawColor(renderer, )
}

void draw_all_agents(SDL_Renderer* renderer) {
  for (int i = 0; i < AGENTS_COUNT; ++i) {
    draw_single_agent(renderer, agents[i]);
  }

}

int main(int argc, char* argv[]) {
  init_agents();
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
  	std::cerr << "SDL error: " << SDL_GetError() << std::endl;
 	exit(1);
  }

  SDL_Window* const window = SDL_CreateWindow("GAME OF LIFE",
      	                                0, 0,
      					SCREEN_WIDTH, SCREEN_HEIGHT,
      					SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
   	std::cerr << "SDL error: " << SDL_GetError() << std::endl;
 	exit(1);
  }

  SDL_Renderer* const renderer = SDL_CreateRenderer(window, -1,
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
    while (SDL_PollEvent (&event)) {
      switch(event.type) {
        case SDL_QUIT: {
          quit = true;
        } break;
    }

    sdl_set_color_hex(renderer, BACKGROUND_COLOR);

    SDL_RenderClear(renderer);

    draw_grid(renderer);

    SDL_RenderPresent(renderer);

    }
  }

  SDL_Quit();
  return 0;
}

