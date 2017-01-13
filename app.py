from flask import Flask, send_from_directory
from flask_restful import Api

from views import main
from api import GameState

import os

app = Flask(__name__)
app.register_blueprint(main)

api = Api(app)
api.add_resource(GameState, '/api/game_state/')

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """

    return response

if __name__ == '__main__':
    app.run(debug=True)
