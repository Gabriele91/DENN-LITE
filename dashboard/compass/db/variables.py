from sqlalchemy.ext.declarative import declarative_base

SECRET_KEY = "good old days"
JWT_ALGORITHM = "HS256"
BASE = declarative_base()
