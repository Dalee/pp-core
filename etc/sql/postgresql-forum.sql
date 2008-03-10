DROP TABLE forumblocking;
DROP TABLE forummessage;
DROP TABLE forumtopic;

CREATE TABLE forumtopic (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	parent        INT4 NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	pid           INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	allowed       TEXT,
	title         VARCHAR(255),
	pathname      VARCHAR(128),
	lastauthor    INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	lastreply     TIMESTAMP,
	count         INT4,
	status        BOOL
);

CREATE TABLE forummessage (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	parent        INT4 NOT NULL REFERENCES forumtopic ON DELETE CASCADE ON UPDATE CASCADE,
	pid           INT4,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	title         VARCHAR(255),
	pathname      VARCHAR(128),
	body          TEXT,
	status        BOOL
);

CREATE TABLE forumblocking (
	id            INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order     INT4,
	sys_owner     INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created   TIMESTAMP DEFAULT now(),
	sys_modified  TIMESTAMP DEFAULT now(),
	sys_accessmod INT4,
	sys_accessput INT4,
	title         VARCHAR(255),
	ip            INT4,
	mask          INT4,
	status        BOOL
);

ALTER TABLE forumtopic ADD is_last boolean;
ALTER TABLE forumtopic ADD pinned boolean;
ALTER TABLE forumtopic ADD locked boolean;

UPDATE forumtopic SET is_last = false where is_last is null;
UPDATE forumtopic SET pinned = false where pinned is null;
UPDATE forumtopic SET locked = false where locked is null;

ALTER TABLE forummessage ADD ip integer;


-- ALTER TABLE suser ADD data text;
-- Добавить поле в котором будет храниться всякая информация
-- в PP уже есть в базе но не описано в datatypes.xml