#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "test.h"

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

int main() {
    druid_game *game = new_druid_game(4);

    is(game->size, 4, "game initialized with the right size");
    is(game->player_on_turn, VERTICAL, "vertical starts");
    is(color_at(game, "b2"), NONE, "the board is empty at the start");
    is(height_at(game, "b2"), 0, "the board is flat at the start");

    is(make_move(game, "b2"), MOVE_MADE, "sarsen move works");
    is(color_at(game, "b2"), VERTICAL, "color changed at the position");
    is(height_at(game, "b2"), 1, "height has increased by one");
    is(game->player_on_turn, HORIZONTAL, "it is now horizontal's turn");

    is(make_move(game, "b2"), INVALID_MOVE, "can't put sarsen there");
    is(make_move(game, "a2-c2"), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");
    is(make_move(game, "b2-d2"), INVALID_MOVE,
        "lintel can't rest on a single sarsen at one end");
    is(make_move(game, "a1-c1"), INVALID_MOVE,
        "can't put lintel on ground");

    make_move(game, "b1");
    make_move(game, "b4");

    /* The board now looks like this:

         A B C D
       4 . V . . 4
       3 . . . . 3
       2 . V . . 2
       1 . H . . 1
         A B C D   */

    is(make_move(game, "b1-b3"), INVALID_MOVE,
        "lintel can't stick out in the air");
    is(make_move(game, "b2-b4"), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_move(game, "d1");
    is(make_move(game, "b1-d1"), INVALID_MOVE,
        "lintel must rest on two same-colored pieces");

    make_move(game, "c3");
    is(make_move(game, "b1-d1"), MOVE_MADE, "lintel on two pieces");
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

    is(make_move(game, "c2-c4"), INVALID_MOVE,
        "lintel can't lie on single sarsen in the middle");
    is(make_move(game, "b2-b4"), MOVE_MADE, "lintel on two pieces");

    make_move(game, "c1");
    is(make_move(game, "b1-b3"), MOVE_MADE, "lintel on one opponent piece");

    destroy_druid_game(game);
    game = new_druid_game(4);

    make_move(game, "a1");   make_move(game, "d2");
    make_move(game, "a1");   make_move(game, "d2");
    make_move(game, "b1");   make_move(game, "d3");
    make_move(game, "c1");   make_move(game, "d4");
    make_move(game, "c1");   make_move(game, "d4");

    is(make_move(game, "a1-c1"), MOVE_MADE,
        "three same-color pieces, but only two supporting (horizontal)");

    is(make_move(game, "d2-d4"), MOVE_MADE,
        "three same-color pieces, but only two supporting (vertical)");
    is(make_move(game, "a1-c1"), INVALID_MOVE,
        "three same-color pieces (horizontal)");
    make_move(game, "a2");
    is(make_move(game, "d2-d4"), INVALID_MOVE,
        "three same-color pieces (vertical)");

    make_move(game, "a3");
    make_move(game, "a4");
    make_move(game, "a3");
    is(make_move(game, "a2-a4"), INVALID_MOVE,
        "lintel can't rest on a single sarsen in the middle");

    destroy_druid_game(game);
    game = new_druid_game(3);

    is(who_won(game), NONE, "no-one won before first move");

    make_move(game, "b3");   make_move(game, "a3");
    is(who_won(game), NONE, "no-one won before the game is over");
    make_move(game, "b2");   make_move(game, "a3");
    make_move(game, "b1");

    is(who_won(game), VERTICAL, "vertical win");
    is(make_move(game, "a3"), INVALID_MOVE, "can't move after win");

    destroy_druid_game(game);

    return EXIT_SUCCESS;
}
