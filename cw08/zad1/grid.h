#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid();
void* update_grid_field(void* arg);
void create_threads(char* src, char *dst);


#define grid_width 30
#define grid_height 30