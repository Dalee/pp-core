DROP TABLE acl_objects;
DROP TABLE sgroup2suser;
ALTER TABLE suser DROP COLUMN parent;
DROP TABLE sgroup;

CREATE TABLE sgroup (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	parent        INT4 DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	allowed       TEXT,

	title         VARCHAR(256),
	description   TEXT,

	status        BOOL
) WITH OIDS;

ALTER TABLE suser ADD COLUMN parent INT4 DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;

CREATE TABLE sgroup2suser (
	sgroupid      INT4 NOT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	suserid       INT4 NOT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE
) WITH OIDS;

CREATE TABLE acl_objects (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	sgroupid      INT4 DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	objectid      INT4,
	objectparent  INT4,
	objecttype    VARCHAR(64),
	what          VARCHAR(64),
	access        VARCHAR(4)
) WITH OIDS;

INSERT INTO sgroup (title, status, allowed) VALUES ('Администраторы', true, 'a:1:{s:5:"suser";i:1;}');
UPDATE suser SET parent = (SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1) WHERE access = 16384;

INSERT INTO acl_objects (what, access) VALUES ('read', 'allo');

INSERT INTO acl_objects (sgroupid, what) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'admin');
INSERT INTO acl_objects (sgroupid, what) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы' LIMIT 1), 'write');

UPDATE acl_objects SET access = 'allo', sys_order = id;