import flask

import serial

from flask import Flask
app = Flask(__name__)
ser = serial.Serial('/dev/tty.usbmodem1411', 115200, timeout=1) # thom's 
# ser = serial.Serial('/dev/tty.usbmodemFA131', 115200, timeout=1) # john's

@app.after_request
def add_header(response):    
    if flask.request.url.endswith('.html'):
        response.headers['Cache-Control'] = 'no-cache, no-store, must-revalidate'
        response.headers['Pragma'] = 'no-cache'
    else:
        response.headers['Cache-Control'] = 'max-age=6000'
    return response

@app.route("/open/<door>")
def hello(door):
    string = b'1 {}\r'.format(door)
    try:
        ser.write(string)
    except serial.serialutil.SerialException:
        pass
    return flask.redirect('/close_door.html')

@app.route('/<filename>')
def serve(filename):
    return flask.send_from_directory('static', filename)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
