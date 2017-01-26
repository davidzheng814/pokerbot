#ifndef PLAYER_H
#define PLAYER_H

#include "./cards.h"

typedef struct gamestate {
  int num_players;
  int turn;
  int last_turn;
  int pot;
  int active_pot;
  int num_raises;
  int num_callers;
  int *stacks;
  int *cur_bets;
  int num_actions;
  char *actions;
  int *bets;
  int num_board_cards;
  int *board;
  string street;
  int num_limpers;
  int open_ind;
  bool heads_up;
  int aggressor_ind;
  int caller_ind;
} gamestate;

class Player {
  public:
    string label;
    position pos;
    int ind;
    range_t range;
    range_t range_c;
    arange_t arange;
    arange_t arange_c;
    int me_card1;
    int me_card2;

    Player(string label, int pos, int ind, int card1, int card2);
    void think(const vector<Player> &players, const gamestate *g);
    void move(const vector<Player> &players, const gamestate *g, char action, int bet);
};

#endif
