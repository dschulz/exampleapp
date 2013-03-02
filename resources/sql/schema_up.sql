
\c exampleapp;

SET ROLE admin;


BEGIN;
	
CREATE SCHEMA exampleapp;
COMMENT ON SCHEMA exampleapp IS 'ExampleApp schema';
ALTER SCHEMA exampleapp OWNER TO admin;

ALTER DATABASE exampleapp SET search_path=exampleapp;

CREATE TABLE exampleapp.audit (
    id SERIAL NOT NULL PRIMARY KEY,
    tstamp TIMESTAMP DEFAULT now(),
    role TEXT NOT NULL,
    event TEXT NOT NULL
);

COMMENT ON TABLE exampleapp.audit IS 'Audit';
ALTER TABLE exampleapp.audit OWNER TO admin;

CREATE TABLE exampleapp.usuario (
    id SERIAL NOT NULL PRIMARY KEY,
    tstamp TIMESTAMP DEFAULT now(),
    role TEXT UNIQUE NOT NULL,
    fullname TEXT NOT NULL,
    active boolean DEFAULT false
);

COMMENT ON TABLE exampleapp.usuario IS 'Details about roles with LOGIN privileges';
ALTER TABLE exampleapp.usuario OWNER TO admin;


CREATE TABLE exampleapp.ticket (
    id SERIAL NOT NULL PRIMARY KEY,
    tstamp TIMESTAMP DEFAULT now(),
    userid TEXT NOT NULL,
    subject TEXT NOT NULL,
    body TEXT NOT NULL,
    prio INTEGER NOT NULL,
    status INTEGER NOT NULL
);

COMMENT ON TABLE exampleapp.ticket IS 'Tickets';
ALTER TABLE exampleapp.ticket OWNER TO admin;

CREATE TABLE exampleapp.pixmap (
    id SERIAL NOT NULL PRIMARY KEY,
    userid TEXT DEFAULT session_user,
    tstamp TIMESTAMP DEFAULT now(),
    base64data bytea,
    hash TEXT UNIQUE NOT NULL 
);

COMMENT ON TABLE exampleapp.pixmap IS 'Captured screenshots';
ALTER TABLE exampleapp.pixmap OWNER TO admin;


CREATE TABLE IF NOT EXISTS exampleapp.parameter (
    id SERIAL NOT NULL PRIMARY KEY,
    clave TEXT UNIQUE NOT NULL,
    valor TEXT NOT NULL,
    grupo TEXT NOT NULL,
    descripcion TEXT NOT NULL
);
COMMENT ON TABLE exampleapp.parameter IS 'ExampleApp global parameters';
ALTER TABLE exampleapp.parameter OWNER TO admin;

	
CREATE OR REPLACE FUNCTION exampleapp.prueba(eid INTEGER) RETURNS TEXT AS $$
DECLARE 
        eid ALIAS FOR $1;
	nombre TEXT;
BEGIN
	RAISE NOTICE 'Funcion prueba()' ;

	SELECT '(' || entidad.id || ') ' || entidad.nombre INTO nombre FROM exampleapp.entidad WHERE entidad.id=eid;
	

	RETURN nombre;
END;
$$ LANGUAGE plpgsql; 

COMMENT ON FUNCTION exampleapp.prueba(INTEGER) IS 'Test function';
ALTER FUNCTION exampleapp.prueba(INTEGER) OWNER TO postgres;

CREATE OR REPLACE FUNCTION exampleapp.check_password(role TEXT, rolpassword TEXT) RETURNS INTEGER AS $$
DECLARE
            r ALIAS FOR $1;
	    rp ALIAS FOR $2;
	    cnt INTEGER;
BEGIN

	SELECT COUNT(1) 
	FROM pg_catalog.pg_authid a 
	WHERE
              a.rolname = r
        AND 
	      lower(a.rolpassword) = lower(rp)
	INTO cnt;

	RETURN cnt;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

COMMENT ON FUNCTION exampleapp.check_password(TEXT,TEXT) IS 'Function to check users passwords';
ALTER FUNCTION exampleapp.check_password(TEXT,TEXT) OWNER TO postgres;


-- Permissions
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA exampleapp FROM public CASCADE;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA exampleapp FROM public CASCADE;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA exampleapp FROM public CASCADE;

GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA exampleapp TO GROUP admin;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA exampleapp TO GROUP admin;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA exampleapp TO GROUP admin;

GRANT USAGE ON SCHEMA exampleapp TO GROUP query_only;
GRANT SELECT ON ALL TABLES IN SCHEMA exampleapp TO GROUP query_only;
GRANT EXECUTE ON ALL FUNCTIONS IN SCHEMA exampleapp TO GROUP query_only;


COMMIT;


