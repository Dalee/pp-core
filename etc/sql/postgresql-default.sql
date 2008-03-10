DROP TABLE city;
DROP TABLE sample;
DROP TABLE news;
DROP TABLE article;
DROP TABLE struct;
DROP TABLE suser;

DROP SEQUENCE main_key;

CREATE SEQUENCE main_key START 1 INCREMENT 1 MAXVALUE 2147483647 MINVALUE 1 CACHE 1;

CREATE TABLE suser (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,

	title         VARCHAR(64),
	passwd        VARCHAR(64),
	realname      VARCHAR(256),
	email         VARCHAR(256),
	access        INT4 DEFAULT 0,
	data          TEXT,

	status        BOOL
) WITH OIDS;

CREATE TABLE struct (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	parent        INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,

	title         VARCHAR(256),
	pathname      VARCHAR(128),
	type          VARCHAR(16),
	description   TEXT,
	allowed       TEXT,

	status        BOOL
) WITH OIDS;

CREATE TABLE article (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	parent        INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	title         VARCHAR(256),
	pathname      VARCHAR(128),
	text          TEXT,
	index         BOOL,
	status        BOOL
) WITH OIDS;

CREATE TABLE news (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	parent        INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	title         VARCHAR(256),
	date          TIMESTAMP,
	pathname      VARCHAR(128),
	anons         TEXT,
	text          TEXT,
	index         BOOL,
	status        BOOL
) WITH OIDS;

CREATE TABLE sample (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order   INT4,
	parent      INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	title       VARCHAR(256),
	pathname    VARCHAR(128),
	ip          INT4,
	ts          TIMESTAMP,
	status      BOOL
);

CREATE TABLE city (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order   INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	title       VARCHAR(256),
	status      BOOL
);

CREATE TABLE forummessage (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order   INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	parent      INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	pid         INT4,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	sys_accessput   INT4,
	title       VARCHAR(255),
	pathname    VARCHAR(128),
	body        TEXT,
	status      BOOL
);

CREATE TABLE sample2 (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order   INT4,
	parent      INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	title       VARCHAR(256),
	pathname    VARCHAR(128),
	cityid      INT4 DEFAULT NULL REFERENCES city ON DELETE SET NULL ON UPDATE CASCADE,
	status      BOOL
);

CREATE TABLE debts (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order   INT4,
	parent      INT4 DEFAULT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,

	title       VARCHAR(64),
	data        TIMESTAMP DEFAULT now(),
	sumtotal    INT4 DEFAULT 0,
	surcharge   INT4 DEFAULT 0,
	status      BOOL
);

INSERT INTO suser (title, passwd, realname, access, status) VALUES ('admin', '1010', 'Site Administrator', 16384, TRUE);
