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
    int **colors;
    int **heights;
} druid_game;

const int VERTICAL   = 1,
          HORIZONTAL = 2;

druid_game *new_druid_game(int size) {
    druid_game *new_game;

    new_game = malloc(sizeof (druid_game));
    new_game->size = size;
    return new_game;
}

int main() {
    druid_game *game;

    game = new_druid_game(3);
    is(game->size, 3, "game initialized with the right size");
    return 0;
}
