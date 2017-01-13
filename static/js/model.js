// Model:
// - players: list of Players
// - cards: ['Ac', '7d']
// - board: ['5d', '7h', 'Kh']
// - action: ['r5', 'c', 'f']
// - pos: 0 (UTG), 5 (BU)
// - street: 'p', 'f', 't', 'r'
// - turn: 0 (UTG), 5 (BU)
// - pot: 35
// - blinds: [1, 2]

// Player:
// - stack: 125
// - order: 0 (me), 1 (left of me)
// - label: 
import $ from 'jquery';
import { CARD_STR_TO_NUM } from './constants';

class Player {
  constructor (stack, label) {
    this.stack = stack;
    this.label = label;
  }
}

class Model {
  constructor (updateFromServer, resetRanges) {
    this.players = [];
    this.num_players = 0;
    this.sb = 1;
    this.bb = 2;
    this.actions = [];
    this.street = 'p';
    this.setPosition('utg');
    this.board = [];
    this.setNumPlayers(6);
    this.cards = [];
    this.pot = 0; this.active_pot = 0;
    this.updateFromServer = updateFromServer;
    this.resetRanges = resetRanges;
  }

  setBlinds(sb, bb) {
    if (sb > bb) { return false; }
    this.sb = sb;
    this.bb = bb;

    return true;
  }

  addBoard(board) {
    var newLength = this.board.length + board.length;
    if (newLength < 3 || newLength > 5) return false;
    board = board.map((x) => x[0].toUpperCase() + x[1]);
    this.board.push(...board);

    var text = "Set board to " + this.board.join(" ") + ". ";
    return text;
  }

  posStrToNum(posStr) {
    switch (posStr) {
      case 'bb':
        return this.num_players - 1;
      case 'sb':
        return this.num_players - 2;
      case 'bu':
        return this.num_players - 3;
      case 'co':
        return this.num_players - 4;
      case 'mid':
        return this.num_players - 5;
      default:
        return 0; break;
    }
  }

  getPlayerStr(player) {
    const numToStr = {
      1: 'BB',
      2: 'SB',
      3: 'BU',
      4: 'CO',
      5: 'MID',
      6: 'UTG'
    }
    var x = (player + this.pos) % this.num_players;
    return numToStr[this.num_players - x];
  }

  posNumToPlayer(posNum) {
    return (2 * this.num_players - this.pos + posNum) % this.num_players;
  }

  setPosition(posStr) {
    if (!['bb', 'sb', 'bu', 'co', 'mid', 'utg'].includes(posStr)) return false;
    this.pos = this.posStrToNum(posStr);
    return true;
  }

  addPlayer(order, stack, label) {
    if (this.num_players == 6) return false;
    var player = new Player(stack, label);
    this.players.splice(order, 0, player);
    this.num_players += 1;

    return true;
  }

  setPlayerStack(ind, stack) {
		if (ind >= this.players.length) return false;
    this.players[ind].stack = stack;
    return true;
  }

  setPlayerLabel(ind, label) {
		if (ind >= this.players.length) return false;
    this.players[ind].label = label;
    return true;
  }

  setNumPlayers(num_players) {
    if (num_players > 6) return false;
    this.num_players = num_players;
    this.players = [];
    for (var i = 0; i < num_players; ++i) {
      var player = new Player(100 * this.bb, "reg");
      this.players.push(player);
    }

    return true;
  }

  newHand(cards, incrPos=false, adjust_pot=false) {
    if (!adjust_pot && typeof this.start_stacks !== 'undefined') {
      for (var i = 0; i < this.num_players; ++i) {
        this.players[i].stack = this.start_stacks[i];
      }
    }

    this.pot = 0;
    this.board = [];
    this.cards = cards.map((x) => x[0].toUpperCase() + x[1]);
    this.street = 'p';
    if (incrPos) {
      this.pos = (this.pos + 1) % this.num_players;
    }
    this.turn = this.posNumToPlayer(0);
    this.actions = [];

    this.live_players = [];
    this.cur_bets = [];
    this.start_stacks = [];
    for (var i = 0; i < this.num_players; ++i) {
      this.live_players.push(true);
      this.cur_bets.push(0);
      this.start_stacks.push(this.players[i].stack);
    }
    var bb_player = this.posNumToPlayer(-1);
    var sb_player = this.posNumToPlayer(-2);
    this.cur_bets[bb_player] = this.bb;
    this.cur_bets[sb_player] = this.sb;
    this.players[bb_player].stack -= this.bb;
    this.players[sb_player].stack -= this.sb;
    this.min_bet = this.bb;

    this.num_live = this.num_players;
    this.acts_since_raise = 0;
    this.active_pot = this.bb + this.sb;

    var text = "Set new hand with hole cards: " + this.cards.join(" ")  + ". ";
    text += this.queryIfCan(true);

    return text;
  }

  setNextLive(incr=false) {
    if (incr) {
      this.turn = (this.turn + 1) % this.num_players;
    }

    while (!this.live_players[this.turn]) {
      this.turn = (this.turn + 1) % this.num_players;
    }
  }

  setNextStreet() {
    switch(this.street) {
      case 'p': this.street = 'f'; break;
      case 'f': this.street = 't'; break;
      case 't': this.street = 'r'; break;
    }
  }

  getCurPlayerStr() {
    return this.getPlayerStr(this.turn);
  }

  getStreetStr() {
    const streetToStr = {
      p: 'Preflop',
      f: 'Flop',
      t: 'Turn',
      r: 'River'
    };
    return streetToStr[this.street];
  }

  addActions(actions, adjust_pot=false) {
    if (this.num_live <= 1) {
      return "Hand already ended.";
    }

    if (this.cards.length == 0) {
      return "No hole cards added.";
    }

    if ((this.street == 'f' && this.board.length < 3)
      || (this.street == 't' && this.board.length < 4)
      || (this.street == 'r' && this.board.length < 5)) {
      var text = "Fill in board. ";
      return text;
    }

    var text = "";
    for (var action of actions) {
      if (action == 'f') {
        this.live_players[this.turn] = false;
        --this.num_live;
        text += this.getCurPlayerStr() + " folds. "
        if (this.num_live == 1) {
          if (adjust_pot) {
            this.setNextLive();
            this.pot += this.active_pot;
            this.active_pot = 0;
            this.players[this.turn].stack += this.pot;
          } else {
            for (var i = 0; i < this.num_players; ++i) {
              this.players[i].stack = this.start_stacks[i];
            }
          }
          text += "Hand ends. "
          return text;
        }
        this.setNextLive();
        this.actions.push(action);
      } else if (action == 'c') {
        var action = this.min_bet == this.cur_bets[this.turn] ? "checks" : "calls";
        this.players[this.turn].stack -= this.min_bet - this.cur_bets[this.turn];
        this.active_pot += this.min_bet - this.cur_bets[this.turn];
        this.cur_bets[this.turn] = this.min_bet;

        text += this.getCurPlayerStr() + " " + action + ". "
        this.setNextLive(true);
        ++this.acts_since_raise;
        this.actions.push(action == "checks" ? 'h' : 'c');
      } else if (action.startsWith('b') || action.startsWith('r')) {
        var amt = parseInt(action.slice(1));
        var action = this.min_bet != 0 ? "raises to" : "bets";
        text += this.getCurPlayerStr() + " " + action + " " + amt + ". ";
        this.players[this.turn].stack -= amt - this.cur_bets[this.turn];
        this.active_pot += amt - this.min_bet;
        this.min_bet = amt;
        this.cur_bets[this.turn] = amt;

        this.setNextLive(true);
        this.acts_since_raise = 1;
        this.actions.push((action == "bets" ? 'b' : 'r') + amt);
      } else {
        text += "Action is invalid. ";
        continue;
      }

      if (this.acts_since_raise == this.num_live) {
        if (['p', 'f', 't'].includes(this.street)) {
          this.pot += this.active_pot;
          this.active_pot = 0;
          for (var i = 0; i < this.num_players; ++i) {
            this.cur_bets[i] = 0;
          }

          this.acts_since_raise = 0;
          this.turn = this.posNumToPlayer(-2);
          this.min_bet = 0;
          this.setNextLive();
          this.setNextStreet();
          text += "Next street is " + this.getStreetStr() + ". ";
          if ((this.street == 'f' && this.board.length < 3)
            || (this.street == 't' && this.board.length < 4)
            || (this.street == 'r' && this.board.length < 5)) {
            text += "Fill in board. ";
            break;
          }
        } else {
          this.num_live = 0;
          if (!adjust_pot) {
            for (var i = 0; i < this.num_players; ++i) {
              this.players[i].stack = this.start_stacks[i];
            }
          }
          text += "Showdown. ";
        }
      }

      text += this.queryIfCan();
    }

    return text;
  }

  setShowdown() {
    return true;
  }

  queryIfCan(is_new_hand=false) {
    var last_action = this.actions[this.actions.length-1];
    var last_bet = 0;
    if (this.actions.length > 0 && (last_action.startsWith('b') || last_action.startsWith('r'))) {
      last_bet = parseFloat(last_action.slice(1));
      last_action = last_action[0];
    }


    var data = {
      street: this.street,
      cards: this.cards.map(x => CARD_STR_TO_NUM[x]),
      board: this.board.map(x => CARD_STR_TO_NUM[x]),
      actions: this.actions,
      labels: this.players.map((x) => x.label),
      stacks: this.players.map((x) => x.stack),
      pos: this.pos,
      turn: this.turn,
      pot: this.pot,
      active_pot: this.active_pot,
      cur_bets: this.cur_bets,
      num_players: this.num_players,
      last_action: last_action,
      last_bet: last_bet,
    }

    var payload = {
      data: JSON.stringify(data),
      is_new_hand: is_new_hand
    }

    $.post('/api/game_state/', payload, (data) => {
      if (is_new_hand) {
        this.resetRanges();
      }
      this.updateFromServer(data.text);
    });

    return ""
  }
}

export default Model
