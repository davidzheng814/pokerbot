import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import _ from 'underscore'

const HANDS = [
 ['AA', 'AKs', 'AQs', 'AJs', 'ATs', 'A9s', 'A8s', 'A7s', 'A6s', 'A5s', 'A4s', 'A3s', 'A2s'],
 ['AKo', 'KK', 'KQs', 'KJs', 'KTs', 'K9s', 'K8s', 'K7s', 'K6s', 'K5s', 'K4s', 'K3s', 'K2s'],
 ['AQo', 'KQo', 'QQ', 'QJs', 'QTs', 'Q9s', 'Q8s', 'Q7s', 'Q6s', 'Q5s', 'Q4s', 'Q3s', 'Q2s'],
 ['AJo', 'KJo', 'QJo', 'JJ', 'JTs', 'J9s', 'J8s', 'J7s', 'J6s', 'J5s', 'J4s', 'J3s', 'J2s'],
 ['ATo', 'KTo', 'QTo', 'JTo', 'TT', 'T9s', 'T8s', 'T7s', 'T6s', 'T5s', 'T4s', 'T3s', 'T2s'],
 ['A9o', 'K9o', 'Q9o', 'J9o', 'T9o', '99', '98s', '97s', '96s', '95s', '94s', '93s', '92s'],
 ['A8o', 'K8o', 'Q8o', 'J8o', 'T8o', '98o', '88', '87s', '86s', '85s', '84s', '83s', '82s'],
 ['A7o', 'K7o', 'Q7o', 'J7o', 'T7o', '97o', '87o', '77', '76s', '75s', '74s', '73s', '72s'],
 ['A6o', 'K6o', 'Q6o', 'J6o', 'T6o', '96o', '86o', '76o', '66', '65s', '64s', '63s', '62s'],
 ['A5o', 'K5o', 'Q5o', 'J5o', 'T5o', '95o', '85o', '75o', '65o', '55', '54s', '53s', '52s'],
 ['A4o', 'K4o', 'Q4o', 'J4o', 'T4o', '94o', '84o', '74o', '64o', '54o', '44', '43s', '42s'],
 ['A3o', 'K3o', 'Q3o', 'J3o', 'T3o', '93o', '83o', '73o', '63o', '53o', '43o', '33', '32s'],
 ['A2o', 'K2o', 'Q2o', 'J2o', 'T2o', '92o', '82o', '72o', '62o', '52o', '42o', '32o', '22']
]


// Assume this.props.ranges is List[169] of Percentages
class RangeTable extends Component {
  constructor() {
    super();
    this.state = {
      show_arange: true
    };
  }
  
  chunk(x) {
    var y = [];
    for (var i = 0; i < x.length; i += 13) {
      y.push(x.slice(i, i + 13));
    }
    return y;
  }

  elt(pct, val) {
    var hold_width = (pct < 0.0001) ? 0 : Math.round(25 * (9.2 + Math.log(pct)) / 9.2);
    return (
      <div className="range-elt">
        <div className="centered"><div className="centered-child">{val}</div></div>
        <div className="back hold" style={{width: hold_width + 'px'}}></div>
        <div className="back fold" style={{width: (25 - hold_width) + 'px'}}></div>
      </div>
    )
  }

  aelt(pcts, val) {
    var fold_width = Math.round(pcts[0] * 25);
    var call_width = Math.round(pcts[1] * 25);

    return (
      <div className="range-elt">
        <div className="centered"><div className="centered-child">{val}</div></div>
        <div className="back raise" style={{width: (25 - fold_width - call_width) + 'px'}}></div>
        <div className="back call" style={{width: call_width + 'px'}}></div>
        <div className="back fold" style={{width: fold_width + 'px'}}></div>
      </div>
    )
  }

  toggleShowArange() {
    this.setState({show_arange: !this.state.show_arange});
  }

  componentWillMount() {
    this.setState({show_arange: this.props.ind == 0});
  }

  render() {
    if ((this.state.show_arange && this.props.arange == null) ||
        (!this.state.show_arange && this.props.range == null)) {
      var table = (
        <div className="empty-box" />
      );
    } else {
      var table = (
        <table className="rangeTable">
          <tbody>
            {this.state.show_arange &&
             this.chunk(this.props.arange).map((x, i) => 
              <tr key={i}>
                {x.map((y, j) =>
                  <td key={j}>
                    {this.aelt(y, HANDS[i][j])}
                  </td>
                )}
              </tr>
            )}
            {!this.state.show_arange &&
             this.chunk(this.props.range).map((x, i) => 
              <tr key={i}>
                {x.map((y, j) =>
                  <td key={j}>
                    {this.elt(y, HANDS[i][j])}
                  </td>
                )}
              </tr>
            )}
          </tbody>
        </table>
      );
    }

    return (
      <div className="range-group">
        <div>
          Seat {this.props.ind}
        </div>
        {table}
        <form>
          <div className="radio">
            <label>
              <input
                type="radio"
                value="action range" 
                checked={this.state.show_arange} 
                onChange={() => this.toggleShowArange()} />
              Action Range
            </label>
          </div>
          <div className="radio">
            <label>
              <input
                type="radio"
                value="holding range"
                checked={!this.state.show_arange} 
                onChange={() => this.toggleShowArange()} />
              Holding Range
            </label>
          </div>
        </form>
      </div>
    );
  }
}

class UnboundRangeView extends Component {
  render() {
    console.log("Updating Ranges.");
    return (
      <div className="range-view">
        {_.range(this.props.ranges.length).map((i) => {
          return (
            <RangeTable key={i} range={this.props.ranges[i]} arange={this.props.aranges[i]} ind={i} />
          );
        })}
      </div>
    )
  }
}

const mapStateToProps = (state, ownProps) => {
  return {
    // propName: state.prop
    ranges: state.ranges,
    aranges: state.aranges
  }
}

const mapDispatchToProps = (dispatch, ownProps) => {
  return {
		// propName: () => {dispatch(action(x))}
  }
}

const RangeView = connect(
  mapStateToProps,
  mapDispatchToProps
)(UnboundRangeView)

export default RangeView

