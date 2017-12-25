if __name__ == '__main__':
    import http.server
    addr = ('', 8000)
    httpd = http.server.HTTPServer(addr, http.server.CGIHTTPRequestHandler)
    httpd.serve_forever()
"""
from flask import Flask, render_template

app = Flask(__name__)

@app.route("/")
def home():
    return render_template('home.html')

if __name__ == '__main__':
    app.run()
"""
