#include <stdbool.h>

#include "./cards.h"
#include "./constants.h"
#include "./params.h"

string CARD_TO_STR[] = {
  "A", "K", "Q", "J", "T", "9", "8", "7", "6", "5", "4", "3", "2"
};

vector<int> get_ordering(weight weight) {
  vector<pii> scores(NUM_COMBOS);
  for (int i = 0; i < NUM_COMBOS; ++i) {
    scores[i] = {dot(weight, STRENGTHS[i]), i};
  }
  sort(scores.begin(), scores.end());
  vector<int> final_scores(NUM_COMBOS);
  for (int i = 0; i < NUM_COMBOS; ++i) {
    final_scores[i] = scores[i].second;
  }
  return final_scores;
}

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

inline vector<int> combo_to_hands(int combo) {
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

// TODO add nit on button
int get_open_thresh(position pos, int num_fish_in_pos, int num_fish_out_pos,
                    int num_light_in_pos, int num_light_out_pos) {
  double loose_factor = FISH_POS_WT * num_fish_in_pos + num_fish_out_pos;
  double tight_factor = LIGHT_POS_WT * num_light_out_pos + num_light_in_pos;
  double pct = 0;
  switch (pos) {
    case UTG:
      pct = UTG_DEFAULT_PCT + loose_factor * UTG_LOOSE_INC - tight_factor * UTG_TIGHT_INC;
      break;
    case HJ:
      pct = HJ_DEFAULT_PCT + loose_factor * HJ_LOOSE_INC - tight_factor * HJ_TIGHT_INC;
      break;
    case CO:
      pct = CO_DEFAULT_PCT + loose_factor * CO_LOOSE_INC - tight_factor * CO_TIGHT_INC;
      break;
    case BU:
      pct = BU_DEFAULT_PCT + loose_factor * BU_LOOSE_INC - tight_factor * BU_TIGHT_INC;
      break;
    case SB:
      pct = SB_DEFAULT_PCT + loose_factor * SB_LOOSE_INC - tight_factor * SB_TIGHT_INC;
      break;
    default:
      break;
  }
  return pct * NUM_HANDS;
}

void set_open_arange(arange_t out, position pos, int num_limpers) {
  size_t thresh = get_open_thresh(pos, 0, 0, 0, 0);
  vector<int> combos = get_ordering({2, 1, 1});
  vector<int> hands;

  for (size_t i = 0; i < combos.size(); ++i) {
    vector<int> x = combo_to_hands(combos[i]);
    hands.insert(hands.end(), x.begin(), x.end());
  }

  for (size_t i = 0; i < NUM_HANDS; ++i) {
    out[i] = {.96, 0.02, 0.02};
  }

  if (num_limpers == 0) {
    for (size_t i = 0; i < thresh; ++i) {
      out[hands[i]] = {0, 0.02, 0.98};
    }

    size_t buffer = 66; // 5% buffer
    size_t buffered_thresh = min(thresh + buffer, (size_t) NUM_HANDS);

    for (size_t i = thresh; i < buffered_thresh; ++i) {
      out[hands[i]] = {0.49, 0.02, 0.49};
    }
  } else {
    thresh -= 66; // lower default raise threshold
  }
}

void set_default_arange(arange_t out) {
  for (int i = 0; i < NUM_HANDS; ++i) {
    out[i] = {0., 0. , 1.};
  }
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

void set_starting_range(range_t range) {
  for (int i = 0; i < NUM_HANDS; ++i) range[i] = 1. / NUM_HANDS;
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
