#ifndef GAME_H_GUARD
#define GAME_H_GUARD

typedef struct {
    int size;
    int player_on_turn;
    int *colors;
    int *heights;
    int previous_move;
    int finished;
} druid_game;

enum color {
    ILLEGAL = -1,
    NONE,
    VERTICAL,
    HORIZONTAL,
};

enum move_result {
    INVALID_MOVE,
    MOVE_MADE,
};

druid_game *new_druid_game(int size);
void destroy_druid_game(druid_game *);
int parse_coord(druid_game *, char *, char);
int color_at(druid_game *, char *);
int _color_at(druid_game *, int, int);
int height_at(druid_game *, char *);
int _height_at(druid_game *, int, int);
void _set_color_at(druid_game *, int, int, int);
void _set_height_at(druid_game *, int, int, int);
void _increase_height_at(druid_game *, int, int);
void _switch_player_on_turn(druid_game *);
int _previous_move_row(druid_game *);
int _previous_move_col(druid_game *);
void _set_previous_move(druid_game *, int, int);
int who_won(druid_game *);
int _make_sarsen_move(druid_game *, int, int);
int _make_hlintel_move(druid_game *, int, int);
int _make_vlintel_move(druid_game *, int, int);
int _make_passing_move(druid_game *);
int make_move(druid_game *, char *);

#endif
