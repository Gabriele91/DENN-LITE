from flask import Flask
from flask import send_from_directory
from flask import request
from flask import abort
from flask import redirect
from flask import Response
from flask import render_template
import flask_login
from compass.db import Session
from compass.db import User
from compass.db import gen_passwd_hash
from compass.db import gen_user_id_hash
from compass.db import get_user_from_api_key
from compass import SECRET_KEY
import sqlalchemy
import json
from os import path

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
login_manager = flask_login.LoginManager()
login_manager.session_protection = 'strong'
login_manager.init_app(app)


class DashboardUser(flask_login.UserMixin):

    """A typical Dashboard user."""

    def __init__(self, username):
        self.username = username
        self.role = self.__get_role()
        self.__user_id_hash = gen_user_id_hash(username)

    def get_id(self):
        """Return the current user id hash."""
        return self.__user_id_hash

    def __repr__(self):
        return "[{}|{}|{}]".format(self.username, self.role, self.__user_id_hash)

    def check_passwd(self, passwd):
        """Checks if the password given matches the user password."""
        with Session() as session:
            try:
                session.query(User).filter(
                    User.username == self.username,
                    User.password == gen_passwd_hash(passwd)
                ).one()
            except sqlalchemy.orm.exc.NoResultFound:
                return False
            return True

    def __get_role(self):
        with Session() as session:
            try:
                res = session.query(User).filter(
                    User.username == self.username
                ).one()
            except sqlalchemy.orm.exc.NoResultFound:
                return False
            return res.role


USER_CACHE = {}  # Logged user cache

with open(path.join(path.dirname(__file__), "config.json")) as config_file:
    DASHBOARD_CONFIG = json.load(config_file)


@app.route("/", methods=["GET"])
def web_root():
    """Main route of the website is redirected to login."""
    return redirect('/static/login.html')


# somewhere to login
@app.route("/login", methods=["GET", "POST"])
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
        else:
            return abort(401)
    else:
        return redirect('/static/login.html')


@app.route("/dashboard", methods=["GET"])
@flask_login.login_required
def web_dashboard():
    """Redirects the base dashboard url to the index."""
    return redirect('/dashboard/home')


@app.route('/dashboard/<path:filename>')
@flask_login.login_required
def web_dashboard_files(filename):
    """Dashboard section.

    Check if user requests the index and
    serves the template or the file from the 
    protected directory.
    """
    if filename in DASHBOARD_CONFIG['sections']:
        return render_template("dashboard/index.html",
                               role=flask_login.current_user.role,
                               section=filename
                               )
    else:
        return send_from_directory(
            PROTECTED_FOLDER,
            filename
        )


@login_manager.unauthorized_handler
def unauthorized():
    """Shows if user is not authorized"""
    return Response('<p>Unauthorized</p>')


@app.route("/logout")
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


@app.errorhandler(401)
def page_not_found(error):
    """401 error handler."""
    return Response('<p>Login failed</p>')


@login_manager.user_loader
def load_user(user_id):
    """User loader.

    Check the user instance from the USER_CACHE
    using the passed user_id.
    """
    if user_id in USER_CACHE:
        return USER_CACHE[user_id]
