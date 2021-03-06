CREATE ROLE admin SUPERUSER NOINHERIT CREATEDB CREATEROLE NOREPLICATION;
CREATE ROLE consultas NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE NOREPLICATION;


CREATE ROLE usuario1 LOGIN
  ENCRYPTED PASSWORD 'supersecretpassword1'
  NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE NOREPLICATION;
GRANT admin TO usuario1;
COMMENT ON ROLE usuario1 IS 'User with administrative privileges';


CREATE ROLE usuario2 LOGIN
  ENCRYPTED PASSWORD 'supersecretpassword2'
  NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE NOREPLICATION;
GRANT consultas TO usuario2;
COMMENT ON ROLE usuario2 IS 'Restricted user';
