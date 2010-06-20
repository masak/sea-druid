#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "autoplay.h"

/* Assumes that row and col are within bounds. */
char *_coords_to_sarsen_move(druid_game *game, int row, int col) {
    char *move = malloc(sizeof(char) * 4);

    move[0] = 'a' + col;
    if (row >= 10) {
        move[1] = '0' + (row + 1) / 10;
        move[2] = '0' + (row + 1) % 10;
        move[3] = '\0';
    }
    else {
        move[1] = '0' + (row + 1);
        move[2] = '\0';
    }

    return move;
}

int opponent_of(int player) {
    return player == HORIZONTAL ? VERTICAL : HORIZONTAL;
}

int pi_row_has_opponent_pieces(druid_game *game, int color, int row) {
    int i, size = game->size;
    if (color == VERTICAL) {
        for (i = 0; i < size; ++i) {
            if (_color_at(game, i, row) == HORIZONTAL) {
                return 1;
            }
        }
    }
    else { /* color == HORIZONTAL */
        for (i = 0; i < size; ++i) {
            if (_color_at(game, row, i) == VERTICAL) {
                return 1;
            }
        }
    }
    return 0;
}

char *pi_coords_to_sarsen_move(druid_game *game, int color,
                               int row, int col) {
    return color == VERTICAL
        ? _coords_to_sarsen_move(game, col, row)
        : _coords_to_sarsen_move(game, row, col);
}

alpha_0_player *initialize_alpha_0_player(druid_game *game, int color) {
    alpha_0_player *player = malloc(sizeof(alpha_0_player));
    player->game = game;
    player->color = color;

    return player;
}

char *make_move_alpha_0(alpha_0_player *player) {
    druid_game *game = player->game;
    int size = game->size, opponent_color = opponent_of(player->color);
    int row, col;

    do {
        row = rand() % size;
        col = rand() % size;
    } while (_color_at(game, row, col) == opponent_color);

    return _coords_to_sarsen_move(game, row, col);
}

alpha_1_player *initialize_alpha_1_player(druid_game *game, int color) {
    alpha_1_player *player = malloc(sizeof(alpha_1_player));
    player->game = game;
    player->algorithm = ALPHA_1;
    player->color = color;
    player->current_row = ILLEGAL;

    return player;
}

char *make_move_alpha_1(alpha_1_player *player) {
    druid_game *game = player->game;
    int row = player->current_row, color = player->color, size = game->size;

    if (player->algorithm == ALPHA_1
        && (row == ILLEGAL || pi_row_has_opponent_pieces(game, color, row))) {

        int free_row_exists = 0;
        for (row = 0; row < size; ++row) {
            if (!pi_row_has_opponent_pieces(game, color, row)) {
                free_row_exists = 1;
                break;
            }
        }

        if (free_row_exists) {
            do {
                row = rand() % size;
            } while (pi_row_has_opponent_pieces(game, color, row));
            player->current_row = row;
            player->min_col = player->max_col = rand() % size;
            return pi_coords_to_sarsen_move(game, color, row, player->min_col);
        }
        else {
            player->algorithm = ALPHA_0;
        }
    }

    if (player->algorithm == ALPHA_1) {
        if (size - player->max_col > player->min_col) {
            return pi_coords_to_sarsen_move(game, color,
                                            row, ++player->max_col);
        }
        else {
            return pi_coords_to_sarsen_move(game, color,
                                            row, --player->min_col);
        }
    }
    else { /* player->algorithm == ALPHA_0 */
        int opponent_color = opponent_of(color);
        int row, col;

        do {
            row = rand() % size;
            col = rand() % size;
        } while (_color_at(game, row, col) == opponent_color);

        return _coords_to_sarsen_move(game, row, col);
    }
}

int main() {
    const int ROUNDS = 10000, MAX_ALLOWED_MOVES = 300;
    int i;
    int vertical_wins = 0, horizontal_wins = 0, ties = 0;

    srand((unsigned) time(NULL)); 

    for (i = 0; i < ROUNDS; ++i) {
        druid_game *game = new_druid_game(8);
        alpha_0_player *player1;
        alpha_1_player *player2;
        int j;
        int *disqualified = calloc(2, sizeof(int));

        player1 = initialize_alpha_0_player(game, VERTICAL);
        player2 = initialize_alpha_1_player(game, HORIZONTAL);

        for (j = 0; j < MAX_ALLOWED_MOVES; ++j) {
            char *move;
            int move_result;

            if (j % 2 == 0) {
                move = make_move_alpha_0(player1);
            }
            else {
                move = make_move_alpha_1(player2);
            }
            move_result = make_move(game, move);
            free(move);
            if (move_result == INVALID_MOVE) {
                ++disqualified[j % 2];
                make_move(game, "pass");
            }
            if (game->finished) {
                break;
            }
        }
        if (game->finished) {
            if (game->player_on_turn == VERTICAL) {
                ++horizontal_wins;
            }
            else {
                ++vertical_wins;
            }
        }
        else {
            ++ties;
        }

        if (disqualified[0]) {
            printf("The first player, alpha_0, has attempted illegal moves.\n");
        }
        if (disqualified[1]) {
            printf("The second player, alpha_1, has attempted illegal moves.\n");
        }

        free(game);
    }
    printf("Vertical wins: %d\nHorizontal wins: %d\nTies: %d\n",
           vertical_wins, horizontal_wins, ties);

    return 0;
}
