#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

enum color {
    ILLEGAL = -1,
    EMPTY,
    VERTICAL,
    HORIZONTAL
};

enum move_result {
    INVALID_MOVE,
    MOVE_MADE
};

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

/* Coordinates are bounds-checked. */
int parse_coord(druid_game *game, char *coord, char expected_stopper) {
    int row, col, size;

    if (*coord >= 'A' && *coord <= 'Z') {
        col = *coord - 'A';
    }
    else if (*coord >= 'a' && *coord <= 'z') {
        col = *coord - 'a';
    }
    else {
        return ILLEGAL;
    }
    ++coord;

    if (*coord < '0' || *coord > '9') {
        return ILLEGAL;
    }
    row = *coord - '0';
    coord++;
    if (*coord >= '0' && *coord <= '9') {
        row *= 10;
        row += *coord - '0';
        coord++;
    }
    row--;

    if (*coord != expected_stopper) {
        return ILLEGAL;
    }

    size = game->size;
    if (row < 0 || row >= size)
        return ILLEGAL;
    if (col >= size)
        return ILLEGAL;

    return row * size + col;
}

/* Coordinates are bounds-checked. */
int color_at(druid_game *game, char *coord) {
    int pos;

    pos = parse_coord(game, coord, '\0');
    if (pos == ILLEGAL) {
        return ILLEGAL;
    }

    return game->colors[pos];
}

/* Bounds checking not made. */
int _color_at(druid_game *game, int row, int col) {
    return game->colors[row * game->size + col];
}

/* Coordinates are bounds-checked. */
int height_at(druid_game *game, char *coord) {
    int pos;

    pos = parse_coord(game, coord, '\0');
    if (pos == ILLEGAL) {
        return ILLEGAL;
    }

    return game->heights[pos];
}

/* Bounds checking not made. */
int _height_at(druid_game *game, int row, int col) {
    return game->heights[row * game->size + col];
}

/* Bounds checking not made. */
void _set_color_at(druid_game *game, int row, int col, int new_color) {
    game->colors[row * game->size + col] = new_color;
}

void _set_height_at(druid_game *game, int row, int col, int new_height) {
    game->heights[row * game->size + col] = new_height;
}

void _increase_height_at(druid_game *game, int row, int col) {
    game->heights[row * game->size + col]++;
}

void _switch_player_on_turn(druid_game *game) {
    game->player_on_turn
        = game->player_on_turn == VERTICAL
            ? HORIZONTAL
            : VERTICAL;
}

/* Bounds checking not made. Returns MOVE_MADE or INVALID_MOVE. */
int _make_sarsen_move(druid_game *game, int row, int col) {
    int present_color;

    present_color = _color_at(game, row, col);
    if (present_color != 0 && present_color != game->player_on_turn)
        return INVALID_MOVE;

    _set_color_at(game, row, col, game->player_on_turn);
    _increase_height_at(game, row, col);
    _switch_player_on_turn(game);

    return MOVE_MADE;
}

/* Coordinates are bounds-checked. Returns MOVE_MADE or INVALID_MOVE. */
int make_sarsen_move(druid_game *game, char *coord) {
    int pos, size;

    pos = parse_coord(game, coord, '\0');
    if (pos == ILLEGAL)
        return INVALID_MOVE;

    size = game->size;
    return _make_sarsen_move(game, pos / size, pos % size);
}

/* Bounds checking not made. The variable 'col' refers to the left end of
   the lintel. */
int _make_hlintel_move(druid_game *game, int row, int col) {
    int player_support, new_height, new_color, i;

    if (_height_at(game, row, col) != _height_at(game, row, col + 2))
        return INVALID_MOVE;
    if (_height_at(game, row, col + 1) > _height_at(game, row, col))
        return INVALID_MOVE;

    player_support = 0;
    if (_color_at(game, row, col) == game->player_on_turn)
        player_support++;
    if (_color_at(game, row, col + 2) == game->player_on_turn)
        player_support++;
    if (_color_at(game, row, col + 1) == game->player_on_turn
        && _height_at(game, row, col + 1) == _height_at(game, row, col))
        player_support++;
    if (player_support != 2)
        return INVALID_MOVE;

    new_height = _height_at(game, row, col) + 1;
    new_color = game->player_on_turn;
    for (i = 0; i <= 2; ++i) {
        _set_height_at(game, row, col + i, new_height);
        _set_color_at(game, row, col + i, new_color);
    }
    _switch_player_on_turn(game);

    return MOVE_MADE;
}

/* Bounds checking not made. The variable 'row' refers to the bottom end of
   the lintel. */
int _make_vlintel_move(druid_game *game, int row, int col) {
    int player_support, new_height, new_color, i;

    if (_height_at(game, row, col) != _height_at(game, row + 2, col))
        return INVALID_MOVE;
    if (_height_at(game, row + 1, col) > _height_at(game, row, col))
        return INVALID_MOVE;

    player_support = 0;
    if (_color_at(game, row, col) == game->player_on_turn)
        player_support++;
    if (_color_at(game, row + 2, col) == game->player_on_turn)
        player_support++;
    if (_color_at(game, row + 1, col) == game->player_on_turn
        && _height_at(game, row + 1, col) == _height_at(game, row, col))
        player_support++;
    if (player_support != 2)
        return INVALID_MOVE;

    new_height = _height_at(game, row, col) + 1;
    new_color = game->player_on_turn;
    for (i = 0; i <= 2; ++i) {
        _set_height_at(game, row + i, col, new_height);
        _set_color_at(game, row + i, col, new_color);
    }
    _switch_player_on_turn(game);

    return MOVE_MADE;
}

/* Coordinates are bounds-checked. Returns MOVE_MADE or INVALID_MOVE. */
int make_lintel_move(druid_game *game, char *coord) {
    int pos1, pos2, row1, row2, temp, col1, col2, size;

    pos1 = parse_coord(game, coord, '-');
    if (pos1 == ILLEGAL)
        return INVALID_MOVE;
    coord = strchr(coord, '-') + 1;
    pos2 = parse_coord(game, coord, '\0');
    if (pos2 == ILLEGAL)
        return INVALID_MOVE;

    size = game->size;
    row1 = pos1 / size;
    row2 = pos2 / size;
    if (row1 > row2) {
        temp = row1;
        row1 = row2;
        row2 = temp;
    }

    col1 = pos1 % size;
    col2 = pos2 % size;
    if (col1 > col2) {
        temp = col1;
        col1 = col2;
        col2 = temp;
    }

    if (row1 < 0 || row2 >= size)
        return INVALID_MOVE;
    if (col2 >= size)
        return INVALID_MOVE;

    if (row1 == row2 && col2 == col1 + 2)
        return _make_hlintel_move(game, row1, col1);
    else if (col1 == col2 && row2 == row1 + 2)
        return _make_vlintel_move(game, row1, col1);
    else
        return INVALID_MOVE;
}

int main() {
    druid_game *game;

    game = new_druid_game(4);

    is(game->size, 4, "game initialized with the right size");
    is(game->player_on_turn, VERTICAL, "vertical starts");
    is(color_at(game, "b2"), EMPTY, "the board is empty at the start");
    is(height_at(game, "b2"), 0, "the board is flat at the start");

    is(make_sarsen_move(game, "b2"), MOVE_MADE, "sarsen move works");
    is(color_at(game, "b2"), VERTICAL, "color changed at the position");
    is(height_at(game, "b2"), 1, "height has increased by one");
    is(game->player_on_turn, HORIZONTAL, "it is now horizontal's turn");

    is(make_sarsen_move(game, "b2"), INVALID_MOVE, "can't put sarsen there");
    is(make_lintel_move(game, "a2-c2"), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");
    is(make_lintel_move(game, "b2-d2"), INVALID_MOVE,
        "lintel can't rest on a single sarsen at one end");
    is(make_lintel_move(game, "a1-c1"), INVALID_MOVE,
        "can't put lintel on ground");

    make_sarsen_move(game, "b1");
    make_sarsen_move(game, "b4");

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . . . . 3
       2 . V . . 2
       1 . H . . 1
         A B C D   */

    is(make_lintel_move(game, "b1-b3"), INVALID_MOVE,
        "lintel can't stick out in the air");
    is(make_lintel_move(game, "b2-b4"), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_sarsen_move(game, "d1");
    is(make_lintel_move(game, "b1-d1"), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_sarsen_move(game, "c3");
    is(make_lintel_move(game, "b1-d1"), MOVE_MADE, "lintel on two pieces");
    is(height_at(game, "b1"), 2, "lintel gets us to height 2 (a)");
    is(height_at(game, "c1"), 2, "lintel gets us to height 2 (b)");
    is(height_at(game, "d1"), 2, "lintel gets us to height 2 (c)");
    is(color_at(game, "c1"), HORIZONTAL, "lintel is placed");

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . . V . 3
       2 . V . . 2
       1 . H H H 1
         A B C D   */

    is(make_lintel_move(game, "c2-c4"), INVALID_MOVE,
        "lintel can't lie on single sarsen in the middle");
    is(make_lintel_move(game, "b2-b4"), MOVE_MADE, "lintel on two pieces");

    make_sarsen_move(game, "c1");
    is(make_lintel_move(game, "b1-b3"), MOVE_MADE,
        "lintel on one opponent piece");

    make_sarsen_move(game, "d1");
    make_sarsen_move(game, "b2");

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . V V . 3
       2 . V . . 2
       1 . V H H 1
         A B C D   */

    is(make_lintel_move(game, "b1-d1"), MOVE_MADE,
        "lintel on one opponent piece");

    destroy_druid_game(game);
    game = new_druid_game(4);

    make_sarsen_move(game, "a1");   make_sarsen_move(game, "d2");
    make_sarsen_move(game, "a1");   make_sarsen_move(game, "d2");
    make_sarsen_move(game, "b1");   make_sarsen_move(game, "d3");
    make_sarsen_move(game, "c1");   make_sarsen_move(game, "d4");
    make_sarsen_move(game, "c1");   make_sarsen_move(game, "d4");

    is(make_lintel_move(game, "a1-c1"), MOVE_MADE,
        "three same-color pieces, but only two supporting (horizontal)");

    is(make_lintel_move(game, "d2-d4"), MOVE_MADE,
        "three same-color pieces, but only two supporting (vertical)");
    is(make_lintel_move(game, "a1-c1"), INVALID_MOVE,
        "three same-color pieces (horizontal)");
    make_sarsen_move(game, "a2");
    is(make_lintel_move(game, "d2-d4"), INVALID_MOVE,
        "three same-color pieces (vertical)");

    make_sarsen_move(game, "a3");
    make_sarsen_move(game, "a4");
    make_sarsen_move(game, "a3");
    is(make_lintel_move(game, "a2-a4"), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");

    destroy_druid_game(game);

    return 0;
}
