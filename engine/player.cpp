#include "./player.h"

Player::Player(string label, int pos, int ind, int card1, int card2) {
  this->label = label;
  this->pos = pos;
  this->ind = ind;
  this->range = new double[NUM_HANDS];
  this->range_c = new double[NUM_COMBOS];
  set_starting_range(this->range);
  if (ind != 0) {
    set_blockers(this->range, card1, card2);
  }
  convert_to_combos(this->range, this->range_c);
  this->arange = new struct act[NUM_HANDS];
  this->arange_c = new struct act[NUM_COMBOS];
}

void Player::think(const vector<Player> &players, const gamestate *g) {
  if (g->street == "p") {
    if (g->is_open) {
      set_open_arange(this->arange, (position)this->pos, g->num_limpers);
    } else {
      set_default_arange(this->arange);
    }
  } else {
    set_default_arange(this->arange);
  }

  convert_to_combos_a(this->arange, this->arange_c);
}

void Player::move(const vector<Player> &players, const gamestate *g, char action, int bet) {
  bayes_apply(this->range, this->arange, action);
  convert_to_combos(this->range, this->range_c);
}
