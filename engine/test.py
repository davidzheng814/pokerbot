to_num = {
    'A': 0,
    'K': 1,
    'Q': 2,
    'J': 3,
    'T': 4,
    '9': 5,
    '8': 6,
    '7': 7,
    '6': 8,
    '5': 9,
    '4': 10,
    '3': 11,
    '2': 12
}

def to_ind(string):
    card1 = to_num[string[0]]
    card2 = to_num[string[1]]
    if len(string) == 2 or string[2] == 's':
        return card1 * 13 + card2
    return card2 * 13 + card1

f = open('a.txt')
cards = {}
for i, line in enumerate(f.readlines()):
    cards[to_ind(line.strip())] = i

for i in range(169):
    print "{" + str(cards[i]), ", 0, 0},",
    if i % 13 == 12 or i % 13 == 7:
        print ""
