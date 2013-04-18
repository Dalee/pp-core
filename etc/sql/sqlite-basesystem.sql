-- 001_default.sql
CREATE TABLE suser (
	id            INTEGER PRIMARY KEY AUTOINCREMENT,
	sys_order     INTEGER,
	sys_created   TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	sys_modified  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

	title         VARCHAR,
	passwd        VARCHAR,
	realname      VARCHAR,

	status        INTEGER
);

CREATE TABLE sgroup (
	id            INTEGER PRIMARY KEY AUTOINCREMENT,
	sys_owner     INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_order     INTEGER,
	sys_created   TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	sys_modified  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	allowed       VARCHAR,

	title         VARCHAR,

	status        INTEGER
);

ALTER TABLE suser  ADD COLUMN parent INTEGER DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;
ALTER TABLE sgroup ADD COLUMN parent INTEGER DEFAULT NULL REFERENCES sgroup ON DELETE SET NULL ON UPDATE CASCADE;

-- 1e48c4420b7073bc11916c6c1de226bb = md5('1010')
INSERT INTO suser (title, passwd, realname, status) VALUES ('admin', '1e48c4420b7073bc11916c6c1de226bb', 'Site Administrator', 1);

CREATE TABLE sgroup2suser (
	sgroupid      INTEGER NOT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	suserid       INTEGER NOT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE acl_objects (
	id            INTEGER PRIMARY KEY AUTOINCREMENT,

	sys_order     INTEGER,
	sgroupid      INTEGER DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	objectid      INTEGER,
	objectparent  INTEGER,

	objecttype    VARCHAR,
	what          VARCHAR,
	access        VARCHAR,
	objectrule	  VARCHAR
);

INSERT INTO sgroup (title, status, allowed) VALUES ('Администраторы', 1, 'a:1:{s:5:"suser";i:1;}');
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
	id              INTEGER PRIMARY KEY,
	parent          INTEGER DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	
	sys_order       INTEGER,
	sys_owner       INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	sys_modified    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	allowed		    TEXT,
	
	title           VARCHAR,
	pathname        VARCHAR,
	type            VARCHAR,
	
	status          INTEGER
);

CREATE TABLE html (
	id              INTEGER PRIMARY KEY,
	parent          INTEGER NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	
	sys_order       INTEGER,
	sys_owner       INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	sys_modified    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	
	title           VARCHAR,
	pathname        VARCHAR,
	"text"          TEXT,
	"index"         INTEGER DEFAULT 1,
	
	status          INTEGER
);

-- 002_auditlog.sql
CREATE TABLE log_audit (
	id      INTEGER PRIMARY KEY AUTOINCREMENT,
	ts      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	"level" INTEGER,
	"type"  CHAR(8),
	source  VARCHAR,
	"user"  VARCHAR,
	ip      VARCHAR,
	description VARCHAR
);

--CREATE INDEX source_idx ON log_audit(split_part(source, '/', 2));
CREATE INDEX type_idx ON log_audit (type);
--CREATE INDEX data_idx ON log_audit(date_trunc('day', ts));
CREATE VIEW log_audit_view AS SELECT "id", "ts", date(ts, 'DD.MM.YYYY') as "date", "level", "type", "source", "user", "ip", "description" FROM log_audit;

-- 003_search.sql
CREATE TABLE stems (
	id          INTEGER PRIMARY KEY AUTOINCREMENT,
	stem        VARCHAR
);
CREATE TABLE searchdata (
	stemid      INTEGER NOT NULL,
	did         INTEGER NOT NULL,
	dtype       VARCHAR,
	weightsum   INTEGER NOT NULL
);
CREATE TABLE sitesearch (
	stem        VARCHAR NOT NULL,
	did         INTEGER NOT NULL,
	dtype       VARCHAR NOT NULL,
	weightsum   INTEGER NOT NULL
);
-- ---------------------------------------------------------------------------------------------
CREATE UNIQUE INDEX stems_idx ON stems (stem);
CREATE INDEX sdata_stemid_idx ON searchdata (stemid);
CREATE UNIQUE INDEX sdata_uniq_idx ON searchdata (stemid, did);
-- ---------------------------------------------------------------------------------------------
ALTER TABLE struct ADD "index" INTEGER DEFAULT 1;

-- 004_suser_session
CREATE TABLE suser_session (
	id            INTEGER PRIMARY KEY AUTOINCREMENT,
	suser_id      INTEGER DEFAULT NULL REFERENCES suser ON DELETE CASCADE ON UPDATE CASCADE,
	mtime         TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	sid           VARCHAR,
	ip            VARCHAR
);
