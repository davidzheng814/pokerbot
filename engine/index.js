function square(text) {
  return $('<td>').html(text).addClass('square').addClass(text)
}

toStr = ["A", "K", "Q", "J", "T", "9", "8", "7", "6", "5", "4", "3", "2"]

function makeCombos() {
  combos = [];
  for (var i = 0; i < 13; ++i) {
    for (var j = 0; j < 13; ++j) {
      if (i == j) {
        combos.push(toStr[i] + toStr[j]);
      } else if (i < j) {
        combos.push(toStr[i] + toStr[j] + 'o');
      } else {
        combos.push(toStr[j] + toStr[i] + 's');
      }
    }
  }
}

function numCombos(x) {
  return x.includes('s') ? 4 : x.includes('o') ? 12 : 6
}

function color() {
  var elts = $('#list').children().map(function(x, y) {
    return $(y).html();
  });
  var thresh = Math.round((parseFloat($('#pct').val()) / 100 ) * 1326);
  for (var elt of elts) {
    if (elt.length < 2 || elt.length > 3) continue;
    thresh -= numCombos(elt);
    if (thresh >= 0) {
      $('.' + elt).css({'background-color': 'white'})
    } else {
      $('.' + elt).css({'background-color': '#ccc'})
    }
  }
  var text = "[";
  for (var x of elts) {
    text += '"' + x + '", ';
  }
  text += "]";
  $('#text').html(text);
  console.log('finished');
}

cards = ["AA","KK","QQ","AKs","AKo","JJ","AQs","AQo","TT","99","KQs","AJs","ATs","AJo","KQo","88","77","KJs","QJs","KTs","QTs","JTs","T9s","A9s","A5s","A4s","A3s","A2s","ATo","QJo","98s","KJo","66","A8s","A7s","A6s","87s","55","44","33","22","J9s","T8s","K9s","KTo","Q9s","76s","A9o","65s","QTo","97s","JTo","T9o","K8s","K9o","Q9o","J8s","Q8s","K7s","K6s","K5s","86s","K4s","K3s","K2s","J9o","75s","A8o","A7o","A6o","A5o","A4o","A3o","A2o","98o","T8o","T7s","Q7s","J7s","54s","Q6s","64s","43s","J8o","K8o","Q8o","K7o","Q7o","96s","Q5s","Q4s","Q3s","T6s","Q2s","53s","85s","J6s","J5s","74s","J4s","J3s","95s","J2s","63s","52s","T5s","84s","T4s","T3s","42s","T2s","73s","32s","94s","93s","62s","92s","87o","83s","82s","97o","72s","76o","65o","T7o","K6o","86o","54o","K5o","J7o","75o","K4o","K3o","96o","K2o","64o","Q6o","53o","85o","T6o","Q5o","43o","Q4o","Q3o","74o","Q2o","J6o","63o","J5o","95o","52o","J4o","J3o","42o","J2o","84o","T5o","T4o","32o","T3o","73o","T2o","62o","94o","93o","92o","83o","82o","72o"]

b = ["K7o", "Q7o", "K8o", "Q8o", "J8o", "T8o", "98o", "64s", "54s", "43s", "T7s", "J7s", "Q7s", "Q6s"]

a = cards.filter(function(x) {
  return b.includes(x);
});

c = cards.filter(function(x) {
  return !b.includes(x);
});

console.log(JSON.stringify(a));
console.log(JSON.stringify(c));

$(document).ready(function() {
  makeCombos();
  for (var i = 0; i < 13; ++i) {
    var row = $('<tr>');
    for (var j = 0; j < 13; ++j) {
      row.append(square(combos[j * 13 + i]));
    }
    $("#range").append(row);
  }
  for (var i = 0; i < cards.length; ++i) {
    $('#list').append($('<li>').html(cards[i]));
  }

  $('#list').sortable();
  $('#list').disableSelection();
  $('body').mouseup(function() {
    color();
  });
  $('#pct').change(function() { color() });
  color();
});

