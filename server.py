import requests
from flask import Flask, render_template, request
from flask_httpauth import HTTPBasicAuth

app = Flask(__name__)
#ARDUINO_URL = "http://192.168.168.115"
ARDUINO_URL = "http://localhost:5001"
TIMEOUT = 10

auth = HTTPBasicAuth()
users = {"admin": "admin"}  # WARNING: to be customized!


@auth.get_password
def get_pw(username):
    if username in users:
        return users.get(username)
    return None


def getArduinoUrl(suffix):
    return ARDUINO_URL + "/" + suffix


@app.route('/')
@auth.login_required
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
@auth.login_required
def fence():
    if request.method == 'POST':
        return switch("fence")
    return read_state("fence")


@app.route('/light_in1', methods=['GET', 'POST'])
@auth.login_required
def light_in1():
    if request.method == 'POST':
        return switch("light_in1")
    return read_state("light_in1")


@app.route('/light_in2', methods=['GET', 'POST'])
@auth.login_required
def light_in2():
    if request.method == 'POST':
        return switch("light_in2")
    return read_state("light_in2")


@app.route('/light_out', methods=['GET', 'POST'])
@auth.login_required
def light_out():
    if request.method == 'POST':
        return switch("light_out")
    return read_state("light_out")


if __name__ == "__main__":
    app.run(host="0.0.0.0")
