import requests
from flask import Flask, render_template, request, jsonify
from flask_httpauth import HTTPBasicAuth

app = Flask(__name__)
#ARDUINO_URL = "http://192.168.168.115"
ARDUINO_URL = "http://192.168.167.102"
#ARDUINO_URL = "http://127.0.0.1:5001"
TIMEOUT = 10

auth = HTTPBasicAuth()
users = {"admin": "admin"}  # WARNING: to be customized!

alarm_detector_on = False


@auth.get_password
def get_pw(username):
    if username in users:
        return users.get(username)
    return None


def arduino_url(suffix):
    return ARDUINO_URL + "/" + suffix


@app.route('/')
@auth.login_required
def home():
    return render_template("index.html")


def switch(url_suffix):
    try:
        return requests.post(arduino_url(url_suffix), timeout=TIMEOUT).text
    except requests.exceptions.ConnectionError:
        return 'Cannot connect to Arduino /' + url_suffix, 404


def read_state(url_suffix):
    return '0'
    try:
        return requests.get(arduino_url(url_suffix), timeout=TIMEOUT).text
    except (requests.exceptions.ConnectionError, requests.exceptions.ReadTimeout):
        return 'Cannot connect to Arduino /' + url_suffix, 404


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


@app.route('/alarm', methods=['GET', 'POST'])
@auth.login_required
def alarm():
    if request.method == 'POST':
        return switch("alarm")
    return read_state("alarm")


@app.route('/alarm_detector', methods=['GET', 'POST'])
@auth.login_required
def alarm_detector():
    global alarm_detector_on
    if request.method == 'POST':
        alarm_detector_on = bool(request.json)
    return jsonify(alarm_detector_on)

if __name__ == "__main__":
    app.run(host="0.0.0.0")
