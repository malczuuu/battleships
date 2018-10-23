#ifndef BSHIPS_BOARD_H
#define BSHIPS_BOARD_H

#include "core.h"

#define BOARD_SIZE     10
#define BOARD_MEM_SIZE (BOARD_SIZE + 2)

#define STRIKE_HIT      101
#define STRIKE_SINK     102
#define STRIKE_REPEATED 103
#define STRIKE_MISS     104
#define COORD_CLEAN     105
#define COORD_DAMAGED   106
#define COORD_MISSED    107
#define COORD_ALIVE     108

typedef struct {
    int x;
    int y;
} coords_t;

coords_t coords_create(int x, int y);

void coords_init(coords_t *c, int x, int y);

typedef struct {
    int **fields;
} board_t;

void board_init(board_t *b);

void board_destroy(board_t *b);

void board_randomize(const board_t *b);

int board_fire(const board_t *board, coords_t coords);

bool board_alive(const board_t *board);

coords_t board_random_hit(const board_t *board);

int board_coords_status(const board_t *board, coords_t coords);

int board_coords_status_hidden(const board_t *board, coords_t coords, bool hidden);

#endif // BSHIPS_BOARD_H
