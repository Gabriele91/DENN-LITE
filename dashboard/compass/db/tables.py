"""TO DO DOC"""
import hashlib

import jwt
import sqlalchemy

from . import BASE, SECRET_KEY, JWT_ALGORITHM, Session

__all__ = ['User']


class User(BASE):

    """Dashboard user."""

    __tablename__ = 'usert'
    # First integer primary key has autoincrement by default
    id = sqlalchemy.Column(sqlalchemy.Integer, primary_key=True)
    username = sqlalchemy.Column(
        sqlalchemy.String(256), nullable=False, unique=True)
    password = sqlalchemy.Column(sqlalchemy.String(256), nullable=False)
    api_key = sqlalchemy.Column(
        sqlalchemy.String(256), nullable=False, unique=True)
    role = sqlalchemy.Column(sqlalchemy.String(256), nullable=False)

    @staticmethod
    def gen_passwd_hash(passwd):
        """Generates the hash of a user password.

        Args:
            passwd (str): user password

        Returns:
            str: hash (sha256) of the user password
        """
        passwd_hash = hashlib.sha256()
        passwd_hash.update("{}".format(passwd).encode('ascii'))
        return passwd_hash.hexdigest()

    @staticmethod
    def gen_user_id_hash(username):
        """Generates the hash of a user.

        Args:
            username (str): the user nickname

        Returns:
            str: hash (md5) of the username
        """
        with Session() as session:
            try:
                res = session.query(User).filter(
                    User.username == username
                ).one()
                id_hash = hashlib.md5()
                id_hash.update("{}".format(res.id).encode('ascii'))
            except sqlalchemy.orm.exc.NoResultFound:
                return None
        return id_hash.hexdigest()

    @staticmethod
    def gen_api_key(username, role):
        """Generates an api key.

        Args:
            username (str): the user nickname
            role (str): the user role

        Returns:
            str: JWT with username and role encoded
        """
        encoded = jwt.encode(
            {
                'username': username,
                'role': role
            },
            SECRET_KEY, algorithm=JWT_ALGORITHM
        )
        return encoded

    @staticmethod
    def read_api_key(encoded):
        """Reads api key.

        Args:
            encoded (str): JWT encoded string

        Returns:
            str/bool: the decoded object or False if not possible
        """
        try:
            decoded = jwt.decode(encoded, SECRET_KEY, algorithm=JWT_ALGORITHM)
        except jwt.exceptions.DecodeError:
            return False
        return decoded

    @staticmethod
    def get_user_from_api_key(api_key):
        """Returns the user of the given api_key.

        Args:
            api_key (str): user api key

        Returns:
            User: the user instance
        """
        return User.query.filter_by(api_key=api_key).first()

    @staticmethod
    def get_role(username):
        """The the role of the given user."""
        with Session() as session:
            try:
                res = session.query(User).filter(
                    User.username == username
                ).one()
            except sqlalchemy.orm.exc.NoResultFound:
                return False
            return res.role

    @staticmethod
    def check_passwd(username, passwd):
        """Check if the passwd is correct for the given user."""
        with Session() as session:
            try:
                session.query(User).filter(
                    User.username == username,
                    User.password == User.gen_passwd_hash(passwd)
                ).one()
            except sqlalchemy.orm.exc.NoResultFound:
                return False
            return True
