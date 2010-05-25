#include <stdio.h>
#include <stdlib.h>

int tests_run = 0;

/* Test functions */

void ok(int passed, char *message) {
    if (!passed) {
        printf("not ");
    }
    printf("ok %d - %s\n", ++tests_run, message);
}

void is(int actual, int expected, char *message) {
    int passed;

    passed = (actual == expected);
    ok(passed, message);
    if (!passed) {
        printf("# Got %d\n# Expected %d\n", actual, expected);
    }
}

/* Druid game machinery. This should eventually be moved to another file. */

typedef struct {
    int size;
    int player_on_turn;
    int *colors;
    int *heights;
} druid_game;

const int ILLEGAL    = -1,
          EMPTY      = 0,
          VERTICAL   = 1,
          HORIZONTAL = 2;

druid_game *new_druid_game(int size) {
    druid_game *new_game;

    new_game = malloc(sizeof (druid_game));
    new_game->size = size;
    new_game->player_on_turn = VERTICAL;
    new_game->colors = calloc(size * size, sizeof (int));
    new_game->heights = calloc(size * size, sizeof (int));
    return new_game;
}

/* Coordinates are zero-based and bounds-checked. */
int color_at(druid_game *game, int row, int col) {
    int size;

    size = game->size;
    if (row < 0 || row >= size)
        return ILLEGAL;
    if (col < 0 || col >= size)
        return ILLEGAL;

    return game->colors[row * size + col];
}

int height_at(druid_game *game, int row, int col) {
    int size;

    size = game->size;
    if (row < 0 || row >= size)
        return ILLEGAL;
    if (col < 0 || col >= size)
        return ILLEGAL;

    return game->heights[row * size + col];
}

int main() {
    druid_game *game;

    game = new_druid_game(3);
    is(game->size, 3, "game initialized with the right size");
    is(game->player_on_turn, VERTICAL, "vertical starts");
    is(color_at(game, 1, 1), EMPTY, "the board is empty at the start");
    is(height_at(game, 1, 1), 0, "the board is flat at the start");
    return 0;
}
