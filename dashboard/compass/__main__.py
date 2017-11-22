"""Command line interface for compass module."""
from __future__ import print_function
import argparse
import os
import sys


def main():
    """Main function."""

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('script', type=str,
                        choices=['init-db'],
                        help='Script to execute')

    args = parser.parse_args()

    if args.script == 'init-db':
        print("==> Initialize database! (all previous data will be deleted...)")
        try:
            os.remove(os.path.join(os.path.dirname(
                __file__), "db", "dashboard.db"))
        except FileNotFoundError:
            pass
        print("==> Deleted current database!")

        from compass.db import User, Session

        with Session() as session:
            # Insert a Person in the person table
            new_user = User(
                username='admin',
                password=User.gen_passwd_hash('admin'),
                api_key=User.gen_api_key('admin', 'admin'),
                role='admin'
            )
            session.add(new_user)
            session.commit()
            print("==> Created base user 'admin' with passwd 'admin'")


if __name__ == '__main__':
    sys.exit(main())
