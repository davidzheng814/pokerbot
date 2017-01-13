#ifndef PLAYER_H
#define PLAYER_H

#include "./cards.h"

typedef struct gamestate {
  int num_players;
  int turn;
  int pot;
  int active_pot;
  bool is_open;
  int *stacks;
  int *cur_bets;
  int num_actions;
  char *actions;
  int *bets;
  int num_board_cards;
  string *board;
  string street;
  int num_limpers;
} gamestate;

class Player {
  public:
    string label;
    int pos;
    int ind;
    range_t range;
    range_t range_c;
    arange_t arange;
    arange_t arange_c;

    Player(string label, int pos, int ind, int card1, int card2);
    void think(const vector<Player> &players, const gamestate *g);
    void move(const vector<Player> &players, const gamestate *g, char action, int bet);
};

#endif
