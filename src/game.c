#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

druid_game *new_druid_game(int size) {
    druid_game *new_game;

    if (size < 1 || size > 26) {
        fprintf(stderr, "Illegal board size %d -- must be between 1 and 26",
                        size);
        exit(EXIT_FAILURE);
    }

    new_game = malloc(sizeof (druid_game));
    new_game->size = size;
    new_game->player_on_turn = VERTICAL;
    new_game->colors = calloc((size_t)size * size, sizeof (int));
    new_game->heights = calloc((size_t)size * size, sizeof (int));
    new_game->previous_move = ILLEGAL;
    new_game->finished = 0;
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
    int pos = parse_coord(game, coord, '\0');
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
    int pos = parse_coord(game, coord, '\0');
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

void _set_previous_move(druid_game *game, int row, int col) {
    game->previous_move = row * game->size + col;
}

/* Bounds checking not made. Returns MOVE_MADE or INVALID_MOVE. */
int _make_sarsen_move(druid_game *game, int row, int col) {
    int present_color = _color_at(game, row, col);
    if (present_color != 0 && present_color != game->player_on_turn)
        return INVALID_MOVE;

    _set_color_at(game, row, col, game->player_on_turn);
    _increase_height_at(game, row, col);
    _switch_player_on_turn(game);
    _set_previous_move(game, row, col);

    return MOVE_MADE;
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
    _set_previous_move(game, row, col);

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
    _set_previous_move(game, row, col);

    return MOVE_MADE;
}

/* Coordinates are bounds-checked. Returns MOVE_MADE or INVALID_MOVE. */
int make_move(druid_game *game, char *move) {
    int pos1, pos2, row1, row2, col1, col2, size = game->size;

    if (game->finished)
        return INVALID_MOVE;

    pos1 = parse_coord(game, move, '\0');
    if (pos1 != ILLEGAL) {
        int result = _make_sarsen_move(game, pos1 / size, pos1 % size);
        if (result == MOVE_MADE && who_won(game) != NONE) {
            game->finished = 1;
        }
        return result;
    }

    pos1 = parse_coord(game, move, '-');
    if (pos1 == ILLEGAL)
        return INVALID_MOVE;
    move = strchr(move, '-') + 1;
    pos2 = parse_coord(game, move, '\0');
    if (pos2 == ILLEGAL)
        return INVALID_MOVE;

    row1 = pos1 / size;
    row2 = pos2 / size;
    if (row1 > row2) {
        int temp = row1;
        row1 = row2;
        row2 = temp;
    }

    col1 = pos1 % size;
    col2 = pos2 % size;
    if (col1 > col2) {
        int temp = col1;
        col1 = col2;
        col2 = temp;
    }

    if (row1 < 0 || row2 >= size)
        return INVALID_MOVE;
    if (col2 >= size)
        return INVALID_MOVE;

    {
        int result;
        if (row1 == row2 && col2 == col1 + 2) {
            result = _make_hlintel_move(game, row1, col1);
        }
        else if (col1 == col2 && row2 == row1 + 2) {
            result = _make_vlintel_move(game, row1, col1);
        }
        else {
            return INVALID_MOVE;
        }
        if (result == MOVE_MADE && who_won(game) != NONE) {
            game->finished = 1;
        }
        return result;
    }
}

int who_won(druid_game *game) {
    int size = game->size, *visited, *queue, current = 0, end = 1,
        previous_move = game->previous_move,
        color = _color_at(game, previous_move / size, previous_move % size),
        touched_one_side = 0, touched_other_side = 0;

    if (game->previous_move == ILLEGAL)
        return NONE;

    visited = calloc((size_t)size * size, sizeof (int));
    queue = malloc(sizeof (int) * size * size);
    visited[previous_move] = 1;
    queue[current] = previous_move;
    for(; current < end; ++current) {
        int row = queue[current] / size, col = queue[current] % size,
            direction;

        if (color == VERTICAL && row == 0)
            ++touched_one_side;
        if (color == HORIZONTAL && col == 0)
            ++touched_one_side;
        if (color == VERTICAL && row == size - 1)
            ++touched_other_side;
        if (color == HORIZONTAL && col == size - 1)
            ++touched_other_side;
        if (touched_one_side && touched_other_side)
            break;

        for (direction = 0; direction < 4; ++direction) {
            int nrow = row, ncol = col, pos;
            switch (direction) {
                case 0: if (row == 0)        continue; nrow--; break;
                case 1: if (col == size - 1) continue; ncol++; break;
                case 2: if (row == size - 1) continue; nrow++; break;
                case 3: if (col == 0)        continue; ncol--; break;
            }

            pos = nrow * size + ncol;
            if (!visited[pos]++ && _color_at(game, nrow, ncol) == color)
                queue[end++] = pos;
        }
    }

    free(visited);
    free(queue);

    return touched_one_side && touched_other_side ? color : NONE;
}
