from flask import Blueprint, render_template
from flask_restful import Resource, reqparse

import json
import os
import pexpect

parser = reqparse.RequestParser()
parser.add_argument('data')
parser.add_argument('is_new_hand', type=lambda x: x == 'true')

class Engine(object):
    def __init__(self):
        self.proc = pexpect.spawn('engine/poker')

    def update(self, text):
        self.proc.sendline(text)
        return self.proc.expect('START (.*) END')

    def reply(self):
        return self.proc.match.group(1)

eng = Engine()

class GameState(Resource):
    def post(self):
        global eng
        args = parser.parse_args()
        if args.is_new_hand:
            eng = Engine()
        with open('./received.txt', 'a') as f:
            f.write(args.data + '\n')
        eng.update(args.data)
        text = eng.reply()
        return {
            'text': eng.reply(),
        }

