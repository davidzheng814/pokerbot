from flask import Blueprint, render_template
from flask_restful import Resource, reqparse

import json
import os
import pexpect
import subprocess
import glob

parser = reqparse.RequestParser()
parser.add_argument('data')
parser.add_argument('is_new_hand', type=lambda x: x == 'true')
parser.add_argument('params_ind')

class Engine(object):
    def __init__(self, params_ind=0):
        self.proc = pexpect.spawn(
            'engine/poker --params engine/params/params%d.txt' % (params_ind,))

    def update(self, text):
        self.proc.sendline(text)
        return self.proc.expect('START (.*) END')

    def reply(self):
        return self.proc.match.group(1)

def create_new_params():
    ind = len(glob.glob('engine/params/params*.txt'))
    subprocess.call(
        ('cp engine/params/params0.txt engine/params/params%d.txt' % (ind,))
        .split(' '))

    return ind
    

eng = Engine()

class GameState(Resource):
    def post(self):
        global eng
        args = parser.parse_args()
        if args.is_new_hand:
            params_ind = int(args.params_ind) if args.params_ind else 0
            eng = Engine(params_ind)
        with open('./received.txt', 'a') as f:
            f.write(args.data + '\n')
        eng.update(args.data)
        text = eng.reply()
        return {
            'text': eng.reply(),
        }

class Params(Resource):
    put_parser = reqparse.RequestParser()
    post_parser = reqparse.RequestParser()

    def __init__(self):
        self.post_parser.add_argument('params_ind')

        self.put_parser.add_argument('params')
        self.put_parser.add_argument('params_ind')

    def post(self):
        ind = create_new_params()

        return ind

    def get(self):
        args = self.post_parser.parse_args()
        params_ind = int(args.params_ind) if args.params_ind else 0

        path = 'engine/params/params%d.txt' % (params_ind,)
        if not os.path.exists(path):
            return False
        with open(path) as f:
            func = lambda x: (x[0], float(x[1]))
            data = [func(x.split(' ')) for x in f.readlines()]
            return data

    def put(self):
        args = self.put_parser.parse_args()
        new_params = json.loads(args.params)
        params_ind = int(args.params_ind)
        with open('engine/params/params%d.txt' % (params_ind,), 'w') as f:
            for name, val in new_params:
                f.write("%s %f\n" % (name, val))

