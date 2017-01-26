import sys

to_num = {
    'A':0,
    'K':1,
    'Q':2,
    'J':3,
    'T':4,
    '9':5,
    '8':6,
    '7':7,
    '6':8,
    '5':9,
    '4':10,
    '3':11,
    '2':12,
}

def convert_to_num(x):
    if 's' in x:
        return to_num[x[0]] * 13 + to_num[x[1]]
    else:
        return to_num[x[1]] * 13 + to_num[x[0]]

with open(sys.argv[1]) as f:
    hands = [convert_to_num(x.strip()) for x in f.readlines()]
    print hands
