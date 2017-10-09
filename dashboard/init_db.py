import os

os.remove(os.path.join("compass", "dashboard.db"))

from sqlalchemy import create_engine
from compass.db import User, Session, gen_passwd_hash, gen_api_key
import hashlib

with Session() as session: 
    # Insert a Person in the person table
    new_user = User(
        username='admin',
        password=gen_passwd_hash('admin'),
        api_key=gen_api_key('admin', 'admin'),
        role='admin'
    )
    session.add(new_user)
    session.commit()