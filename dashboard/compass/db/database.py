"""TO DO DOC"""
from os import path

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

from . import BASE

__all__ = ['Session']


class Session(object):

    """Controller for database session."""

    def __init__(self, database_name='dashboard.db', base=BASE):
        self.engine = create_engine('sqlite:///{}'.format(
            path.join(path.dirname(path.abspath(__file__)), database_name)
        ))
        base.metadata.create_all(self.engine)
        base.metadata.bind = self.engine
        self.db_session = sessionmaker(bind=self.engine)
        self.session = None

    def __enter__(self):
        self.session = self.db_session()
        return self.session

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.session.close()
