from flask import Flask
app = Flask(__name__,
            static_url_path="/dashboard")


@app.route('/test')
def hello_world():
    return 'Hello, World!'
