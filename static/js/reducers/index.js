import _ from 'underscore'

const start_num_players = 6;
const null_ranges = (x) => _.range(x).map(y => null);
const initial_state = {
  num_players: start_num_players,
  ranges: null_ranges(start_num_players),
  aranges: null_ranges(start_num_players),
  params_ind: null,
  comments: []
};

function setRanges(ranges, aranges, newranges) {
  ranges = ranges.slice();
  aranges = aranges.slice(); 
  for (var range of newranges) {
    if (range.type == 'range') {
      ranges[range.ind] = range.range;
    } else {
      aranges[range.ind] = range.range;
    }
  }
  return [ranges, aranges]
}

export default (state=initial_state, action) => {
  switch (action.type) {
    case 'SET_NUM_PLAYERS':
      return Object.assign({}, state, {
        num_players: action.num_players,
        ranges: null_ranges(action.num_players),
        aranges: null_ranges(action.num_players)
      });
    case 'SET_RANGES':
      var [ranges, aranges] = setRanges(state.ranges, state.aranges, action.ranges);
      return Object.assign({}, state, {
        ranges: ranges,
        aranges: aranges,
        comments: action.comments
      });
    case 'RESET_RANGES':
      var newranges = {
        ranges: null_ranges(),
        aranges: null_ranges()
      };
      return Object.assign({}, state, newranges);
    case 'SET_PARAMS_IND':
      return Object.assign({}, state, { params_ind: action.ind })
    default:
      return state
  }
}

