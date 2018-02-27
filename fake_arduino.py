from flask import Flask, render_template, request

app = Flask(__name__)
fence_on = False


@app.route('/fence', methods=['GET', 'POST'])
def fence():
    global fence_on
    if request.method == 'POST':
        fence_on = not fence_on
    return "1" if fence_on else "0"


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)
