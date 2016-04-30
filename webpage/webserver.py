import flask

import serial

from flask import Flask
app = Flask(__name__)

@app.route("/open/<door>")
def hello(door):
    string = b'1 {}\r'.format(door)
    try:
        with serial.Serial('/dev/tty.usbmodem1411', 115200, timeout=1) as ser:
            ser.write(string)
    except serial.serialutil.SerialException:
        pass
    return flask.redirect('/close_door.html')

@app.route('/<filename>')
def serve(filename):
    light = {
        'index.html': 1,
        'instructions.html': 2,
        'door_select.html': 3,
        'close_door.html': 4,
    }

    try:
        with serial.Serial('/dev/tty.usbmodem1411', 115200, timeout=1) as ser:
            string = b'2 {}\r'.format(light[filename])
            ser.write(string)
    except serial.serialutil.SerialException:
        pass

    return flask.send_from_directory('static', filename)

if __name__ == "__main__":
    print "hello world"
    app.run(host="0.0.0.0", port=8000, debug=True)
