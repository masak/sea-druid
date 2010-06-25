#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "druid.h"
#include "autoplay.h"

const int NUMBER_OF_ALGORITHMS = 3;
char *algorithm_names[] = { "alpha-0", "alpha-1", "alpha-2" };

void print_header(druid_game *game) {
    int col;

    printf("  ");
    for (col = 0; col < game->size; ++col) {
        printf(" %c", 'A' + col);
    }
    printf("      ");
    for (col = 0; col < game->size; ++col) {
        printf(" %c", 'A' + col);
    }
    printf("\n");
}

void print_board(druid_game *game) {
    int row, col;

    printf("\n");
    print_header(game);
    for (row = game->size - 1; row >= 0; --row) {
        printf("%2d", row + 1);
        for (col = 0; col < game->size; ++col) {
            int color = _color_at(game, row, col);
            printf(" %c", color == VERTICAL ? 'V'
                        : color == HORIZONTAL ? 'H'
                        : '.');
        }
        printf(" %-2d", row + 1);
        printf(" ");
        printf("%2d", row + 1);
        for (col = 0; col < game->size; ++col) {
            int height = _height_at(game, row, col);
            printf(" %c", height > 9 ? '#'
                        : height == 0 ? '.'
                        : '0' + height);
        }
        printf(" %-2d\n", row + 1);
    }
    print_header(game);
    printf("\n");
}

void autoplay() {
    int p1, p2;

    for (p1 = 0; p1 < NUMBER_OF_ALGORITHMS; ++p1) {
        for (p2 = 0; p2 < NUMBER_OF_ALGORITHMS; ++p2) {
            have_players_compete(p1, p2);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    srand((unsigned) time(NULL)); 

    if (argc == 2 && strcmp(argv[1], "--autoplay") == 0) {
        autoplay();
        exit(EXIT_SUCCESS);
    }
    if (argc != 1) {
        printf("Unknown option.\n");
        exit(EXIT_FAILURE);
    }

    druid_game *game = new_druid_game(8);
    char *move = malloc(sizeof(char) * 80);
    generic_player *computer_player
        = initialize_player(game, ALPHA_2, HORIZONTAL);

    print_board(game);
    while (!game->finished) {
        char *computer_move;
        printf("%s> ", game->player_on_turn == VERTICAL
                        ? "vertical" : "horizontal");
        if (scanf("%s", move) == EOF) {
            printf("\n");
            exit(0);
        }
        if (make_move(game, move) == INVALID_MOVE) {
            printf("Illegal move.\n");
            continue;
        }
        if (game->finished) {
            break;
        }
        computer_move = calculate_move(computer_player);
        if (make_move(game, computer_move) == INVALID_MOVE) {
            printf("%s tried to move %s, which is illegal. Aborting.\n",
                   computer_player->name, computer_move);
            free(computer_move);
            exit(EXIT_FAILURE);
        }
        printf("%s makes move %s.\n", computer_player->name, computer_move);
        free(computer_move);
        print_board(game);
    }
    print_board(game);
    printf("%s won.\n", game->player_on_turn == VERTICAL
                            ? "horizontal" : "vertical");

    free(game);

    return EXIT_SUCCESS;
}
