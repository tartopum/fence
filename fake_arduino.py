from flask import Flask, render_template, request

app = Flask(__name__)
fence_on = False
light_in1_on = False
light_in2_on = False
light_out_on = False


@app.route('/fence', methods=['GET', 'POST'])
def fence():
    global fence_on
    if request.method == 'POST':
        fence_on = not fence_on
    return "1" if fence_on else "0"


@app.route('/light_in1', methods=['GET', 'POST'])
def light_in1():
    global light_in1_on
    if request.method == 'POST':
        light_in1_on = not light_in1_on
    return "1" if light_in1_on else "0"


@app.route('/light_in2', methods=['GET', 'POST'])
def light_in2():
    global light_in2_on
    if request.method == 'POST':
        light_in2_on = not light_in2_on
    return "1" if light_in2_on else "0"


@app.route('/light_out', methods=['GET', 'POST'])
def light_out():
    global light_out_on
    if request.method == 'POST':
        light_out_on = not light_out_on
    return "1" if light_out_on else "0"


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)
