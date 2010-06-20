#ifndef AUTOPLAY_H_GUARD
#define AUTOPLAY_H_GUARD

char *_coords_to_sarsen_move(druid_game *, int, int);
int opponent_of(int);
int pi_row_has_opponent_pieces(druid_game *, int, int);
char *pi_coords_to_sarsen_move(druid_game *, int, int, int);

enum algorithm {
    ALPHA_0,
    ALPHA_1,
};

typedef struct {
    druid_game *game;
    int color;
} alpha_0_player;

alpha_0_player *initialize_alpha_0_player(druid_game *, int);
char *make_move_alpha_0(alpha_0_player *);

typedef struct {
    druid_game *game;
    int color;
    int algorithm;
    int current_row;
    int min_col;
    int max_col;
} alpha_1_player;

alpha_1_player *initialize_alpha_1_player(druid_game *, int);
char *make_move_alpha_1(alpha_1_player *);

#endif
