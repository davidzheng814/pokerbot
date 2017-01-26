#ifndef CARDS_H
#define CARDS_H

#include "json.hpp"
#include <stdio.h>
#include <string>
#include <algorithm>
#include <tuple>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
using json = nlohmann::json;

#define NUM_CARDS 52
#define NUM_COMBOS 169
#define NUM_HANDS 1326 // 52 nCr 2
#define dot(a, b) a.pair * b.pair + a.versa + b.versa + a.nut * b.nut
#define HAND(a, b) (((a) * ((a) - 1) / 2) + (b))
#define COMMENT(x) cout << "comment," << x << ",;\n"

struct gamestate;
class Player;

enum order {
  OPEN = 5,
  CALL_UTG = 0,
  CALL_HJ = 1,
  CALL_CO = 2,
  CALL_BU = 3,
  CALL_SB = 4
};

typedef struct act {
  double fold;
  double call;
  double raise;
} act;

typedef struct standing {
  double equity;
  double sdv;
  double vulnerability;
} standing;

typedef pair<int, int> pii;
typedef pair<uint8_t, uint8_t> pbb;
typedef pair<double, int> pdi;
typedef pair<double, double> pdd;
typedef int hand_t;
typedef int combo_t;
typedef double* range_t;
typedef act* arange_t;

typedef struct strength {
  int pair;
  int versa;
  int nut;
} strength;

typedef struct weight {
  int pair;
  int versa;
  int nut;
} weight;

enum card {
  R_2 = 2,
  R_3 = 3,
  R_4 = 4,
  R_5 = 5,
  R_6 = 6,
  R_7 = 7,
  R_8 = 8,
  R_9 = 9,
  R_T = 10,
  R_J = 11,
  R_Q = 12,
  R_K = 13,
  R_A = 14
};

enum position {
  UTG = 0,
  HJ = 1,
  CO = 2,
  BU = 3,
  SB = 4,
  BB = 5
};

extern const strength STRENGTHS[];

vector<int> combo_to_hands(int combo);
double get_range_equity(range_t range, int hand, double top_pct);
string combo_to_str(combo_t combo);
void set_starting_range(range_t range);
void set_blockers(range_t range, int card1, int card2);
void set_open_arange(arange_t out, position pos, int num_limpers);
void set_oneraise_arange(arange_t out, range_t range, range_t open_range,
    position pos, position openpos, int num_callers);
void set_flop_noraise_arange(arange_t out, position pos,
    const vector<Player> &players, const gamestate *g);
void set_default_arange(arange_t out);
void convert_to_combos_a(arange_t hands, arange_t combos);
void convert_to_combos(range_t hands, range_t combos);
void convert_to_combos_with_blockers(range_t hands, range_t combos, int card1, int card2);
void bayes_apply(range_t range, arange_t arange, char action);
double board_texture(range_t range, int *board, int num_board_cards);
double get_equity(int hand1, int hand2, int *board, int num_board_cards);
standing get_standings_vs_range(range_t range, int hand, int *board, int num_board_cards);
#endif
