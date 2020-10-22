-- 001_default.sql
CREATE TABLE suser (
	id            SERIAL PRIMARY KEY,
	sys_order     INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_meta      VARCHAR,
	sys_uuid      VARCHAR(36),

	title         VARCHAR,
	email         VARCHAR,
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
	sys_meta      VARCHAR,
	sys_uuid      VARCHAR(36),
	allowed       VARCHAR,

	title         VARCHAR,

	status        BOOL
) WITH OIDS;

ALTER TABLE suser  ADD COLUMN parent INT4 DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;
ALTER TABLE sgroup ADD COLUMN parent INT4 DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;

INSERT INTO suser (title, email, passwd, realname, status, sys_uuid)
VALUES (
	'admin',
	'admin@localhost.local',
	-- bcrypt hash '1010'
	'$2y$10$0x7cRFlHmuDin.rjiMropue1CGmKxO4q4.i7eX/nlWlKZbehoEa/O',
	'Администратор',
	true,
	'0466909f-2e37-4f01-969a-7ef68ec23976'
);

CREATE TABLE sgroup2suser (
	sgroupid      INT4 NOT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	suserid       INT4 NOT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE
) WITH OIDS;

-- sys_property
CREATE TABLE sys_properties (
	id            SERIAL PRIMARY KEY,

	sys_order     INT4,
	sys_uuid      VARCHAR(36),

	"name"        VARCHAR UNIQUE,
	description   VARCHAR,
	"value"       VARCHAR
) WITH OIDS;

CREATE TABLE acl_objects (
	id            SERIAL PRIMARY KEY,

	sys_order     INT4,
	sys_uuid      VARCHAR(36),
	sgroupid      INT4 DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	objectid      INT4,
	objectparent  INT4,

	objecttype    VARCHAR,
	what          VARCHAR,
	access        VARCHAR,
	objectrule	  VARCHAR
) WITH OIDS;

INSERT INTO sgroup (title, status, allowed, sys_uuid)
VALUES (
	'Администраторы',
	true,
	'a:1:{s:5:"suser";i:1;}',
	'a44636d6-c455-4721-a4ac-dc22bc185dbd'
);

UPDATE suser SET parent = (SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1);

-- PROPERTIES
INSERT INTO sys_properties (description, "name", "value", sys_uuid)
VALUES (
	'Отладочный режим (отключить в production)',
	'ENVIRONMENT',
	'DEVELOPER',
	'16b33fe4-f24f-4a24-8c67-482f9e15c092'
);

-- ACL
INSERT INTO acl_objects (what, access, objectrule, sys_uuid)
VALUES (
	'read',
	'allo',
	'user',
	'b4c23701-c8f2-45b6-adb5-e7c62d5e0709'
);

INSERT INTO acl_objects (sgroupid, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'admin',
	'allo',
	'user',
	'ff7aac6f-6c1e-4b82-a4b3-bd540d0f63ad'
);

INSERT INTO acl_objects (sgroupid, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'write',
	'allo',
	'user',
	'9c7e586a-50d9-40cb-a07c-5ac4d37fbcf1'
);

INSERT INTO acl_objects (sgroupid, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'admin',
	'allo',
	'module',
	'6e56c112-496c-4256-88ea-81a7c85b8118'
);

INSERT INTO acl_objects (objecttype, what, access, objectrule, sys_uuid)
VALUES (
	'auth',
	'admin',
	'allo',
	'module',
	'89c532e2-639e-4b78-9745-065c4d39fcd7'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'main',
	'viewmenu',
	'allo',
	'module',
	'84222706-6a54-4333-9b94-63582abb73d2'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'users',
	'viewmenu',
	'allo',
	'module',
	'fe935eb3-5606-4afb-8dfa-cebd231aa215'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'acl',
	'viewmenu',
	'allo',
	'module',
	'170999b2-4fd4-4c79-a934-dd4ad6b64e1a'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'macl',
	'viewmenu',
	'allo',
	'module',
	'8d48db14-9c14-4186-ae5a-167559c024b2'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'objects',
	'viewmenu',
	'allo',
	'module',
	'e0eb8142-e641-4fba-aa6f-4e6b9b416ce0'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'properties',
	'viewmenu',
	'allo',
	'module',
	'a52895f4-b74f-40b9-8f5a-04419c6a09ba'
);

INSERT INTO acl_objects (sgroupid, objecttype, what, access, objectrule, sys_uuid)
VALUES (
	(SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1),
	'properties',
	'sys_properties_edit',
	'allo',
	'module',
	'e456b476-c3d4-4831-af5e-307361f0abc8'
);


UPDATE acl_objects SET sys_order = id;

CREATE TABLE struct (
	id            SERIAL PRIMARY KEY,
	parent        INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,

	sys_order     INT4,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_meta      VARCHAR,
	sys_uuid      VARCHAR(36),
	allowed		  TEXT,

	title         VARCHAR,
	pathname      VARCHAR,
	type          VARCHAR,

	status        BOOL
) WITH OIDS;

CREATE TABLE html (
	id            SERIAL PRIMARY KEY,
	parent        INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,

	sys_order     INT4,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_meta      VARCHAR,
	sys_uuid      VARCHAR(36),

	title         VARCHAR,
	pathname      VARCHAR,
	text          TEXT,
	index         BOOL DEFAULT TRUE,

	status        BOOL
) WITH OIDS;

-- auditlog
CREATE TABLE log_audit (
	id      SERIAL PRIMARY KEY,
	ts      TIMESTAMP DEFAULT NOW(),
	"level" INT4,
	"type"  CHAR(8),
	source  VARCHAR(128),
	"user"  VARCHAR(128),
	ip      INET,
	description VARCHAR,
	diff	TEXT
) WITH OIDS;

CREATE INDEX source_idx ON log_audit(split_part(source, '/', 2));
CREATE INDEX type_idx ON log_audit (type);
CREATE INDEX data_idx ON log_audit(date_trunc('day', ts));
CREATE INDEX log_audit_ip_idx ON log_audit(ip);
CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts::text, 'DD.MM.YYYY'::text) as date, "level", "type", "source", "user", "ip", "description", "diff" FROM log_audit;

-- search
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

-- real sessions in admin
CREATE TABLE admin_session (
	session_id       VARCHAR(128) NOT NULL PRIMARY KEY,
	session_data     TEXT NOT NULL,
	session_lifetime INTEGER NOT NULL,
	session_time     INTEGER NOT NULL
);

CREATE TABLE queue_job (
	id           SERIAL PRIMARY KEY,
	sys_order    INTEGER,
	sys_owner    INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created  TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	sys_modified TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	sys_uuid     VARCHAR(36),
	sys_meta     VARCHAR,

	title        VARCHAR(150),
	payload      JSON,
	state        VARCHAR DEFAULT 'fresh',
	status       BOOLEAN DEFAULT true,
	result       JSON
) WITH OIDS;

-- migrations support
CREATE TABLE _migrations (
	id       SERIAL PRIMARY KEY,
	filename VARCHAR
) WITH OIDS;
