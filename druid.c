#include <stdio.h>
#include <stdlib.h>

const int BOARD_SIZE = 10;

void print_header() {
    int col;

    printf("  ");
    for (col = 0; col < BOARD_SIZE; ++col) {
        printf(" %c", 'A' + col);
    }
    printf("      ");
    for (col = 0; col < BOARD_SIZE; ++col) {
        printf(" %c", 'A' + col);
    }
    printf("\n");
}

void print_board(int **pieces, int **heights) {
    int row, col;

    print_header();
    for (row = 0; row < BOARD_SIZE; ++row) {
        printf("%2d", row + 1);
        for (col = 0; col < BOARD_SIZE; ++col) {
            printf(" .");
        }
        printf(" %-2d", row + 1);
        printf(" ");
        printf("%2d", row + 1);
        for (col = 0; col < BOARD_SIZE; ++col) {
            printf(" .");
        }
        printf(" %-2d\n", row + 1);
    }
    print_header();
}

int main() {
    int **pieces;
    int **heights;

    pieces  = malloc(sizeof (int) * BOARD_SIZE * BOARD_SIZE);
    heights = malloc(sizeof (int) * BOARD_SIZE * BOARD_SIZE);

    print_board(pieces, heights);

    return 0;
}
