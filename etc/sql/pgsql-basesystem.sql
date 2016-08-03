-- 001_default.sql
CREATE TABLE suser (
	id            SERIAL PRIMARY KEY,
	sys_order     INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),

	title         VARCHAR,
	passwd        VARCHAR,
	realname      VARCHAR,

	status        BOOL
) WITH OIDS;

CREATE TABLE sgroup (
	id            SERIAL PRIMARY KEY,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_order     INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	allowed       VARCHAR,

	title         VARCHAR,

	status        BOOL
) WITH OIDS;

ALTER TABLE suser  ADD COLUMN parent INT4 DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;
ALTER TABLE sgroup ADD COLUMN parent INT4 DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;

INSERT INTO suser (title, passwd, realname, status) VALUES ('admin', md5('1010'), 'Site Administrator', TRUE);

CREATE TABLE sgroup2suser (
	sgroupid      INT4 NOT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	suserid       INT4 NOT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE
) WITH OIDS;

CREATE TABLE acl_objects (
	id            SERIAL PRIMARY KEY,

	sys_order     INT4,
	sgroupid      INT4 DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	objectid      INT4,
	objectparent  INT4,

	objecttype    VARCHAR,
	what          VARCHAR,
	access        VARCHAR,
	objectrule	  VARCHAR
) WITH OIDS;

INSERT INTO sgroup (title, status, allowed) VALUES ('Администраторы', true, 'a:1:{s:5:"suser";i:1;}');
UPDATE suser SET parent = (SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1);

INSERT INTO acl_objects (what, access, objectrule)
	VALUES ('read', 'allo', 'user');

INSERT INTO acl_objects (sgroupid, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'admin', 'allo', 'user');

INSERT INTO acl_objects (sgroupid, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'write', 'allo', 'user');

INSERT INTO acl_objects (sgroupid, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'admin', 'allo', 'module');

INSERT INTO acl_objects (objecttype, what, access, objectrule)
	VALUES ('auth', 'admin', 'allo', 'module');

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'main', 'viewmenu', 'allo', 'module');

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'users', 'viewmenu', 'allo', 'module');

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'acl', 'viewmenu', 'allo', 'module');

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'macl', 'viewmenu', 'allo', 'module');

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule)
	VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'objects', 'viewmenu', 'allo', 'module');


UPDATE acl_objects SET sys_order = id;

CREATE TABLE struct (
	id              SERIAL PRIMARY KEY,
	parent          INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,

	sys_order       INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	allowed		    TEXT,

	title           VARCHAR,
	pathname        VARCHAR,
	type            VARCHAR,

	status          BOOL
) WITH OIDS;

CREATE TABLE html (
	id              SERIAL PRIMARY KEY,
	parent          INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,

	sys_order       INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),

	title           VARCHAR,
	pathname        VARCHAR,
	text            TEXT,
	index           BOOL DEFAULT TRUE,

	status          BOOL
) WITH OIDS;

-- 002_auditlog.sql
CREATE TABLE log_audit (
	id      SERIAL PRIMARY KEY,
	ts      TIMESTAMP DEFAULT NOW(),
	"level" INT4,
	"type"  CHAR(8),
	source  VARCHAR(128),
	"user"  VARCHAR(32),
	ip      VARCHAR(32),
	description VARCHAR,
	diff	TEXT
) WITH OIDS;

CREATE INDEX source_idx ON log_audit(split_part(source, '/', 2));
CREATE INDEX type_idx ON log_audit (type);
CREATE INDEX data_idx ON log_audit(date_trunc('day', ts));
CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts::text, 'DD.MM.YYYY'::text) as date, "level", "type", "source", "user", "ip", "description", "diff" FROM log_audit;

-- 003_search.sql
CREATE SEQUENCE stem_key  START 1 INCREMENT 1 MAXVALUE 2147483647 MINVALUE 1 CACHE 1;
CREATE TABLE stems (
	id          SERIAL PRIMARY KEY,
	stem        VARCHAR(50)
) WITH OIDS;
CREATE TABLE searchdata (
	stemid      INT4 NOT NULL,
	did         INT4 NOT NULL,
	dtype       VARCHAR(20),
	weightsum   INT4 NOT NULL
);
CREATE TABLE sitesearch (
	stem        VARCHAR(30) NOT NULL,
	did         INT4 NOT NULL,
	dtype       VARCHAR(20) NOT NULL,
	weightsum   INT4 NOT NULL
);
------------------------------------------------------------------------------------------------- --
CREATE UNIQUE INDEX stems_idx ON stems (stem);
CREATE INDEX sdata_stemid_idx ON searchdata (stemid);
CREATE UNIQUE INDEX sdata_uniq_idx ON searchdata (stemid, did);
------------------------------------------------------------------------------------------------- --
ALTER TABLE struct ADD index BOOL DEFAULT TRUE;

-- 004_suser_session
-- deprecated, TODO: refactor PP\Lib\Auth\Session
CREATE TABLE suser_session (
	id            SERIAL PRIMARY KEY,
	suser_id      INT4 DEFAULT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE,
	mtime         TIMESTAMP DEFAULT now(),
	sid           VARCHAR(32) UNIQUE,
	ip            INET
) WITH OIDS;

-- new session mechanic, real sessions in admin
CREATE TABLE admin_session (
	session_id VARCHAR(128) NOT NULL PRIMARY KEY,
	session_data TEXT NOT NULL,
	session_lifetime INTEGER NOT NULL,
	session_time INTEGER NOT NULL
);

CREATE TABLE queue_job (
	id SERIAL PRIMARY KEY,
	sys_order INTEGER,
	sys_owner INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	sys_modified TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	status BOOLEAN DEFAULT true,

	worker VARCHAR(150),
	payload JSON,
	state VARCHAR DEFAULT 'fresh'
) WITH OIDS;
