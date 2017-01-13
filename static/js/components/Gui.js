import React, { Component, PropTypes } from 'react'
import Cli from './Cli'
import Model from '../Model'
import RangeView from './Ranges'

class Gui extends Component {
  render() {
    return (
      <div className="gui">
        <RangeView />
        <Cli />
      </div>
    )
  }
}

export default Gui
