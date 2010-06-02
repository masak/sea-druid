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

    print_header(game);
    for (row = 0; row < game->size; ++row) {
        printf("%2d", row + 1);
        for (col = 0; col < game->size; ++col) {
            printf(" .");
        }
        printf(" %-2d", row + 1);
        printf(" ");
        printf("%2d", row + 1);
        for (col = 0; col < game->size; ++col) {
            printf(" .");
        }
        printf(" %-2d\n", row + 1);
    }
    print_header(game);
}

int main() {
    druid_game *game = new_druid_game(8);

    print_board(game);

    free(game);

    return 0;
}
