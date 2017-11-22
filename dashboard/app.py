"""Dashboard app made with Flask framework."""
import json
from os import path

import flask_login
from flask import (Flask, Response, abort, redirect, render_template, request,
                   send_from_directory)

from compass.db import SECRET_KEY, Session, User

# global vars
PROTECTED_FOLDER = "./compass"

# APP
APP = Flask(__name__)

# config
APP.config.update(
    DEBUG=True,
    SECRET_KEY=SECRET_KEY
)

# flask-login
LOGIN_MANAGER = flask_login.LoginManager()
LOGIN_MANAGER.session_protection = 'strong'
LOGIN_MANAGER.init_app(APP)


class DashboardUser(flask_login.UserMixin):

    """A typical Dashboard user."""

    def __init__(self, username):
        self.username = username
        self.role = User.get_role(username)
        self.__user_id_hash = User.gen_user_id_hash(username)

    def get_id(self):
        """Return the current user id hash."""
        return self.__user_id_hash

    def __repr__(self):
        return "[{}|{}|{}]".format(self.username, self.role, self.__user_id_hash)

    def check_passwd(self, passwd):
        """Checks if the password given matches the user password."""
        return User.check_passwd(self.username, passwd)


USER_CACHE = {}  # Logged user cache

with open(path.join(path.dirname(__file__), "config.json")) as config_file:
    DASHBOARD_CONFIG = json.load(config_file)


@APP.route("/", methods=["GET"])
def web_root():
    """Main route of the website is redirected to login."""
    return redirect('/static/login.html')


# somewhere to login
@APP.route("/login", methods=["GET", "POST"])
def login():
    """Entry point to do the login.

    If user is authenticated is added to the USER_CACHE.
    """
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['passwd']
        user = DashboardUser(username)
        if user.check_passwd(password):
            flask_login.login_user(user)
            USER_CACHE[user.get_id()] = user
            return redirect('/dashboard')

        return abort(401)

    return redirect('/static/login.html')


@APP.route("/dashboard", methods=["GET"])
@flask_login.login_required
def web_dashboard():
    """Redirects the base dashboard url to the index."""
    return redirect('/dashboard/home')


@APP.route('/dashboard/<path:filename>')
@flask_login.login_required
def web_dashboard_files(filename):
    """Dashboard section.

    Check if user requests the index and
    serves the template or the file from the
    protected directory.
    """
    if filename in DASHBOARD_CONFIG['sections']:
        return render_template("dashboard/index.html", role=flask_login.current_user.role, section=filename)

    return send_from_directory(
        PROTECTED_FOLDER,
        filename
    )


@LOGIN_MANAGER.unauthorized_handler
def unauthorized():
    """Shows if user is not authorized"""
    return Response('<p>Unauthorized</p>')


@APP.route("/logout")
@flask_login.login_required
def logout():
    """Logout entry point.

    This function remove the user from the USER_CACHE too.
    """
    user = flask_login.current_user
    if user.get_id() in USER_CACHE:
        del USER_CACHE[user.get_id()]
    flask_login.logout_user()
    return Response('<p>Logged out</p>')


@APP.errorhandler(401)
def page_not_found(error):
    """401 error handler."""
    return Response('<p>Login failed</p>')


@LOGIN_MANAGER.user_loader
def load_user(user_id):
    """User loader.

    Check the user instance from the USER_CACHE
    using the passed user_id.
    """
    if user_id in USER_CACHE:
        return USER_CACHE[user_id]
