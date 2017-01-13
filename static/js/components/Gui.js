import React, { Component, PropTypes } from 'react'
import Cli from './Cli'
import Model from '../Model'
import RangeView from './Ranges'
import ParamsView from './Params'

class Gui extends Component {
  render() {
    return (
      <div className="gui">
        <ParamsView />
        <RangeView />
        <Cli />
      </div>
    )
  }
}

export default Gui
