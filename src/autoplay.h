#ifndef AUTOPLAY_H_GUARD
#define AUTOPLAY_H_GUARD

char *_coords_to_sarsen_move(druid_game *, int, int);
int opponent_of(int);
int pi_row_has_opponent_pieces(druid_game *, int, int);
char *pi_coords_to_sarsen_move(druid_game *, int, int, int);

const int NUMBER_OF_ALGORITHMS = 2;

enum algorithm {
    ALPHA_0,
    ALPHA_1,
};

char *algorithm_names[] = { "alpha-0", "alpha-1" };

typedef struct {
    druid_game *game;
    int color;
} alpha_0_player;

alpha_0_player *initialize_alpha_0_player(druid_game *, int);
char *calculate_move_alpha_0(alpha_0_player *);

typedef struct {
    druid_game *game;
    int color;
    int algorithm;
    int current_row;
    int min_col;
    int max_col;
} alpha_1_player;

alpha_1_player *initialize_alpha_1_player(druid_game *, int);
char *calculate_move_alpha_1(alpha_1_player *);

typedef struct {
    void *player;
    int algorithm;
    char *name;
} generic_player;

generic_player *initialize_player(druid_game *, int, int);
char *calculate_move(generic_player *);

#endif
