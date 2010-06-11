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

typedef struct {
    druid_game *game;
    int opponent_color;
} alpha_0_player;

alpha_0_player *initialize_alpha_0_player(druid_game *game, int color) {
    alpha_0_player *player = malloc(sizeof(alpha_0_player));
    player->game = game;
    player->opponent_color = color == VERTICAL ? HORIZONTAL : VERTICAL;

    return player;
}

char *make_move_alpha_0(alpha_0_player *player) {
    druid_game *game = player->game;
    int size = game->size, opponent_color = player->opponent_color;
    int row, col;

    do {
        row = rand() % size;
        col = rand() % size;
    } while (_color_at(game, row, col) == opponent_color);

    return _coords_to_sarsen_move(game, row, col);
}

int main() {
    const int ROUNDS = 10000, MAX_ALLOWED_MOVES = 300;
    int i;
    int vertical_wins = 0, horizontal_wins = 0, ties = 0;

    srand((unsigned) time(NULL)); 

    for (i = 0; i < ROUNDS; ++i) {
        druid_game *game = new_druid_game(8);
        alpha_0_player **players = malloc(sizeof(alpha_0_player*) * 2);
        int j;
        int *disqualified = calloc(2, sizeof(int));

        players[0] = initialize_alpha_0_player(game, VERTICAL);
        players[1] = initialize_alpha_0_player(game, HORIZONTAL);

        for (j = 0; j < MAX_ALLOWED_MOVES; ++j) {
            char *move;
            alpha_0_player *current_player = players[j % 2];

            move = make_move_alpha_0(current_player);
            if (make_move(game, move) == INVALID_MOVE) {
                ++disqualified[j % 2];
                free(move);
                move = "pass";
                make_move(game, move);
            }
            else {
                free(move);
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
            printf("The second player, alpha_0, has attempted illegal moves.\n");
        }

        free(game);
    }
    printf("Vertical wins: %d\nHorizontal wins: %d\nTies: %d\n",
           vertical_wins, horizontal_wins, ties);

    return 0;
}
