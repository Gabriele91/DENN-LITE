from sqlalchemy import Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import relationship
from sqlalchemy import create_engine
from os import path
import hashlib


__all__ = ['User', 'Session', 'gen_passwd', 'gen_user_id']

BASE = declarative_base()


class User(BASE):
    __tablename__ = 'usert'
    id = Column(Integer, primary_key=True)
    username = Column(String(256), nullable=False)
    password = Column(String(256), nullable=False)
    role = Column(String(256), nullable=False)


ENGINE = create_engine('sqlite:///{}'.format(
    path.join(path.dirname(path.abspath(__file__)), 'dashboard.db')
))

BASE.metadata.create_all(ENGINE)

def gen_passwd(passwd):
    passwd_hash = hashlib.sha256()
    passwd_hash.update("{}".format(passwd).encode('ascii'))
    return passwd_hash.hexdigest()

def gen_user_id(username):
    username_hash = hashlib.md5()
    username_hash.update("{}".format(username).encode('ascii'))
    return username_hash.hexdigest()


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
