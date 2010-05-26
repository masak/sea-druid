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

const int INVALID_MOVE = 0,
          MOVE_MADE    = 1;

druid_game *new_druid_game(int size) {
    druid_game *new_game;

    if (size < 1 || size > 26) {
        fprintf(stderr, "Illegal board size %d -- must be between 1 and 26",
                        size);
        exit(1);
    }

    new_game = malloc(sizeof (druid_game));
    new_game->size = size;
    new_game->player_on_turn = VERTICAL;
    new_game->colors = calloc(size * size, sizeof (int));
    new_game->heights = calloc(size * size, sizeof (int));
    return new_game;
}

void destroy_druid_game(druid_game *game) {
    free(game->colors);
    free(game->heights);
    free(game);
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

/* Bounds checking not made. */
void set_color_at(druid_game *game, int row, int col, int new_color) {
    game->colors[row * game->size + col] = new_color;
}

void set_height_at(druid_game *game, int row, int col, int new_height) {
    game->heights[row * game->size + col] = new_height;
}

void increase_height_at(druid_game *game, int row, int col) {
    game->heights[row * game->size + col]++;
}

void switch_player_on_turn(druid_game *game) {
    game->player_on_turn
        = game->player_on_turn == VERTICAL
            ? HORIZONTAL
            : VERTICAL;
}

/* Bounds checking not made. Returns MOVE_MADE or INVALID_MOVE. */
int make_sarsen_move(druid_game *game, int row, int col) {
    int color;

    color = color_at(game, row, col);
    if (color != 0 && color != game->player_on_turn)
        return INVALID_MOVE;

    set_color_at(game, row, col, game->player_on_turn);
    increase_height_at(game, row, col);
    switch_player_on_turn(game);

    return MOVE_MADE;
}

/* Bounds checking not made. The variable 'col' refers to the left end of
   the lintel. */
int make_hlintel_move(druid_game *game, int row, int col) {
    int player_support, new_height, new_color, i;

    if (height_at(game, row, col) != height_at(game, row, col + 2))
        return INVALID_MOVE;
    if (height_at(game, row, col + 1) > height_at(game, row, col))
        return INVALID_MOVE;

    player_support = 0;
    if (color_at(game, row, col) == game->player_on_turn)
        player_support++;
    if (color_at(game, row, col + 2) == game->player_on_turn)
        player_support++;
    if (color_at(game, row, col + 1) == game->player_on_turn
        && height_at(game, row, col + 1) == height_at(game, row, col))
        player_support++;
    if (player_support != 2)
        return INVALID_MOVE;

    new_height = height_at(game, row, col) + 1;
    new_color = game->player_on_turn;
    for (i = 0; i <= 2; ++i) {
        set_height_at(game, row, col + i, new_height);
        set_color_at(game, row, col + i, new_color);
    }
    switch_player_on_turn(game);

    return MOVE_MADE;
}

/* Bounds checking not made. The variable 'row' refers to the bottom end of
   the lintel. */
int make_vlintel_move(druid_game *game, int row, int col) {
    int player_support, new_height, new_color, i;

    if (height_at(game, row, col) != height_at(game, row + 2, col))
        return INVALID_MOVE;
    if (height_at(game, row + 1, col) > height_at(game, row, col))
        return INVALID_MOVE;

    player_support = 0;
    if (color_at(game, row, col) == game->player_on_turn)
        player_support++;
    if (color_at(game, row + 2, col) == game->player_on_turn)
        player_support++;
    if (color_at(game, row + 1, col) == game->player_on_turn
        && height_at(game, row + 1, col) == height_at(game, row, col))
        player_support++;
    if (player_support != 2)
        return INVALID_MOVE;

    new_height = height_at(game, row, col) + 1;
    new_color = game->player_on_turn;
    for (i = 0; i <= 2; ++i) {
        set_height_at(game, row + i, col, new_height);
        set_color_at(game, row + i, col, new_color);
    }
    switch_player_on_turn(game);

    return MOVE_MADE;
}

int main() {
    druid_game *game;

    game = new_druid_game(4);

    is(game->size, 4, "game initialized with the right size");
    is(game->player_on_turn, VERTICAL, "vertical starts");
    is(color_at(game, 1, 1), EMPTY, "the board is empty at the start");
    is(height_at(game, 1, 1), 0, "the board is flat at the start");

    is(make_sarsen_move(game, 1, 1), MOVE_MADE, "sarsen move works");
    is(color_at(game, 1, 1), VERTICAL, "color changed at the position");
    is(height_at(game, 1, 1), 1, "height has increased by one");
    is(game->player_on_turn, HORIZONTAL, "it is now horizontal's turn");

    is(make_sarsen_move(game, 1, 1), INVALID_MOVE, "can't put sarsen there");
    is(make_hlintel_move(game, 1, 0), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");
    is(make_hlintel_move(game, 1, 1), INVALID_MOVE,
        "lintel can't rest on a single sarsen at one end");
    is(make_hlintel_move(game, 0, 0), INVALID_MOVE,
        "can't put lintel on ground");

    make_sarsen_move(game, 0, 1);
    make_sarsen_move(game, 3, 1);

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . . . . 3
       2 . V . . 2
       1 . H . . 1
         A B C D   */

    is(make_vlintel_move(game, 0, 1), INVALID_MOVE,
        "lintel can't stick out in the air");
    is(make_vlintel_move(game, 1, 1), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_sarsen_move(game, 0, 3);
    is(make_hlintel_move(game, 0, 1), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_sarsen_move(game, 2, 2);
    is(make_hlintel_move(game, 0, 1), MOVE_MADE, "lintel on two pieces");
    is(height_at(game, 0, 1), 2, "lintel gets us to height 2 (a)");
    is(height_at(game, 0, 2), 2, "lintel gets us to height 2 (b)");
    is(height_at(game, 0, 3), 2, "lintel gets us to height 2 (c)");
    is(color_at(game, 0, 2), HORIZONTAL, "lintel is placed");

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . . V . 3
       2 . V . . 2
       1 . H H H 1
         A B C D   */

    is(make_vlintel_move(game, 1, 2), INVALID_MOVE,
        "lintel can't lie on single sarsen in the middle");
    is(make_vlintel_move(game, 1, 1), MOVE_MADE, "lintel on two pieces");

    make_sarsen_move(game, 0, 2);
    is(make_vlintel_move(game, 0, 1), MOVE_MADE,
        "lintel on one opponent piece");

    make_sarsen_move(game, 0, 3);
    make_sarsen_move(game, 1, 1);

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . V V . 3
       2 . V . . 2
       1 . V H H 1
         A B C D   */

    is(make_hlintel_move(game, 0, 1), MOVE_MADE,
        "lintel on one opponent piece");

    destroy_druid_game(game);
    game = new_druid_game(4);

    make_sarsen_move(game, 0, 0);   make_sarsen_move(game, 1, 3);
    make_sarsen_move(game, 0, 0);   make_sarsen_move(game, 1, 3);
    make_sarsen_move(game, 0, 1);   make_sarsen_move(game, 2, 3);
    make_sarsen_move(game, 0, 2);   make_sarsen_move(game, 3, 3);
    make_sarsen_move(game, 0, 2);   make_sarsen_move(game, 3, 3);

    is(make_hlintel_move(game, 0, 0), MOVE_MADE,
        "three same-color pieces, but only two supporting (horizontal)");

    is(make_vlintel_move(game, 1, 3), MOVE_MADE,
        "three same-color pieces, but only two supporting (vertical)");
    is(make_hlintel_move(game, 0, 0), INVALID_MOVE,
        "three same-color pieces (horizontal)");
    make_sarsen_move(game, 1, 0);
    is(make_vlintel_move(game, 1, 3), INVALID_MOVE,
        "three same-color pieces (vertical)");

    make_sarsen_move(game, 2, 0);
    make_sarsen_move(game, 3, 0);
    make_sarsen_move(game, 2, 0);
    is(make_vlintel_move(game, 1, 0), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");

    destroy_druid_game(game);

    return 0;
}
