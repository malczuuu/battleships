#include "core.h"
#include "board.h"

coords_t coords_create(int x, int y) {
    coords_t coords;
    coords_init(&coords, x, y);
    return coords;
}

void coords_init(coords_t *c, int x, int y) {
    c->x = x;
    c->y = y;
}

void board_init(board_t *b) {
    int i, j;
    b->fields = (int **) malloc(BOARD_MEM_SIZE * sizeof(int *));
    for (i = 0; i < BOARD_MEM_SIZE; ++i) {
        b->fields[i] = (int *) malloc(BOARD_MEM_SIZE * sizeof(int));
        for (j = 0; j < BOARD_MEM_SIZE; ++j) {
            b->fields[i][j] = 0;
        }
    }
}

void board_destroy(board_t *b) {
    int i;
    for (i = 0; i < BOARD_MEM_SIZE; ++i) {
        free(b->fields[i]);
        b->fields[i] = NULL;
    }
    free(b->fields);
    b->fields = NULL;
}

static bool check_square(const board_t *b, coords_t coords) {
    return b->fields[coords.x - 1][coords.y - 1] == 0 &&
           b->fields[coords.x - 1][coords.y] == 0 &&
           b->fields[coords.x - 1][coords.y + 1] == 0 &&
           b->fields[coords.x][coords.y - 1] == 0 &&
           b->fields[coords.x][coords.y] == 0 &&
           b->fields[coords.x][coords.y + 1] == 0 &&
           b->fields[coords.x + 1][coords.y - 1] == 0 &&
           b->fields[coords.x + 1][coords.y] == 0 &&
           b->fields[coords.x + 1][coords.y + 1] == 0;
}

static coords_t next_coords(bool vertical, coords_t coords, int offset) {
    if (vertical) {
        return coords_create(coords.x + offset - 1, coords.y);
    } else {
        return coords_create(coords.x, coords.y + offset - 1);
    }
}

static void board_find_location_for(const board_t *b, int size, int ship_id) {
    int i = 0;
    bool vertical = false;
    bool is_empty = false;
    coords_t coords;
    while (!is_empty) {
        vertical = randbool();
        // +1 because array is numerated from 1 (additional border rows and cols with default 0 value)
        coords.x = abs(rand()) % (BOARD_SIZE - size + 1) + 1;
        coords.y = abs(rand()) % (BOARD_SIZE - size + 1) + 1;


        int empty_spots = 0;
        for (i = 1; i < size + 1; ++i) {
            coords_t current = next_coords(vertical, coords, i);
            if (check_square(b, current)) {
                ++empty_spots;
            }
        }
        if (empty_spots == size) {
            is_empty = true;
        }
    }

    for (i = 0; i < size; ++i) {
        if (vertical) {
            b->fields[coords.x + i][coords.y] = ship_id;
        } else {
            b->fields[coords.x][coords.y + i] = ship_id;
        }
    }
}

void board_randomize(const board_t *b) {
    int curr_id = 1;

    board_find_location_for(b, 4, curr_id++);
    board_find_location_for(b, 3, curr_id++);
    board_find_location_for(b, 3, curr_id++);
    board_find_location_for(b, 2, curr_id++);
    board_find_location_for(b, 2, curr_id++);
    board_find_location_for(b, 2, curr_id++);
    board_find_location_for(b, 1, curr_id++);
    board_find_location_for(b, 1, curr_id++);
    board_find_location_for(b, 1, curr_id++);
    board_find_location_for(b, 1, curr_id + 1);
}

int board_fire(const board_t *board, coords_t coords) {
    if (board->fields[coords.x][coords.y] < 0 || board->fields[coords.x][coords.y] / 100 != 0) {
        return STRIKE_REPEATED;
    } else if (board->fields[coords.x][coords.y] != 0 && board->fields[coords.x][coords.y] / 100 == 0) {
        board->fields[coords.x][coords.y] *= 100;
        return STRIKE_HIT;
    } else {
        board->fields[coords.x][coords.y] = -1;
        return STRIKE_MISS;
    }
}

bool board_alive(const board_t *board) {
    int i;
    int j;
    bool alive = false;
    for (i = 1; i <= BOARD_SIZE; ++i) {
        for (j = 1; j <= BOARD_SIZE; ++j) {
            if (board->fields[i][j] != 0 && board->fields[i][j] / 100 == 0) {
                alive = true;
                break;
            }
        }
    }
    return alive;
}

coords_t board_random_hit(const board_t *board) {
    coords_t coords = coords_create(abs(rand()) % BOARD_SIZE + 1, abs(rand()) % BOARD_SIZE + 1);

    while (board->fields[coords.x][coords.y] < 0 ||
           (board->fields[coords.x][coords.y] > 0 && board->fields[coords.x][coords.y] / 100 != 0)) {
        coords = coords_create(abs(rand()) % BOARD_SIZE + 1, abs(rand()) % BOARD_SIZE + 1);
    }

    return coords;
}


int board_coords_status(const board_t *board, coords_t coords) {
    if (board->fields[coords.x][coords.y] < 0) {
        return COORD_MISSED;
    } else if (board->fields[coords.x][coords.y] > 0 && board->fields[coords.x][coords.y] / 100 == 0) {
        return COORD_ALIVE;
    } else if (board->fields[coords.x][coords.y] > 0 && board->fields[coords.x][coords.y] / 100 != 0) {
        return COORD_DAMAGED;
    }
    return COORD_CLEAN;
}

int board_coords_status_hidden(const board_t *board, coords_t coords, bool hidden) {
    int status = board_coords_status(board, coords);
    if (hidden && status == COORD_ALIVE) {
        return COORD_CLEAN;
    }
    return status;
}
