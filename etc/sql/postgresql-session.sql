CREATE TABLE suser_session (
	id            SERIAL PRIMARY KEY,
	suser_id      INT4 DEFAULT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE,
	mtime         TIMESTAMP DEFAULT now(),
	sid           VARCHAR(32) UNIQUE,
	ip            INET
) WITH OIDS;