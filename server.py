import requests
from flask import Flask, render_template

app = Flask(__name__)
ARDUINO_URL = "http://192.168.167.100"

def getArduinoUrl(suffix):
    return ARDUINO_URL + "/fence/" + suffix


@app.route('/')
def home():
    return render_template("index.html")


@app.route('/on')
def on():
    return requests.get(getArduinoUrl("on")).text


@app.route('/off')
def off():
    return requests.get(getArduinoUrl("off")).text


@app.route('/state')
def state():
    return requests.get(getArduinoUrl("state")).text


if __name__ == "__main__":
    app.run(host="0.0.0.0")
