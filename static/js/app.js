import React from 'react'
import ReactDOM from 'react-dom'
import { createStore } from 'redux'
import counter from './reducers'
import Gui from './components/Gui'
import { Provider } from 'react-redux'

const store = createStore(counter)
const rootEl = document.getElementById('root')

const render = () => ReactDOM.render(
  <Provider store={store}>
    <Gui />
  </Provider>,
  rootEl
)

render()
store.subscribe(render)
