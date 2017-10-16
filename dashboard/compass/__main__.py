def main():
    import argparse
    import os
    import hashlib

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('script', type=str,
                        choices=['init-db'],
                        help='Script to execute')

    args = parser.parse_args()

    if args.script == 'init-db':
        print("==> Initialize database! (all previous data will be deleted...)")
        os.remove(os.path.join(os.path.dirname(__file__), "dashboard.db"))
        print("==> Deleted current database!")

        from sqlalchemy import create_engine
        from . db import User, Session, gen_passwd_hash, gen_api_key

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
            print("==> Created base user 'admin' with passwd 'admin'")


if __name__ == '__main__':
    main()
