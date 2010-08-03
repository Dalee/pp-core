DROP SEQUENCE log_key;
DROP TABLE log_audit;

CREATE SEQUENCE log_key START 1 INCREMENT 1 MINVALUE 1 CACHE 1;
CREATE TABLE log_audit (
	id      INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('log_key'),
	ts      TIMESTAMP DEFAULT NOW(),
	"level" INT4,
	"type"  CHAR(8),
	source  VARCHAR(128),
	"user"  VARCHAR(32),
	ip      VARCHAR(32),
	description VARCHAR(256)
);

CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts::text, 'DD.MM.YYYY') as date, "level", "type", "source", "user", "ip", "description" FROM log_audit;
