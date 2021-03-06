import $ from 'jquery'

function chunk (x, size) {
  var y = [];
  for (var i = 0; i < x.length; i += size) {
    y.push(x.slice(i, i + size));
  }
  return y;
}

export function updateFromServer(text) {
  var updates = text.split(';').slice(0, -1);
  var newranges = [];
  var comments = [];
  for (var update of updates) {
    var [action, ...rest] = $.trim(update).split(',').slice(0, -1);
    if (action.startsWith('arange')) {
      var range = chunk(rest.map(parseFloat), 3);
      newranges.push({
        type:'arange',
        ind:parseInt(action.split(':')[1]),
        range:range
      });
    } else if (action.startsWith('range')) {
      var range = rest.map(parseFloat);
      newranges.push({
        type:'range',
        ind:parseInt(action.split(':')[1]),
        range:range
      });
    } else if (action.startsWith('comment')) {
      comments.push(rest[0]);
    }
  }

  return {
    type: 'SET_RANGES',
    ranges: newranges,
    comments: comments
  }
}

export function resetRanges(text) {
  return {
    type: 'RESET_RANGES'
  }
}

export function setNumPlayers(num_players) {
  return {type: 'SET_NUM_PLAYERS', num_players: num_players};
}

export function setParamsInd(x) {
  return {type: 'SET_PARAMS_IND', ind: x};
}
