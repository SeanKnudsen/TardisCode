import flask

import serial

from flask import Flask
app = Flask(__name__)

s = serial.Serial('/dev/tty.usbmodem1411', 115200, timeout=1)
@app.route("/open/<door>")
def hello(door):
    string = b'1 {}\r'.format(door)
    s.write(string)
    return ""

@app.route('/<filename>')
def serve(filename):
    return flask.send_from_directory('static', filename)

if __name__ == "__main__":
        app.run(host="0.0.0.0")
