import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import { setParamsInd } from '../actions/index'
import $ from 'jquery'

class UnboundParamsView extends Component{
  constructor () {
    super();
    this.state = {
      ind: null,
      params: null
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
      this.pelts = [];
      this.props.setParamsInd(x);
      this.setState({params: data});
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
    for (var i = 0; i < newParams.length; ++i) {
      newParams[i][1] = parseFloat(this.pelts[i].value);
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

  onChange(event, i) {
    this.state.params[i][1] = event.target.value;
    this.setState({params: this.state.params});
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
          this.state.params.map((x, i) => {
            return (
              <div key={i}>
                <label>{x[0]} </label>
                <input
                  type="text"
                  ref={e => {this.pelts[i] = e}}
                  value={x[1]}
                  onChange={(e) => this.onChange(e, i)}
                  onBlur={(e) => this.updateParams()}
                />
              </div>
            )
        })}
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
