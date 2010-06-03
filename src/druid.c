#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "druid.h"

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

int main() {
    druid_game *game = new_druid_game(8);
    char *move = malloc(sizeof(char) * 80);

    while (1) {
        print_board(game);
        printf("%s> ", game->player_on_turn == VERTICAL
                        ? "vertical" : "horizontal");
        if (scanf("%s", move) == EOF) {
            printf("\n");
            exit(0);
        }
        if (make_move(game, move) == INVALID_MOVE) {
            printf("Illegal move.\n");
        }
    }

    free(game);

    return 0;
}
