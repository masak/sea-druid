#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "autoplay.h"

const int NUMBER_OF_ALGORITHMS = 3;
char *algorithm_names[] = { "alpha-0", "alpha-1", "alpha-2" };

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

char *_coords_to_lintel_move(druid_game *game,
                             int row_a, int col_a, int row_b, int col_b) {
    char *move = malloc(sizeof(char) * 7);
    int pos;

    move[0] = 'a' + col_a;
    if (row_a >= 10) {
        move[1] = '0' + (row_a + 1) / 10;
        move[2] = '0' + (row_a + 1) % 10;
        move[3] = '-';
        pos = 4;
    }
    else {
        move[1] = '0' + (row_a + 1);
        move[2] = '-';
        pos = 3;
    }
    move[pos] = 'a' + col_b;
    if (row_b >= 10) {
        move[pos + 1] = '0' + (row_b + 1) / 10;
        move[pos + 2] = '0' + (row_b + 1) % 10;
        move[pos + 3] = '\0';
    }
    else {
        move[pos + 1] = '0' + (row_b + 1);
        move[pos + 2] = '\0';
    }

    return move;
}

int opponent_of(int player) {
    return player == HORIZONTAL ? VERTICAL : HORIZONTAL;
}

int pi_color_at(druid_game *game, int color, int row, int col) {
    return color == VERTICAL
            ? _color_at(game, col, row)
            : _color_at(game, row, col);
}

int pi_height_at(druid_game *game, int color, int row, int col) {
    return color == VERTICAL
            ? _height_at(game, col, row)
            : _height_at(game, row, col);
}

int pi_row_has_opponent_pieces(druid_game *game, int color, int row) {
    int col, size = game->size, opponent_color = opponent_of(color);
    for (col = 0; col < size; ++col) {
        if (pi_color_at(game, color, row, col) == opponent_color) {
            return 1;
        }
    }
    return 0;
}

int pi_row_has_opponent_pieces_outside_chain(druid_game *game,
                                             int color, int row) {
    int col, size = game->size, opponent_color = opponent_of(color);
    for (col = 0;
         col < size && pi_color_at(game, color, row, col) != color;
         ++col) {
        if (pi_color_at(game, color, row, col) == opponent_color) {
            return 1;
        }
    }
    for (col = size - 1;
         col >= 0 && pi_color_at(game, color, row, col) != color;
         --col) {
        if (pi_color_at(game, color, row, col) == opponent_color) {
            return 1;
        }
    }
    return 0;
}

int pi_chain_has_a_breach(druid_game *game, int color, int row,
                          int min_col, int max_col) {
    int col, opponent_color = opponent_of(color);
    if (pi_color_at(game, color, row, min_col) == opponent_color
        || pi_color_at(game, color, row, max_col) == opponent_color) {
        return 1;
    }
    for (col = min_col + 1; col < max_col; ++col) {
        if (pi_color_at(game, color, row, col) == opponent_color) {
            int height_one_end, height_middle, height_other_end;
            if (pi_color_at(game, color, row, col + 1) == opponent_color) {
                return 1;
            }
            height_one_end   = pi_height_at(game, color, row, col - 1);
            height_middle    = pi_height_at(game, color, row, col);
            height_other_end = pi_height_at(game, color, row, col + 1);
            if (height_middle > height_one_end
                || height_middle > height_other_end) {
                return 1;
            }
        }
    }
    return 0;
}

int pi_chain_has_a_threat(druid_game *game, int color, int row) {
    int col, size = game->size, opponent_color = opponent_of(color);
    for (col = 0; col < size; ++col) {
        if (pi_color_at(game, color, row, col) == opponent_color
            && pi_color_at(game, color, row, col - 1) == color
            && pi_color_at(game, color, row, col + 1) == color) {
            return col;
        }
    }
    return -1;
}

char *pi_coords_to_sarsen_move(druid_game *game, int color,
                               int row, int col) {
    return color == VERTICAL
        ? _coords_to_sarsen_move(game, col, row)
        : _coords_to_sarsen_move(game, row, col);
}

char *pi_coords_to_hlintel_move(druid_game *game, int color,
                                int row, int col) {
    return color == VERTICAL
        ? _coords_to_lintel_move(game, col, row, col + 2, row)
        : _coords_to_lintel_move(game, row, col, row, col + 2);
}

alpha_0_player *initialize_alpha_0_player(druid_game *game, int color) {
    alpha_0_player *player = malloc(sizeof(alpha_0_player));
    player->game = game;
    player->color = color;

    return player;
}

char *calculate_move_alpha_0(alpha_0_player *player) {
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

char *calculate_move_alpha_1(alpha_1_player *player) {
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

alpha_2_player *initialize_alpha_2_player(druid_game *game, int color) {
    alpha_2_player *player = malloc(sizeof(alpha_2_player));
    player->game = game;
    player->algorithm = ALPHA_2;
    player->color = color;
    player->current_row = ILLEGAL;

    return player;
}

char *calculate_move_alpha_2(alpha_2_player *player) {
    druid_game *game = player->game;
    int row = player->current_row, color = player->color, size = game->size;

    if (player->algorithm == ALPHA_2
        && (row == ILLEGAL
            || pi_row_has_opponent_pieces_outside_chain(game, color, row)
            || pi_chain_has_a_breach(
                game, color, row, player->min_col, player->max_col))) {

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

    if (player->algorithm == ALPHA_2) {
        int col;
        if ((col = pi_chain_has_a_threat(game, color, row)) != -1) {
            int height_one_side
                    = pi_height_at(game, color, row, col - 1),
                height_other_side
                    = pi_height_at(game, color, row, col + 1);
            if (height_one_side < height_other_side) {
                return pi_coords_to_sarsen_move(game, color, row, col - 1);
            }
            else if (height_other_side < height_one_side) {
                return pi_coords_to_sarsen_move(game, color, row, col + 1);
            }
            else {
                return pi_coords_to_hlintel_move(
                        game, color, row, col - 1);
            }
        }
        else {
            int still_expanding
                = player->max_col < size - 1 || player->min_col > 0;
            if (still_expanding) {
                if (size - player->max_col > player->min_col) {
                    player->max_col += player->max_col < size - 2 ? 2 : 1;
                    return pi_coords_to_sarsen_move(game, color,
                                                    row, player->max_col);
                }
                else {
                    player->min_col -= player->min_col >= 2 ? 2 : 1;
                    return pi_coords_to_sarsen_move(game, color,
                                                    row, player->min_col);
                }
            }
            else {
                do {
                    col = rand() % size;
                } while (pi_color_at(game, color, row, col) != NONE);
                return pi_coords_to_sarsen_move(game, color, row, col);
            }
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

generic_player *initialize_player(druid_game *game, int algorithm, int color) {
    generic_player *player = malloc(sizeof(generic_player));
    player->algorithm = algorithm;
    if (algorithm == ALPHA_0) {
        player->player = (void *)initialize_alpha_0_player(game, color);
    }
    else if (algorithm == ALPHA_1) {
        player->player = (void *)initialize_alpha_1_player(game, color);
    }
    else if (algorithm == ALPHA_2) {
        player->player = (void *)initialize_alpha_2_player(game, color);
    }
    else {
        fprintf(stderr, "Illegal algorithm %d -- must be between 0 and %d\n",
                        algorithm, NUMBER_OF_ALGORITHMS);
        exit(EXIT_FAILURE);
    }
    return player;
}

char *calculate_move(generic_player *player) {
    if (player->algorithm == ALPHA_0) {
        return calculate_move_alpha_0((alpha_0_player*) player->player);
    }
    else if (player->algorithm == ALPHA_1) {
        return calculate_move_alpha_1((alpha_1_player*) player->player);
    }
    else if (player->algorithm == ALPHA_2) {
        return calculate_move_alpha_2((alpha_2_player*) player->player);
    }

    fprintf(stderr, "Illegal algorithm %d -- must be between 0 and %d\n",
                    player->algorithm, NUMBER_OF_ALGORITHMS);
    exit(EXIT_FAILURE);
}

void have_players_compete(int p1, int p2) {
    const int ROUNDS = 10000, MAX_ALLOWED_MOVES = 300;
    int vertical_wins = 0, horizontal_wins = 0, ties = 0;
    int i;

    for (i = 0; i < ROUNDS; ++i) {
        druid_game *game = new_druid_game(8);
        generic_player *player1, *player2;
        int j;
        int *disqualified = calloc(2, sizeof(int));

        player1 = initialize_player(game, p1, VERTICAL);
        player2 = initialize_player(game, p2, HORIZONTAL);

        for (j = 0; j < MAX_ALLOWED_MOVES; ++j) {
            char *move;
            int move_result;

            move = calculate_move(j % 2 == 0 ? player1 : player2);
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
            printf("The first player, %s, has attempted illegal moves.\n",
                   player1->name);
        }
        if (disqualified[1]) {
            printf("The second player, %s, has attempted illegal moves.\n",
                   player2->name);
        }

        free(game);
    }
    printf("%s against %s: %d-(%d)-%d\n",
           algorithm_names[p1], algorithm_names[p2],
           vertical_wins, ties, horizontal_wins);
}

int main() {
    int p1, p2;

    srand((unsigned) time(NULL)); 

    for (p1 = 0; p1 < NUMBER_OF_ALGORITHMS; ++p1) {
        for (p2 = 0; p2 < NUMBER_OF_ALGORITHMS; ++p2) {
            have_players_compete(p1, p2);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
