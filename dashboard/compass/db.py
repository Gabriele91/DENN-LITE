from sqlalchemy import Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import relationship
from sqlalchemy import create_engine
from os import path
import hashlib
import jwt
from . bar import SECRET_KEY
from . bar import JWT_ALGORITHM


__all__ = ['User', 'Session', 'gen_passwd_hash',
           'gen_user_id_hash', 'gen_api_key']

BASE = declarative_base()


class User(BASE):
    __tablename__ = 'usert'
    id = Column(Integer, primary_key=True)
    username = Column(String(256), nullable=False)
    password = Column(String(256), nullable=False)
    api_key = Column(String(256), nullable=False)
    role = Column(String(256), nullable=False)


ENGINE = create_engine('sqlite:///{}'.format(
    path.join(path.dirname(path.abspath(__file__)), 'dashboard.db')
))

BASE.metadata.create_all(ENGINE)


def gen_passwd_hash(passwd):
    passwd_hash = hashlib.sha256()
    passwd_hash.update("{}".format(passwd).encode('ascii'))
    return passwd_hash.hexdigest()


def gen_user_id_hash(username):
    username_hash = hashlib.md5()
    username_hash.update("{}".format(username).encode('ascii'))
    return username_hash.hexdigest()


def gen_api_key(username, role):
    encoded = jwt.encode(
        {
            'username': username,
            'role': role
        },
        SECRET_KEY, algorithm=JWT_ALGORITHM
    )
    return encoded


def read_api_key(encoded):
    try:
        decoded = jwt.decode(encoded, SECRET_KEY, algorithm=JWT_ALGORITHM)
    except jwt.exceptions.DecodeError:
        return False
    return decoded

def get_user_from_api_key(api_key):
    return User.query.filter_by(api_key=api_key).first()


class Session(object):

    def __init__(self):
        BASE.metadata.bind = ENGINE
        self.DBSession = sessionmaker(bind=ENGINE)
        self.session = None

    def __enter__(self):
        self.session = self.DBSession()
        return self.session

    def __exit__(self, exc_type, exc_val, exc_tb):

        self.session.close()
