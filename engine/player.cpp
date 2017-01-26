#include "./player.h"

Player::Player(string label, int pos, int ind, int card1, int card2) {
  this->label = label;
  this->pos = (position)pos;
  this->ind = ind;
  this->range = new double[NUM_HANDS];
  this->range_c = new double[NUM_COMBOS];
  this->me_card1 = card1;
  this->me_card2 = card2;
  set_starting_range(this->range);
  if (ind == 0) {
    convert_to_combos(this->range, this->range_c);
  } else {
    convert_to_combos_with_blockers(this->range, this->range_c, card1, card2);
  }
  this->arange = new struct act[NUM_HANDS];
  this->arange_c = new struct act[NUM_COMBOS];
}

void Player::think(const vector<Player> &players, const gamestate *g) {
  if (g->street == "p") {
    if (g->num_raises == 0) {
      set_open_arange(this->arange, this->pos, g->num_limpers);
    } else if (g->num_raises == 1) {
      set_oneraise_arange(this->arange, this->range, players[g->open_ind].range,
          this->pos, players[g->open_ind].pos, g->num_callers);
    } else if (g->num_raises == 2) {
      set_default_arange(this->arange);
    } else {
      set_default_arange(this->arange);
    }
  } else {
    if (g->num_raises == 0) {
      set_flop_noraise_arange(this->arange, this->pos, players, g);
    } else if (g->num_raises == 1) {
      set_default_arange(this->arange);
    } else {
      set_default_arange(this->arange);
    }
  }

  convert_to_combos_a(this->arange, this->arange_c);
}

void Player::move(const vector<Player> &players, const gamestate *g, char action, int bet) {
  bayes_apply(this->range, this->arange, action);
  if (this->ind == 0) {
    convert_to_combos(this->range, this->range_c);
  } else {
    convert_to_combos_with_blockers(this->range, this->range_c, this->me_card1, this->me_card2);
  }
}
