from sqlalchemy import Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import relationship
from sqlalchemy import create_engine
import sqlalchemy
from os import path
import hashlib
import jwt
from . bar import SECRET_KEY
from . bar import JWT_ALGORITHM


__all__ = ['User', 'Session', 'gen_passwd_hash',
           'gen_user_id_hash', 'gen_api_key']

# TYPE OF DATABASE
BASE = declarative_base()
ENGINE = None


class User(BASE):

    """Dashboard user representation."""

    __tablename__ = 'usert'
    # First integer primary key has autoincrement by default
    id = Column(Integer, primary_key=True)
    username = Column(String(256), nullable=False, unique=True)
    password = Column(String(256), nullable=False)
    api_key = Column(String(256), nullable=False, unique=True)
    role = Column(String(256), nullable=False)


def create_db():
    """Generates the database."""
    global ENGINE, BASE
    # DATABASE ENGINE
    ENGINE = create_engine('sqlite:///{}'.format(
        path.join(path.dirname(path.abspath(__file__)), 'dashboard.db')
    ))

    # BASE METADATA
    BASE.metadata.create_all(ENGINE)


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


def get_user_from_api_key(api_key):
    """Returns the user of the given api_key.

    Args:
        api_key (str): user api key

    Returns:
        User: the user instance
    """
    return User.query.filter_by(api_key=api_key).first()


class Session(object):

    """Controller for database session."""

    def __init__(self):
        if ENGINE is None:
            create_db()
        BASE.metadata.bind = ENGINE
        self.DBSession = sessionmaker(bind=ENGINE)
        self.session = None

    def __enter__(self):
        self.session = self.DBSession()
        return self.session

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.session.close()
