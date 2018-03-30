import requests
from requests.auth import HTTPBasicAuth


def command(on):
    requests.post("http://localhost:5000/alarm_detector", json=on, auth=HTTPBasicAuth('admin', 'admin'))


if __name__ == "__main__":
    import sys
    try:
        on = bool(int(sys.argv[1]))
    except IndexError:
        on = True
    command(on)
