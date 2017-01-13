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

typedef struct act {
  double fold;
  double call;
  double raise;
} act;

typedef pair<int, int> pii;
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

extern string CARD_TO_STR[];
extern const strength STRENGTHS[];

// Based on STRENGTHS, return combo ordering for a given weight vector.
vector<int> get_ordering(weight weight);

// Returns the string associated with a given combo.
string combo_to_str(combo_t combo);
void set_starting_range(range_t range);
void set_blockers(range_t range, int card1, int card2);
void set_open_arange(arange_t out, position pos, int num_limpers);
void set_default_arange(arange_t out);
void convert_to_combos_a(arange_t hands, arange_t combos);
void convert_to_combos(range_t hands, range_t combos);
void bayes_apply(range_t range, arange_t arange, char action);
#endif
