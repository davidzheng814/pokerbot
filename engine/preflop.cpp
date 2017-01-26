#include <stdbool.h>

#include "./cards.h"
#include "./params.h"
#include "./constants.h"

// TODO add nit on button
int get_open_thresh(position pos, int num_fish_in_pos, int num_fish_out_pos,
                    int num_light_in_pos, int num_light_out_pos) {
  double loose_factor = FISH_POS_WT * num_fish_in_pos + num_fish_out_pos;
  double tight_factor = LIGHT_POS_WT * num_light_out_pos + num_light_in_pos;
  double pct = 0;
  switch (pos) {
    case UTG:
      pct = UTG_OPEN_PCT + loose_factor * UTG_OPEN_LOOSE_INC - tight_factor * UTG_OPEN_TIGHT_INC;
      break;
    case HJ:
      pct = HJ_OPEN_PCT + loose_factor * HJ_OPEN_LOOSE_INC - tight_factor * HJ_OPEN_TIGHT_INC;
      break;
    case CO:
      pct = CO_OPEN_PCT + loose_factor * CO_OPEN_LOOSE_INC - tight_factor * CO_OPEN_TIGHT_INC;
      break;
    case BU:
      pct = BU_OPEN_PCT + loose_factor * BU_OPEN_LOOSE_INC - tight_factor * BU_OPEN_TIGHT_INC;
      break;
    case SB:
      pct = SB_OPEN_PCT + loose_factor * SB_OPEN_LOOSE_INC - tight_factor * SB_OPEN_TIGHT_INC;
      break;
    default:
      break;
  }
  return pct * NUM_HANDS;
}

const uint8_t* get_ordering(order order) {
  if (order == OPEN) {
    return OPEN_RANK;
  } else if (order == CALL_UTG) {
    return CALL_UTG_RANK;
  } else if (order == CALL_HJ) {
    return CALL_HJ_RANK; 
  } else if (order == CALL_CO) {
    return CALL_CO_RANK;
  } else if (order == CALL_BU) {
    return CALL_BU_RANK;
  } else {
    return CALL_BU_RANK;
  }
}

bool is_versatile(int x, int y) {
  if (x == 0 && x < y) return true; // suited aces
  if (x < y && y - x <= 2) return true; // suited connector/gapper
  return false;
}

bool is_double_blocker(int x, int y) {
  if (x < 4 && y < 4) return true; // double blockers
  return false;
}

bool is_3bet_hand(int hand, bool polar_3bet) {
  int combo = HAND_TO_COMBO[hand];
  int x = combo / 13, y = combo % 13;
  if (polar_3bet) {
    return is_double_blocker(x, y) || is_versatile(x, y);
  } else {
    return is_double_blocker(x, y) && (x > y);
  }
}


void set_open_arange(arange_t out, position pos, int num_limpers) {
  size_t thresh = get_open_thresh(pos, 0, 0, 0, 0);
  const uint8_t *combos = get_ordering(OPEN);
  vector<int> hands;

  for (size_t i = 0; i < NUM_COMBOS; ++i) {
    vector<int> x = combo_to_hands(combos[i]);
    hands.insert(hands.end(), x.begin(), x.end());
  }

  for (size_t i = 0; i < NUM_HANDS; ++i) {
    out[i] = {.96, 0.02, 0.02};
  }

  if (num_limpers == 0) {
    for (size_t i = 0; i < thresh; ++i) {
      out[hands[i]] = {0.02, 0.02, 0.96};
    }

    size_t buffer = OPEN_BUFFER_PCT * NUM_HANDS; // 5% buffer
    size_t buffered_thresh = min(thresh + buffer, (size_t) NUM_HANDS);

    for (size_t i = thresh; i < buffered_thresh; ++i) {
      out[hands[i]] = {0.49, 0.02, 0.49};
    }
  } else {
    thresh -= 66; // lower default raise threshold
  }
}

size_t get_callopen_thresh(position pos, position openpos) {
  double pct = 0;
  if (openpos == UTG) {
    switch (pos) {
      case HJ:
        pct = HJ_UTG_CALLOPEN_PCT; break;
      case CO:
        pct = CO_UTG_CALLOPEN_PCT; break;
      case BU:
        pct = BU_UTG_CALLOPEN_PCT; break;
      case SB:
        pct = SB_UTG_CALLOPEN_PCT; break;
      case BB:
        pct = BB_UTG_CALLOPEN_PCT; break;
      default: break;
    }
  } else if (openpos == HJ) {
    switch (pos) {
      case CO:
        pct = CO_HJ_CALLOPEN_PCT; break;
      case BU:
        pct = BU_HJ_CALLOPEN_PCT; break;
      case SB:
        pct = SB_HJ_CALLOPEN_PCT; break;
      case BB:
        pct = BB_HJ_CALLOPEN_PCT; break;
      default: break;
    }
  } else if (openpos == CO) {
    switch (pos) {
      case BU:
        pct = BU_CO_CALLOPEN_PCT; break;
      case SB:
        pct = SB_CO_CALLOPEN_PCT; break;
      case BB:
        pct = BB_CO_CALLOPEN_PCT; break;
      default: break;
    }
  } else if (openpos == BU) {
    switch (pos) {
      case SB:
        pct = SB_BU_CALLOPEN_PCT; break;
      case BB:
        pct = BB_BU_CALLOPEN_PCT; break;
      default: break;
    }
  } else {
    switch (pos) {
      case BB:
        pct = BB_SB_CALLOPEN_PCT; break;
      default: break;
    }
  }

  return pct * NUM_HANDS;
}

void set_3bet_arange(arange_t arange, range_t range, range_t open_range,
    vector<int> &hands, size_t thresh, double call_3bet_pct, double bluff_ratio,
    bool polar_3bet) {
  const act prob = {0.02, 0.14, 0.84};
  bool is_3bet[NUM_HANDS] = { false };
  double pct_3bet_value = 0;
  for (size_t i = 0; i < thresh; ++i) {
    int hand = hands[i];
    double equity = get_range_equity(open_range, hand, call_3bet_pct);
    if (equity >= 0.48) {
      is_3bet[hand] = true;
      pct_3bet_value += range[hand];
      arange[hand] = prob;
      // printf("Value: %s %0.4f\n", combo_to_str(HAND_TO_COMBO[hand]).c_str(), equity);
    }
  }

  const size_t bonus = (size_t)BONUS_3BET;
  double pct_3bet_bluff = bluff_ratio * pct_3bet_value;

  size_t start = polar_3bet ? thresh : 0;
  size_t cap = min((size_t)NUM_HANDS, thresh + bonus);
  for (size_t i = start; i < cap && pct_3bet_bluff >= 0; ++i) {
    int hand = hands[i];
    if (!is_3bet[hand] && is_3bet_hand(hand, polar_3bet)) {
      pct_3bet_bluff -= range[hand];
      arange[hand] = prob;
      is_3bet[hand] = true;
      // printf("Bluff-b: %s\n", combo_to_str(HAND_TO_COMBO[hand]).c_str());
    }
  }

  for (size_t i = start; i < NUM_HANDS && pct_3bet_bluff >= 0; ++i) {
    int hand = hands[i];
    if (!is_3bet[hand]) {
      pct_3bet_bluff -= range[hand];
      arange[hand] = prob;
      is_3bet[hand] = true;
      // printf("Bluff: %s\n", combo_to_str(HAND_TO_COMBO[hand]).c_str());
    }

    if (pct_3bet_bluff < 0) break;

    if (i >= bonus) {
      hand = hands[i+bonus];
      if (!is_3bet[hand] && is_3bet_hand(hand, polar_3bet)) {
        pct_3bet_bluff -= range[hand];
        arange[hand] = prob;
        is_3bet[hand] = true;
        // printf("Bluff-a: %s\n", combo_to_str(HAND_TO_COMBO[hand]).c_str());
      }
    }
  }
}

void set_starting_range(range_t range) {
  for (int i = 0; i < NUM_HANDS; ++i) range[i] = 1. / NUM_HANDS;
}

void set_default_arange(arange_t out) {
  for (int i = 0; i < NUM_HANDS; ++i) {
    out[i] = {0., 0. , 1.};
  }
}


void set_oneraise_arange(arange_t out, range_t range, range_t open_range,
    position pos, position openpos, int num_callers) {
  size_t thresh = get_callopen_thresh(pos, openpos);
  bool polar_3bet = true;
  if (num_callers >= 1) {
    polar_3bet = false;
  }

  const uint8_t *combos = get_ordering((order)openpos);
  vector<int> hands;

  for (size_t i = 0; i < NUM_COMBOS; ++i) {
    vector<int> x = combo_to_hands(combos[i]);
    hands.insert(hands.end(), x.begin(), x.end());
  }

  for (size_t i = 0; i < NUM_HANDS; ++i) {
    out[i] = {.96, 0.02, 0.02};
  }

  for (size_t i = 0; i < thresh; ++i) {
    out[hands[i]] = {0.02, 0.84, 0.14};
  }

  if (polar_3bet) {
    set_3bet_arange(out, range, open_range, hands, thresh, 0.5, 1.27, true);
  } else {
    set_3bet_arange(out, range, open_range, hands, thresh, 0.5, 1.00, false);
    size_t buffer = CALLOPEN_BUFFER_PCT * NUM_HANDS; // 5% buffer
    size_t buffered_thresh = min(thresh + buffer, (size_t) NUM_HANDS);

    for (size_t i = thresh; i < buffered_thresh; ++i) {
      out[hands[i]] = {0.50, 0.35, 0.15};
    }
  }
}
