#include "./cards.h"
#include "./player.h"

#include <stdbool.h>

bool init = false;
bool noprint = false;
vector<Player> players;

void print_arange(Player &player, int turn) {
  if (noprint) {
    return;
  }
  printf("arange:%d,", turn);
  for (int i = 0; i < NUM_COMBOS; ++i) {
    printf("%0.3f,%0.3f,%0.3f,",
        player.arange_c[i].fold,
        player.arange_c[i].call,
        player.arange_c[i].raise);
  }
  printf(";");
}

void pretty_print_arange(Player &player) {
  printf("Pretty Print\n");
  for (int i = 0; i < NUM_COMBOS; ++i) {
    if (player.arange_c[i].raise > 0.00001) {
      cout << combo_to_str(i) << " ";
    } else {
      cout << "   ";
    }

    if (i % 13 == 12) {
      cout << "\n";
    }
  }
  cout << "\n";
}

void print_range(Player &player, int ind) {
  if (noprint) {
    return;
  }
  printf("range:%d,", ind);
  for (int i = 0; i < NUM_COMBOS; ++i) {
    printf("%0.3f,", player.range_c[i]);
  }
  printf(";");
}

void pretty_print_range(Player &player) {
  printf("Pretty Print\n");
  for (int i = 0; i < NUM_COMBOS; ++i) {
    if (player.range_c[i] > 1. / 169) {
      cout << combo_to_str(i) << " ";
    } else {
      cout << "   ";
    }

    if (i % 13 == 12) {
      cout << "\n";
    }
  }
  cout << "\n";
}

void initialize(json &state, gamestate *g) {
  const vector<string> &labels = state["labels"];
  int num_players = state["num_players"];
  int pos = state["pos"];
  int card1 = state["cards"][0];
  int card2 = state["cards"][1];

  for (int i = 0; i < num_players; ++i) {
    int cur_pos = (pos + i) % num_players;
    Player player(labels[i], cur_pos, i, card1, card2);
    players.push_back(player);
    print_range(player, i);
  }

  g->is_open = true;
}

void think(gamestate *g) {
  Player &player = players[g->turn];
  player.think(players, g);
  print_arange(player, g->turn);
}

void move(gamestate *g, string last_action, int last_bet, int last_turn) {
  Player &player = players[last_turn];
  player.move(players, g, last_action[0], last_bet);
  print_range(player, last_turn);
}

int num_board_cards(string street) {
  if (street == "p") return 0;
  if (street == "f") return 3;
  if (street == "t") return 4;
  return 5;
}

void update_gamestate(gamestate *g, json &state) {
  g->num_players = state["num_players"];
  g->turn = state["turn"];
  g->street = state["street"];
  g->pot = state["pot"];
  g->active_pot = state["active_pot"];
  g->stacks = new int[g->num_players];
  g->cur_bets = new int[g->num_players];
  g->num_actions = state["actions"].size();
  g->actions = new char[g->num_actions];
  g->bets = new int[g->num_actions];
  g->board = new string[5];
  g->num_board_cards = num_board_cards(g->street);
  g->num_limpers = 0;

  const vector<int> &stacks = state["stacks"];
  const vector<int> &cur_bets = state["cur_bets"];
  const vector<string> &actions = state["actions"];
  const vector<string> &board = state["board"];
  for (int i = 0; i < g->num_players; ++i) {
    g->stacks[i] = stacks[i];
    g->cur_bets[i] = cur_bets[i];
  }

  for (int i = 0; i < g->num_actions; ++i) {
    if (actions[i].length() > 1) {
      string action = actions[i];
      g->bets[i] = stoi(action.substr(1));
    }
    g->actions[i] = actions[i][0];
    g->bets[i] = 0;
  }

  for (int i = 0; i < g->num_board_cards; ++i) {
    g->board[i] = board[i];
  }

  if (g->is_open && g->street == "p" && g->actions[g->num_actions-1] == 'c') {
    g->num_limpers++;
  }

  if (g->is_open && g->street == "p" && g->actions[g->num_actions-1] == 'r') {
    g->is_open = false;
  }
}

void free_gamestate(gamestate *g) {
  delete[] g->stacks;
  delete[] g->actions;
  delete[] g->board;
}

int main(int argc, char *argv[]) {
  gamestate g;
  int last_turn;
  if (argc > 1 && strcmp(argv[1], "-np") == 0) {
    noprint = true;
  }
  while (true) {
    string inp;
    cin >> inp;
    json state = json::parse(inp);

    if (!noprint) printf("START ");
    if (!init) {
      initialize(state, &g);
      init = true;
    } else {
      move(&g, state["last_action"], (int)state["last_bet"], last_turn);
      free_gamestate(&g);
    }

    update_gamestate(&g, state);
    think(&g);
    last_turn = g.turn;
    if (!noprint) printf(" END\n");
  }
}
