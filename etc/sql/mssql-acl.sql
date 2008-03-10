DROP TABLE acl_objects;
DROP TABLE sgroup2suser;
ALTER TABLE suser DROP parent;
DROP TABLE sgroup;

CREATE TABLE sgroup (
	id            INT IDENTITY NOT NULL PRIMARY KEY,
	sys_order     INT,
	parent        INT DEFAULT NULL REFERENCES sgroup,
	sys_owner     INT DEFAULT NULL REFERENCES suser,
	sys_created   DATETIME DEFAULT (GETDATE()),
	sys_modified  DATETIME DEFAULT (GETDATE()),
	sys_accessmod INT,
	sys_accessput INT,
	allowed       TEXT,

	title         VARCHAR(255),
	description   TEXT,

	status        INT
);

ALTER TABLE suser ADD parent INT DEFAULT NULL REFERENCES sgroup ON UPDATE CASCADE;

CREATE TABLE sgroup2suser (
	sgroupid      INT NOT NULL,
	suserid       INT NOT NULL
);

CREATE TABLE acl_objects (
	id            INT IDENTITY NOT NULL PRIMARY KEY,
	sgroupid      INT DEFAULT NULL REFERENCES sgroup ON DELETE CASCADE ON UPDATE CASCADE,
	objectid      INT,
	objectparent  INT,
	objecttype    VARCHAR(64),
	what          VARCHAR(64)
);

INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what) VALUES (NULL, NULL, NULL, NULL, 'read');
