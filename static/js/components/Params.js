import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import { setParamsInd } from '../actions/index'
import $ from 'jquery'

class UnboundParamsView extends Component{
  constructor () {
    super();
    this.state = {
      ind: null,
      params: null,
      pind: 0,
      inp: null
    }
  }

  getParams(x) {
    var payload = {params_ind: x};
    $.get('/api/params/', payload, (data) => {
      if (!data) {
        console.log("Could not find params.");
        return;
      }
      console.log("Got new params.");
      this.props.setParamsInd(x);
      this.setState({params: data, inp: data[0][1]});
    });
  }

  createNewParams() {
    $.post('/api/params/', (data) => {
      this.getParams(parseInt(data));
    });
  }

  setParams() {
    var ind = parseInt(this.select_inp.value);
    console.log(ind);
    this.getParams(ind);
  }

  updateParams() {
    var newParams = this.state.params.slice();
    var val = parseFloat(this.state.inp);
    if (isNaN(val)) {
      this.setState({inp: this.state.params[this.state.pind][1]});
      return;
    } else {
      newParams[this.state.pind][1] = val;
    }

    var payload = {
      params: JSON.stringify(newParams),
      params_ind: this.props.params_ind
    }

    $.ajax({
      url: '/api/params/',
      type: 'PUT',
      data: payload,
      success: function (res) {
        console.log("DONE");
      }
    });
  }

  onChange(event) {
    this.state.inp = event.target.value;
    this.setState({inp: this.state.inp});
  }

  onSelectChange(e) {
    this.setState({pind: e.target.value, inp: this.state.params[e.target.value][1]});
  }

  render () {
    return (
      <div className="params-view">
        Params File: {this.props.params_ind != null 
          ? this.props.params_ind : 'None'}
        <br />
        <button onClick={() => this.createNewParams()}>New Params</button>
        <br />
        <input
          type="text"
          ref={e => {this.select_inp = e}}
          placeholder="File Number"
        />
        <button onClick={() => this.setParams()}>Select Params</button>
        <br />
        {this.state.params && 
          <div>
            <select value={this.state.pind} onChange={(e) => this.onSelectChange(e)}>
              {this.state.params.map((x, i) => {
                return (
                  <option key={i} value={i}>{x[0]}</option>
                )
                })}
            </select>
            <input
              type="text"
              ref={e => {this.pelt = e}}
              value={this.state.inp}
              onChange={(e) => this.onChange(e)}
              onBlur={(e) => this.updateParams()}
            />
          </div>
        }
        <br /><br />
      </div>
    )
  }
}

const mapStateToProps = (state, ownProps) => {
  return {
    // propName: state.prop
    params_ind: state.params_ind
  }
}

const mapDispatchToProps = (dispatch, ownProps) => {
  return {
		// propName: () => {dispatch(action(x))}
    setParamsInd: (x) => {dispatch(setParamsInd(x))}
  }
}

const ParamsView = connect(
  mapStateToProps,
  mapDispatchToProps
)(UnboundParamsView);

export default ParamsView
