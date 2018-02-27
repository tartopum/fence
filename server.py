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


def switch(url_suffix):
    try:
        return requests.post(getArduinoUrl(url_suffix), timeout=TIMEOUT).text
    except requests.exceptions.ConnectionError:
        return 'Cannot connect to Arduino', 404


def read_state(url_suffix):
    try:
        return requests.get(getArduinoUrl(url_suffix), timeout=TIMEOUT).text
    except requests.exceptions.ConnectionError:
        return 'Cannot connect to Arduino', 404


@app.route('/fence', methods=['GET', 'POST'])
def fence():
    if request.method == 'POST':
        return switch("fence")
    return read_state("fence")


@app.route('/light_in1', methods=['GET', 'POST'])
def light_in1():
    if request.method == 'POST':
        return switch("light_in1")
    return read_state("light_in1")



if __name__ == "__main__":
    app.run(host="0.0.0.0")
