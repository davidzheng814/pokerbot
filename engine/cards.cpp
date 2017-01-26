#include <stdbool.h>

#include "./cards.h"
#include "./params.h"
#include "./constants.h"

string CARD_TO_STR[] = {
  "A", "K", "Q", "J", "T", "9", "8", "7", "6", "5", "4", "3", "2"
};

string combo_to_str(combo_t combo) {
  int card1 = combo / 13;
  int card2 = combo % 13;
  if (card1 == card2) {
    return CARD_TO_STR[card1] + CARD_TO_STR[card2];
  } else if (card1 < card2) {
    return CARD_TO_STR[card1] + CARD_TO_STR[card2] + "s";
  } else {
    return CARD_TO_STR[card2] + CARD_TO_STR[card1] + "o";
  }
}

vector<int> combo_to_hands(int combo) {
  int card1 = combo / 13;
  int card2 = combo % 13;
  if (card1 == card2) {
    int rank = card1 * 4;
    return {HAND(rank+1, rank), HAND(rank+2, rank), HAND(rank+2, rank+1),
      HAND(rank+3, rank), HAND(rank+3, rank+1), HAND(rank+3, rank+2)};
  } else if (card1 < card2) {
    int rank1 = card2 * 4;
    int rank2 = card1 * 4;
    return {HAND(rank1, rank2), HAND(rank1+1, rank2+1), HAND(rank1+2, rank2+2), HAND(rank1+3, rank2+3)};
  } else {
    int rank1 = card1 * 4;
    int rank2 = card2 * 4;
    return {HAND(rank1, rank2+1), HAND(rank1, rank2+2), HAND(rank1, rank2+3),
      HAND(rank1+1, rank2), HAND(rank1+1, rank2+2), HAND(rank1+1, rank2+3),
      HAND(rank1+2, rank2), HAND(rank1+2, rank2+1), HAND(rank1+2, rank2+3),
      HAND(rank1+3, rank2), HAND(rank1+3, rank2+1), HAND(rank1+3, rank2+2)};
  }
}

// int hand_to_combo(int hand) {
//   int beg = 0;
//   int end = 52;
//   int mid;
//   while (beg <= end) {
//     mid = (beg + end) / 2;
//     if (triangles[mid] <= hand && triangles[mid+1] > hand) {
//       break;
//     } else if (triangles[mid] > hand) {
//       end = mid;
//     } else {
//       beg = mid;
//     }
//   }
//   int ind1 = mid + 1;
//   int ind2 = hand - triangles[mid];
//   if (ind1 % 4 == ind2 % 4) {
//     return (ind2 / 4) * 13 + (ind1 / 4);
//   } else {
//     return (ind1 / 4) * 13 + (ind2 / 4);
//   }
//   return 0;
// }

// double get_hand_equity(int hand1, int hand2) {
//   int combo1 = HAND_TO_COMBO[hand1];
//   int x1 = combo1 / 13;
//   int y1 = combo1 % 13;
//   bool suited1 = false;
//   int combo2 = HAND_TO_COMBO[hand2];
//   int x2 = combo2 / 13;
//   int y2 = combo2 % 13;
//   bool suited2 = false;
//   bool switched = false;
// 
//   if (x1 < y1) {
//     int tmp = x1;
//     x1 = y1;
//     y1 = tmp;
//     suited1 = true;
//   }
//   
//   if (x2 < y2) {
//     int tmp = x2;
//     x2 = y2;
//     y2 = tmp;
//     suited2 = true;
//   }
// 
//   if (x2 == y2) {
//     int tmp = x1;
//     x1 = x2;
//     x2 = tmp;
//     tmp = y1;
//     y1 = y2;
//     y2 = tmp;
//     switched = true;
//   }
// 
//   double pct = 0;
// 
//   if (x1 == y1) {
//     if (x2 == y2) {
//       if (x1 < x2) pct = 0.803; // overpair v underpair
//       else if (x2 < x1) pct = 0.197; // underpair vs overpair
//       else pct = 0.5; // same pairs
//     } else {
//       if (y2 < x1) pct = 0.551; // pair vs 2 overs
//       else if (y2 == x1) pct = 0.65; // pair vs 1 over, 1 shared
//       else if (x2 < x1) pct = 0.714; // pair vs 1 over 
//       else if (x2 == x1) pct = 0.9; // pair vs 1 shared, 1 under
//       else pct = 0.827; // pair vs 2 unders
//     }
//   } else {
//     if (x1 < y2) {
//       pct = 0.629;
//     } else if (x2 < y1) {
//       pct = 0.371;
//     } else if (x1 == y2) {
//       pct = 0.733;
//     } else if (y1 == x2) {
//       pct = 0.267;
//     } else if (x2 == y2) {
// 
//     }
//   }
// 
//   if (switched) pct = 1 - pct;
//   return pct;
// }

double get_range_equity(range_t range, int hand, double top_pct) {
  pdi equities[NUM_HANDS];
  size_t start_ind = HAND_TO_COMBO[hand] * 169;
  for (int i = 0; i < NUM_HANDS; ++i) {
    equities[i] = {EQUITIES[start_ind+HAND_TO_COMBO[i]] / 100., i};
  }

  sort(equities, equities + NUM_HANDS);

  double pct = 0;
  double denom = 0;
  double equity = 0;
  int card1 = HAND_TO_CARD[2*hand];
  int card2 = HAND_TO_CARD[2*hand+1];
  for (int i = 0; i < NUM_HANDS; ++i) {
    int hand2 = equities[i].second;
    double prob = range[equities[i].second];
    pct += prob;

    int ocard1 = HAND_TO_CARD[2*hand2];
    int ocard2 = HAND_TO_CARD[2*hand2+1];

    if (ocard1 != card1 && ocard1 != card2 &&
        ocard2 != card1 && ocard2 != card2) {
      equity += equities[i].first * prob;
      denom += prob;
    }

    if (pct >= top_pct) {
      return equity / denom;
    }
  }

  return 0;
}


void bayes_apply(range_t range, arange_t arange, char action) {
  double sum = 0;
  if (action == 'f') {
    for (int i = 0; i < NUM_HANDS; ++i) {
      range[i] *= arange[i].fold;
      sum += range[i];
    }
  } else if (action == 'c' || action == 'h') {
    for (int i = 0; i < NUM_HANDS; ++i) {
      range[i] *= arange[i].call;
      sum += range[i];
    }
  } else {
    for (int i = 0; i < NUM_HANDS; ++i) {
      range[i] *= arange[i].raise;
      sum += range[i];
    }
  }

  for (int i = 0; i < NUM_HANDS; ++i) {
    range[i] /= sum;
  }
}

void set_blockers(range_t range, int card1, int card2) {
  double sum = 0;
  for (int i = 0; i < card1; ++i) {
    int hand = HAND(card1, i);
    sum += range[hand];
    range[hand] = 0;
  }

  for (int i = card1 + 1; i < NUM_CARDS; ++i) {
    int hand = HAND(i, card1);
    sum += range[hand];
    range[hand] = 0;
  }

  for (int i = 0; i < card2; ++i) {
    int hand = HAND(card2, i);
    sum += range[hand];
    range[hand] = 0;
  }

  for (int i = card2 + 1; i < NUM_CARDS; ++i) {
    int hand = HAND(i, card2);
    sum += range[hand];
    range[hand] = 0;
  }

  for (int i = 0; i < NUM_HANDS; ++i) {
    range[i] /= (1 - sum);
  }
}

void convert_to_combos_a(arange_t hands, arange_t combos) {
  for (int i = 0; i < NUM_COMBOS; ++i) {
    combos[i] = {0., 0., 0.};
  }

  for (int i = 0; i < NUM_HANDS; ++i) {
    int x = HAND_TO_COMBO[i];
    combos[x] = {hands[i].fold + combos[x].fold, hands[i].call + combos[x].call, 
      hands[i].raise + combos[x].raise};
  }

  for (int i = 0; i < NUM_COMBOS; ++i) {
    double sum = combos[i].fold + combos[i].call + combos[i].raise;
    combos[i] = {combos[i].fold / sum, combos[i].call / sum, combos[i].raise / sum};
  }
}

void convert_to_combos(range_t hands, range_t combos) {
  for (int i = 0; i < NUM_COMBOS; ++i) {
    combos[i] = 0;
  }

  for (int i = 0; i < NUM_HANDS; ++i) {
    int x = HAND_TO_COMBO[i];
    combos[x] = hands[i] + combos[x];
  }
}

void convert_to_combos_with_blockers(range_t hands, range_t combos, int card1, int card2) {
  double newhands[NUM_HANDS];
  memcpy(newhands, hands, sizeof(double) * NUM_HANDS);
  set_blockers(newhands, card1, card2);
  for (int i = 0; i < NUM_COMBOS; ++i) {
    combos[i] = 0;
  }

  for (int i = 0; i < NUM_HANDS; ++i) {
    int x = HAND_TO_COMBO[i];
    combos[x] = newhands[i] + combos[x];
  }
}

inline int rank_of(int card) {
  return card / 4;
}

inline int suit_of(int card) {
  return card % 4;
}

pii FLOP_STRENGTH[NUM_CARDS * NUM_CARDS];
pii TURN_STRENGTH[NUM_CARDS * NUM_CARDS * NUM_CARDS];
#define FLOP_IND(a, b) NUM_CARDS * a + b
#define TURN_IND(a, b, c) NUM_CARDS * NUM_CARDS * a + NUM_CARDS * b + c
pii get_strength(int card1, int card2, int *board, int num_cards) {
  pii *val;
  if (num_cards == 3) {
    val = FLOP_STRENGTH + FLOP_IND(card1, card2);
    pii strength = *val;
    if (strength != (pii){0,0}) return strength;
  } else if (num_cards == 4) {
    val = TURN_STRENGTH + TURN_IND(card1, card2, board[3]);
    pii strength = *val;
    if (strength != (pii){0,0}) return strength;
  }

  int cards[7];
  for (int i = 0; i < num_cards; ++i) {
    cards[i] = board[i];
  }
  cards[num_cards] = card1;
  cards[num_cards+1] = card2;
  num_cards += 2;

  int ranks[13];
  fill(ranks, ranks+13, 0);
  int suits[4];
  fill(suits, suits+4, 0);
  for (int i = 0; i < num_cards; ++i) {
    ranks[rank_of(cards[i])]++;
    suits[suit_of(cards[i])]++;
  }

  int trips_rank = -1;
  int quads_rank = -1;
  int pair_rank = -1;
  int two_pair_rank = -1;

  for (int i = 12; i >= 0; --i) {
    switch(ranks[i]) {
      case 4:
        quads_rank = 12 -  i; break;
      case 3:
        trips_rank = 12 - i; break;
      case 2:
        if (pair_rank == -1) {
          pair_rank = 12 - i; break;
        } else {
          two_pair_rank = pair_rank;
          pair_rank = 12 - i;
          break;
        }
    }
  }

  int num_consec = ranks[0] > 0 ? 1 : 0;
  int straight_rank = -1;
  for (int i = 12; i >= 0; --i) {
    if (ranks[i] > 0) {
      num_consec++;
      if (num_consec >= 5) {
        straight_rank = 9 - i;
      }
    } else {
      num_consec = 0;
    }
  }

  int flush_rank = -1;
  for (int i = 0; i < 4; ++i) {
    if (suits[i] >= 5) {
      for (int j = 0; j < num_cards; ++j) {
        int rank = 12 - rank_of(cards[j]);
        if (suit_of(cards[j]) == i && rank > flush_rank) {
          flush_rank = rank;
        }
      }
      break;
    }
  }

  if (quads_rank > -1) {
    *val = {7, quads_rank};
    return *val;
  }
  if (trips_rank > -1 && pair_rank > -1) {
    *val = {6, 16 * trips_rank + pair_rank};
    return *val;
  }
  if (flush_rank > -1) {
    *val = {5, flush_rank};
    return *val;
  }
  if (straight_rank > -1) {
    *val = {4, straight_rank};
    return *val;
  }
  if (trips_rank > -1) {
    int score = trips_rank;
    int num_kickers = 0;
    for (int i = 0; i < 13 && num_kickers < 2; ++i) {
      if (ranks[i] > 0 && i != trips_rank) {
        score *= 16;
        score += 12 - i;
        num_kickers++;
      }
    }
    *val = {3, score};
    return *val;
  }

  if (two_pair_rank > -1) {
    int score = pair_rank * 256 + two_pair_rank * 16;
    for (int i = 0; i < 13; ++i) {
      if (ranks[i] > 0 && i != pair_rank && i != two_pair_rank) {
        score += 12 - i;
        break;
      }
    }
    *val = {2, score};
    return *val;
  }

  if (pair_rank > -1) {
    int score = pair_rank;
    int num_kickers = 0;
    for (int i = 0; i < 13 && num_kickers < 3; ++i) {
      if (ranks[i] > 0 && i != pair_rank) {
        score *= 16;
        score += 12 - i;
        num_kickers++;
      }
    }
    *val = {1, score};
    return *val;
  }

  int score = 0;
  int num_kickers = 0;
  for (int i = 0; i < 13 && num_kickers < 5; ++i) {
    if (ranks[i] > 0) {
      score *= 16;
      score += 12 - i;
      num_kickers++;
    }
  }
  *val = {0, score};
  return *val;
}

pdd get_equity(int x1, int y1, int x2, int y2, int p1_wins, int *board, int num_board_cards, bool *deck) {
  // returns (equity, p1_numouts / deck_size)
  double p1_numouts = 0;
  for (int i = 0; i < NUM_CARDS; ++i) {
    if (!deck[i]) continue;
    board[num_board_cards] = i;
    pii p1_strength = get_strength(x1, y1, board, num_board_cards+1);
    pii p2_strength = get_strength(x2, y2, board, num_board_cards+1);

    if (p1_strength > p2_strength) {
      p1_numouts += 1.;
    } else if (p1_strength == p2_strength) {
      p1_numouts += 0.5;
    }
  }

  int deck_size = 48 - num_board_cards;
  double p1_outs_frac = p1_numouts / deck_size;
  if (p1_wins > 0) {
    if (num_board_cards == 4) return {p1_outs_frac, p1_outs_frac};
    return {p1_outs_frac * p1_outs_frac, p1_outs_frac};
  } else if (p1_wins < 0) {
    if (num_board_cards == 4) return {1 - p1_outs_frac, p1_outs_frac};
    return {2 * p1_outs_frac - p1_outs_frac * p1_outs_frac, p1_outs_frac};
  } else {
    return {p1_outs_frac, p1_outs_frac};
  }
}

standing get_standings_vs_range(range_t range, int hand, int *board, int num_board_cards) {
  bool deck[52];
  fill(deck, deck+52, true);

  int x1 = HAND_TO_CARD[hand*2];
  int y1 = HAND_TO_CARD[hand*2+1];
  deck[x1] = false, deck[y1] = false;
  for (int i = 0; i < num_board_cards; ++i) {
    deck[board[i]] = false;
  }

  double equity = 0;
  double equity_tot = 0;
  double sdv = 0;
  double sdv_tot = 0;
  double vulnerability = 0;
  double vulnerability_tot = 0;
  pii p1_strength = get_strength(x1, y1, board, num_board_cards);

  for (int i = 0; i < NUM_HANDS; ++i) {
    int x2 = HAND_TO_CARD[2*i];
    int y2 = HAND_TO_CARD[2*i+1];
    if (!deck[x2] || !deck[y2]) continue;
    deck[x2] = false;
    deck[y2] = false;
    pii p2_strength = get_strength(x2, y2, board, num_board_cards);
    int p1_wins;
    if (p1_strength > p2_strength) {
      sdv += range[i];
      p1_wins = 1;
    } else if (p1_strength < p2_strength) {
      p1_wins = -1;
    } else {
      sdv += 0.5 * range[i];
      p1_wins = 0;
    }
    sdv_tot += range[i];
    pdd equity_and_outs = get_equity(x1, y1, x2, y2, p1_wins, board, num_board_cards, deck);
    equity += equity_and_outs.first * range[i];
    equity_tot += range[i];
    if (p1_wins > 0) {
      vulnerability += (1 - equity_and_outs.second) * range[i];
      vulnerability_tot += range[i];
    }
    deck[x2] = true;
    deck[y2] = true;
  }

  return {equity / equity_tot, sdv / sdv_tot, vulnerability / vulnerability_tot};
}

int connect_dist(int rank1, int rank2) {
  int dist = abs(rank1 - rank2);
  if (dist == 1) return 3;
  if (dist == 2) return 2;
  if (dist == 3) return 1;
  return 0;
}

double board_texture(range_t range, int *board, int num_board_cards) {
  double ranks[13];
  fill(ranks, ranks+13, 0.);

  double sum = 0;
  for (int i = 0; i < NUM_HANDS; ++i) {
    int card1 = HAND_TO_CARD[2*i];
    int card2 = HAND_TO_CARD[2*i+1];
    bool shared = false;
    for (int j = 0; j < num_board_cards; ++j) {
      if (board[j] == card1 || board[j] == card2) {
        shared = true;
        break;
      }
    }
    if (shared) continue;
    ranks[rank_of(card1)] += range[i];
    ranks[rank_of(card2)] += range[i];
    sum += 2 * range[i];
  }

  double pairness = 0;
  for (int i = 0; i < num_board_cards; ++i) {
    int rank = rank_of(board[i]);
    pairness += ranks[rank] / sum;
  }

  double connectedness = 0;
  for (int i = 1; i < num_board_cards; ++i) {
    int rank1 = rank_of(board[i]);
    for (int j = 0; j < i; ++j) {
      int rank2 = rank_of(board[j]);
      connectedness += connect_dist(rank1, rank2) * (ranks[rank1] + ranks[rank2]) / sum;
    }
  }

  int suitedness = 0;
  int suits[4];
  fill(suits, suits+4, 0);
  for (int i = 0; i < num_board_cards; ++i) {
    suits[suit_of(board[i])]++;
  }
  for (int i = 0; i < 4; ++i) {
    if (suits[i] > suitedness) suitedness = suits[i];
  }
  suitedness--;

  return 5 * pairness + 2 * connectedness + suitedness;
}
