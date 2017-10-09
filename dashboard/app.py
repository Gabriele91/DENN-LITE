from flask import Flask
from flask import send_from_directory
from flask import request
from flask import abort
from flask import redirect
from flask import Response
from flask_login import LoginManager
from flask_login import UserMixin
from flask_login import login_required
from flask_login import login_user
from flask_login import logout_user
from compass.db import Session
from compass.db import User
from compass.db import gen_passwd_hash
from compass.db import gen_user_id_hash
from compass.db import get_user_from_api_key
from compass import SECRET_KEY
import sqlalchemy
import os

# global vars
PROTECTED_FOLDER = "./compass"

# app
app = Flask(__name__)

# config
app.config.update(
    DEBUG=True,
    SECRET_KEY=SECRET_KEY
)

# flask-login
login_manager = LoginManager()
login_manager.login_view = "login"
login_manager.init_app(app)

# silly user model


class FlaskUser(UserMixin):

    def __init__(self, username):
        self.username = username
        self.__username_id = gen_user_id_hash(username)

    @property
    def id(self):
        self.__username_id

    def __repr__(self):
        return self.username

    def check_passwd(self, passwd):
        with Session() as session:
            try:
                session.query(User).filter(
                    User.username == self.username,
                    User.password == gen_passwd_hash(passwd)
                ).one()
            except sqlalchemy.orm.exc.NoResultFound:
                return False
            return True

# root


@app.route("/", methods=["GET"])
def web_root():
    return redirect('/static/login.html')


# somewhere to login
@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['passwd']
        user = FlaskUser(username)
        if user.check_passwd(password):
            login_user(user)
            return redirect('/dashboard/index.html')
        else:
            return abort(401)
    else:
        return redirect('/static/login.html')


@app.route('/dashboard/<path:filename>')
@login_required
def protected(filename):
    return send_from_directory(
        PROTECTED_FOLDER,
        filename
    )


@app.route("/logout")
@login_required
def logout():
    logout_user()
    return Response('<p>Logged out</p>')


@app.errorhandler(401)
def page_not_found(e):
    return Response('<p>Login failed</p>')


@login_manager.user_loader
def load_user(username):
    return FlaskUser(username)

@login_manager.request_loader
def load_user_from_request(request):
    print(request)
    print(request.args)
    # first, try to login using the api_key url arg
    api_key = request.args.get('api_key')
    if api_key:
        user = get_user_from_api_key(api_key)
        if user:
            return user

    return None
