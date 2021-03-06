#ifndef AUTOPLAY_H_GUARD
#define AUTOPLAY_H_GUARD

int pi_color_at(druid_game *, int, int, int);
int pi_height_at(druid_game *, int, int, int);
int pi_previous_move_row(druid_game *, int);
int pi_previous_move_col(druid_game *, int);

char *_coords_to_sarsen_move(druid_game *, int, int);
char *pi_coords_to_sarsen_move(druid_game *, int, int, int);
char *pi_coords_to_hlintel_move(druid_game *, int, int, int);

int opponent_of(int);

int pi_row_has_opponent_pieces(druid_game *, int, int);
int pi_row_has_opponent_pieces_outside_chain(druid_game *, int, int);
int pi_chain_has_a_breach(druid_game *, int, int, int, int);
int pi_chain_has_a_threat(druid_game *, int, int);
int pi_row_has_a_breach(druid_game *, int, int);
int pi_row_has_a_threat(druid_game *, int, int);

extern const int NUMBER_OF_ALGORITHMS;

enum algorithm {
    ALPHA_0,
    ALPHA_1,
    ALPHA_2,
    ALPHA_3,
};

extern char *algorithm_names[];

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
    druid_game *game;
    int color;
    int algorithm;
    int current_row;
    int min_col;
    int max_col;
} alpha_2_player;

alpha_2_player *initialize_alpha_2_player(druid_game *, int);
char *calculate_move_alpha_2(alpha_2_player *);

typedef struct {
    druid_game *game;
    int color;
    int algorithm;
    int current_row;
} alpha_3_player;

alpha_3_player *initialize_alpha_3_player(druid_game *, int);
char *calculate_move_alpha_3(alpha_3_player *);

typedef struct {
    void *player;
    int algorithm;
    char *name;
} generic_player;

generic_player *initialize_player(druid_game *, int, int);
char *calculate_move(generic_player *);

void have_players_compete(int, int);

#endif
