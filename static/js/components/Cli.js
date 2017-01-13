import React, { Component, PropTypes } from 'react'
import Model from '../Model'
import { connect } from 'react-redux'
import { setNumPlayers, updateFromServer, resetRanges } from '../actions/index.js'

class Display extends Component {
  constructor() {
    super();
  }

  render() {
    return (
      <div className="display">
        <span>
          <b>Num Players: </b>
          {this.props.model.num_players + " "}
          <b>Blinds: </b>
          {this.props.model.sb + "/" + this.props.model.bb + " "}
          <br />
          <b>Players: </b>
          <br />
          <div className="players">
            {this.props.model.players.map((x, i) =>
              <span key={i}>
                <b>
                  Seat {i}{i == 0 && " (You)"}-{this.props.model.getPlayerStr(i)}:</b>
                <br />
                <span className="smaller">Stack: {x.stack} Label: {x.label} </span><br />
              </span>
            )}
            {this.props.model.num_players % 2 == 1 && <br />}
          </div>

          <br />
          <b>Street: </b>
          {this.props.model.getStreetStr()}
          <br />
          <b>Hole Cards: </b>
          {this.props.model.cards
            ? this.props.model.cards.join(' ')
            : "Not set"}
          <br />
          <b>Pot: </b>
          {this.props.model.pot + this.props.model.active_pot}
          <br />
          <b>Board: </b>
          {this.props.model.board
            ? this.props.model.board.join(' ')
            : "Not set"}
          <br />
          <b>Best Action: </b>
          {/* 'Not your turn' or 'Action and reason' */}
        </span>
      </div>
    )
  }
}

class UnboundCli extends Component {
  constructor () {
    super();
    var history = []
    for (var i = 0; i < 30; ++i) {history.push("")}
    history.push("Enter commands here. Type help for list of commands.")
    this.state = {
      history: history
    };
    this.shouldScrollBottom = true;
  }

  scrollBot() {
    this.output.scrollTop = this.output.scrollHeight;
  }

  componentWillMount() {
    this.model = new Model(this.props.updateFromServer, this.props.resetRanges);
  }

  componentDidMount() {
    this.scrollBot();
  }
   
  componentDidUpdate() {
    if (this.shouldScrollBottom) {
      this.scrollBot();
      this.shouldScrollBottom = false;
    }
  }

  setSettings(tokens) {
    var i = 0;
    var text = "";
    while (i < tokens.length) {
      switch (tokens[i]) {
        case 'b': 
        case 'blinds': 
          var sb = parseInt(tokens[i+1])
          var bb = parseInt(tokens[i+2])
          if (this.model.setBlinds(sb, bb)) {
            text += "Set blinds to " + sb + "/" + bb + ". ";
          } else {
            text += "Did not set blinds. ";
          }
          i += 3;
          break;
        case 'num':
        case 'numplayers':
					var num_players = parseInt(tokens[i+1]);
          if (this.model.setNumPlayers(num_players)) {
            text += "Set num players to " + num_players + ". ";
          } else {
            text += "Did not set num players. ";
          }
          this.props.setNumPlayers(num_players);
          i += 2;
          break;
        case 'new':
        case 'newplayer':
				  var ind = parseInt(tokens[i+1]);
          var stack = parseInt(tokens[i+2]);
          var label = tokens[i+3];
          if (this.model.addPlayer(ind, stack, label)) {
            text += "Added new player to slot " + ind + " with stack " +
              stack + " and label '" + label +  "' . ";
          } else {
            text += "Did not add new player. ";
          }
          this.props.setNumPlayers(this.model.num_players);
          i += 4;
          break;
        case 's':
        case 'stack':
          var ind = parseInt(tokens[i+1]);
          var stack = parseInt(tokens[i+2]);
          if (this.model.setPlayerStack(ind, stack)) {
            text += "Update player " + ind + " stack to " + stack + ". ";
          } else {
            text += "Did not update player. ";
          }
          i += 3;
          break;
        case 'l':
        case 'label':
          var ind = parseInt(tokens[i+1]);
          var label = tokens[i+2];
          if (this.model.setPlayerLabel(ind, label)) {
            text += "Update player " + ind + " label to " + label + ". ";
          } else {
            text += "Did not update player. ";
          }
          i += 3;
          break;
        default:
          text += "Did not understand '" + tokens[i] + "'. ";
          i += 1;
          break;
      }
    }
    return text;
  }

  onSubmit(e) {
    if (e.key !== 'Enter') {
      return;
    }
    var cmd = this.inp.value.toLowerCase();
    var tokens = cmd.split(' ');

    var text = '"' + cmd + '" is not a valid action.'
    switch (tokens[0]) {
      case 'board':
      case 'b':
        var cards = tokens.slice(1)
          .map((x) => x[0].toUpperCase() + x[1].toLowerCase());
        text = this.model.addBoard(cards)
        break;
      case 'pos':
      case 'p':
        var posStr = tokens[1].toLowerCase();
        if (this.model.setPosition(posStr)) {
          text = "Set your position to " + posStr.toUpperCase() + ". ";
        } else {
          text = "Position " + posStr + " not recognized. "
        }
        break;
      case 'newhand':
      case 'n':
        if (tokens.length == 3) {
					var cards = tokens.slice(1)
						.map((x) => x[0].toUpperCase() + x[1].toLowerCase());
					text = this.model.newHand(cards);
				} else {
					text = "Need to input valid hole cards. "
				}
        break;
      case 'undo':
      case 'u':
        text = this.undo()
        break;
      case 'set':
      case 's':
        text = this.setSettings(tokens.slice(1))
    }

    if ((tokens[0].startsWith('b') && tokens[0].length > 1)
      || tokens[0].startsWith('r')
      || ['c', 'f'].includes(tokens[0])) {
      text = this.model.addActions(tokens);
    }

    this.state.history.push(text);
    this.setState({
      history: this.state.history
    });
    this.inp.value = "";
    this.shouldScrollBottom = true;
  }

  render() {
    return (
      <div 
        className="cli"
        onClick={() => {this.inp.focus()}}
      >
        <Display model={this.model} />
				<div
          className="output"
          ref={(e) => {this.output = e}}
        >
          {this.state.history.map((x, i) => {
            return (
              <span key={i}>
                {x} <br/>
              </span>
            )
          })}
        </div>
        <div className="input">
          <input 
            type="text"
            ref={(e) => {this.inp = e}}
            onKeyPress={(e) => this.onSubmit(e)}
          />
        </div>
      </div>
    )
  }
}

const mapStateToProps = (state, ownProps) => {
  return {
		// propName: state.prop
  }
}

const mapDispatchToProps = (dispatch, ownProps) => {
  return {
    // propName: () => {dispatch(action(x))}
    updateFromServer: (x) => {dispatch(updateFromServer(x))},
    resetRanges: (x) => {dispatch(resetRanges(x))},
    setNumPlayers: (x) => {dispatch(setNumPlayers(x))}
  }
}

const Cli = connect(
  mapStateToProps,
  mapDispatchToProps
)(UnboundCli)

export default Cli
