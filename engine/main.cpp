#include "./cards.h"
#include "./player.h"
#include "./params.h"

#include <iostream>
#include <fstream>

#include <stdbool.h>

bool init = false;
bool noprint = false;
vector<Player> players;

double FISH_POS_WT = 0;
double LIGHT_POS_WT = 0;
double UTG_OPEN_PCT = 0;
double UTG_OPEN_LOOSE_INC = 0;
double UTG_OPEN_TIGHT_INC = 0;
double HJ_OPEN_PCT = 0;
double HJ_OPEN_LOOSE_INC = 0;
double HJ_OPEN_TIGHT_INC = 0;
double CO_OPEN_PCT = 0;
double CO_OPEN_LOOSE_INC = 0;
double CO_OPEN_TIGHT_INC = 0;
double BU_OPEN_PCT = 0;
double BU_OPEN_LOOSE_INC = 0;
double BU_OPEN_TIGHT_INC = 0;
double SB_OPEN_PCT = 0;
double SB_OPEN_LOOSE_INC = 0;
double SB_OPEN_TIGHT_INC = 0;
double OPEN_BUFFER_PCT = 0;
double HJ_UTG_CALLOPEN_PCT = 0;
double CO_UTG_CALLOPEN_PCT = 0;
double BU_UTG_CALLOPEN_PCT = 0;
double SB_UTG_CALLOPEN_PCT = 0;
double BB_UTG_CALLOPEN_PCT = 0;
double CO_HJ_CALLOPEN_PCT = 0;
double BU_HJ_CALLOPEN_PCT = 0;
double SB_HJ_CALLOPEN_PCT = 0;
double BB_HJ_CALLOPEN_PCT = 0;
double BU_CO_CALLOPEN_PCT = 0;
double SB_CO_CALLOPEN_PCT = 0;
double BB_CO_CALLOPEN_PCT = 0;
double SB_BU_CALLOPEN_PCT = 0;
double BB_BU_CALLOPEN_PCT = 0;
double BB_SB_CALLOPEN_PCT = 0;
double CALLOPEN_BUFFER_PCT = 0;
double BONUS_3BET = 0;

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
  g->num_raises = 0;
  g->num_callers = 0;
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
  g->last_turn = state["last_turn"];

  string last_street = g->street;
  g->street = state["street"];
  bool is_new_flop = false;
  if (last_street == "p" && g->street == "f") {
    is_new_flop = true;
  }

  g->pot = state["pot"];
  g->active_pot = state["active_pot"];
  g->stacks = new int[g->num_players];
  g->cur_bets = new int[g->num_players];
  g->num_actions = state["actions"].size();
  g->actions = new char[g->num_actions];
  g->bets = new int[g->num_actions];
  g->board = new int[5];
  g->num_board_cards = num_board_cards(g->street);
  g->num_limpers = 0;

  const vector<int> &stacks = state["stacks"];
  const vector<int> &cur_bets = state["cur_bets"];
  const vector<string> &actions = state["actions"];
  const vector<int> &board = state["board"];
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

  if (g->num_actions == 0) return;
  char last_action = g->actions[g->num_actions-1];

  if (g->num_raises == 0 && g->street == "p" && last_action == 'c') {
    g->num_limpers++;
  }

  if (last_action == 'r' && g->num_raises == 0 && g->street == "p") {
    g->open_ind = g->last_turn;
  }

  if (is_new_flop) {
    g->num_raises = 0;
    g->heads_up = g->num_callers == 1;
    if (g->heads_up) {
      
    }
    g->num_callers = 0;
  }

  if (last_action == 'r') {
    g->num_raises++;
    g->num_callers = 0;
    if (g->street == "p") {
      g->aggressor_ind = g->last_turn;
    }
  }

  if (last_action == 'c') {
    g->num_callers++;
    if (g->street == "p") {
      g->caller_ind = g->last_turn;
    }
  }
}

void free_gamestate(gamestate *g) {
  delete[] g->stacks;
  delete[] g->actions;
  delete[] g->board;
}

struct param {
  double *ref;
  double val;
};

int main(int argc, char *argv[]) {
  gamestate g;
  int last_turn;

  if (argc > 1 && strcmp(argv[1], "-np") == 0) {
    noprint = true;
  }

  map<string, double *> param_to_ref = {
    {"FISH_POS_WT", &FISH_POS_WT},
    {"LIGHT_POS_WT", &LIGHT_POS_WT},
    {"UTG_OPEN_PCT", &UTG_OPEN_PCT},
    {"UTG_OPEN_LOOSE_INC", &UTG_OPEN_LOOSE_INC},
    {"UTG_OPEN_TIGHT_INC", &UTG_OPEN_TIGHT_INC},
    {"HJ_OPEN_PCT", &HJ_OPEN_PCT},
    {"HJ_OPEN_LOOSE_INC", &HJ_OPEN_LOOSE_INC},
    {"HJ_OPEN_TIGHT_INC", &HJ_OPEN_TIGHT_INC},
    {"CO_OPEN_PCT", &CO_OPEN_PCT},
    {"CO_OPEN_LOOSE_INC", &CO_OPEN_LOOSE_INC},
    {"CO_OPEN_TIGHT_INC", &CO_OPEN_TIGHT_INC},
    {"BU_OPEN_PCT", &BU_OPEN_PCT},
    {"BU_OPEN_LOOSE_INC", &BU_OPEN_LOOSE_INC},
    {"BU_OPEN_TIGHT_INC", &BU_OPEN_TIGHT_INC},
    {"SB_OPEN_PCT", &SB_OPEN_PCT},
    {"SB_OPEN_LOOSE_INC", &SB_OPEN_LOOSE_INC},
    {"SB_OPEN_TIGHT_INC", &SB_OPEN_TIGHT_INC},
    {"OPEN_BUFFER_PCT", &OPEN_BUFFER_PCT},
    {"HJ_UTG_CALLOPEN_PCT", &HJ_UTG_CALLOPEN_PCT},
    {"CO_UTG_CALLOPEN_PCT", &CO_UTG_CALLOPEN_PCT},
    {"BU_UTG_CALLOPEN_PCT", &BU_UTG_CALLOPEN_PCT},
    {"SB_UTG_CALLOPEN_PCT", &SB_UTG_CALLOPEN_PCT},
    {"BB_UTG_CALLOPEN_PCT", &BB_UTG_CALLOPEN_PCT},
    {"CO_HJ_CALLOPEN_PCT", &CO_HJ_CALLOPEN_PCT},
    {"BU_HJ_CALLOPEN_PCT", &BU_HJ_CALLOPEN_PCT},
    {"SB_HJ_CALLOPEN_PCT", &SB_HJ_CALLOPEN_PCT},
    {"BB_HJ_CALLOPEN_PCT", &BB_HJ_CALLOPEN_PCT},
    {"BU_CO_CALLOPEN_PCT", &BU_CO_CALLOPEN_PCT},
    {"SB_CO_CALLOPEN_PCT", &SB_CO_CALLOPEN_PCT},
    {"BB_CO_CALLOPEN_PCT", &BB_CO_CALLOPEN_PCT},
    {"SB_BU_CALLOPEN_PCT", &SB_BU_CALLOPEN_PCT},
    {"BB_BU_CALLOPEN_PCT", &BB_BU_CALLOPEN_PCT},
    {"BB_SB_CALLOPEN_PCT", &BB_SB_CALLOPEN_PCT},
    {"CALLOPEN_BUFFER_PCT", &CALLOPEN_BUFFER_PCT},
    {"BONUS_3BET", &BONUS_3BET},
  };

  char buffer[50] = "params/params0.txt";
  if (argc > 1 && strcmp(argv[1], "--params") == 0) {
    strcpy(buffer, argv[2]);
  }
  
  ifstream params_file;
  params_file.open(buffer, ios::out);

	string line;
	while (getline(params_file, line)) {
	 std::istringstream iss(line);
	 string name;
	 double val;
	 if (!(iss >> name >> val)) { break; }
   *(param_to_ref[name]) = val;
	}

  params_file.close();

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
