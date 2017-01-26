#include <stdbool.h>

#include "./cards.h"
#include "./params.h"
#include "./constants.h"
#include "./player.h"

#define FLOP_POS(a, b) ((int)(a) + (b) + 2) % (b)

void set_check_all(arange_t arange) {
  for (int i = 0; i < NUM_HANDS; ++i) {
    arange[i] = {0., 1.00, 0.00};
  }
}

string get_wetness_str(double wetness) {
  if (wetness > 4) return "Soaking wet board.";
  if (wetness > 3) return "Wet board.";
  if (wetness > 2) return "Semi-wet board.";
  if (wetness > 1) return "Dry board.";
  return "Bone dry board.";
}

void set_flop_noraise_arange(arange_t out, position pos,
    const vector<Player> &players, const gamestate *g) {
  int np = g->num_players;
  position aggressor_pos = players[g->aggressor_ind].pos;
  position caller_pos = players[g->caller_ind].pos;
  if (g->heads_up) {
    if (aggressor_pos == pos) {
      // preflop aggressor
      COMMENT("Preflop aggressor.");
      double wetness = board_texture(players[g->caller_ind].range, g->board, 3); 
      int in_pos = FLOP_POS(pos, np) > FLOP_POS(caller_pos, np);

      COMMENT(get_wetness_str(wetness));
      COMMENT("Wetness: " << wetness);
      int a = max(players[0].me_card1, players[0].me_card2);
      int b = min(players[0].me_card1, players[0].me_card2);
      int me_hand = HAND(a, b);

      for (int i = 0; i < NUM_HANDS; ++i) {
        standing res = get_standings_vs_range(players[g->caller_ind].range, i, g->board, 3);
        out[i] = {0., 1 - res.equity, res.equity};
        if (i == me_hand) {
          COMMENT("Equity: " << res.equity);
          COMMENT("SDV: " << res.sdv);
          COMMENT("Vulnerability: " << res.vulnerability);
        }
      }
    } else {
      if (FLOP_POS(pos, np) < FLOP_POS(aggressor_pos, np)) {
        COMMENT("Obligatory check.");
        // obligatory check
        set_check_all(out);
      } else { // pf aggressor checks.

      }
    }
  }
}
