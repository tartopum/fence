import requests
from flask import Flask, render_template, request

app = Flask(__name__)
#ARDUINO_URL = "http://192.168.167.100"
ARDUINO_URL = "http://localhost:5001"
TIMEOUT = 10

def getArduinoUrl(suffix):
    return ARDUINO_URL + "/" + suffix


@app.route('/')
def home():
    return render_template("index.html")


def switch_fence():
    try:
        return requests.post(getArduinoUrl("fence")).text
    except requests.exceptions.ConnectionError:
        return 'Cannot connect to Arduino', 404


def fence_state():
    try:
        return requests.get(getArduinoUrl("fence"), timeout=TIMEOUT).text
    except requests.exceptions.ConnectionError:
        return 'Cannot connect to Arduino', 404


@app.route('/fence', methods=['GET', 'POST'])
def fence():
    if request.method == 'POST':
        return switch_fence()
    return fence_state()


if __name__ == "__main__":
    app.run(host="0.0.0.0")
