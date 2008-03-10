DROP TABLE forummessage;
DROP TABLE forumtopic;

CREATE TABLE forumtopic (
	id              INT IDENTITY NOT NULL PRIMARY KEY,
	parent          INT NOT NULL REFERENCES struct ON DELETE CASCADE ON UPDATE CASCADE,
	pid             INT,

	sys_order       INT,
	sys_owner       INT DEFAULT NULL REFERENCES suser,
	sys_created     DATETIME DEFAULT (GETDATE()),
	sys_modified    DATETIME DEFAULT (GETDATE()),
	sys_accessmod   INT,
	sys_accessput   INT,
	allowed         TEXT,

	title           VARCHAR(255),
	pathname        VARCHAR(128),
	lastauthor      INT DEFAULT NULL REFERENCES suser,
	lastreply       DATETIME DEFAULT (GETDATE()),
	count           INT,
	status          INT
);

CREATE TABLE forummessage (
	id              INT IDENTITY NOT NULL PRIMARY KEY,
	parent          INT NOT NULL REFERENCES forumtopic ON DELETE CASCADE ON UPDATE CASCADE,
	pid             INT,

	sys_order       INT,
	sys_owner       INT DEFAULT NULL REFERENCES suser,
	sys_created     DATETIME DEFAULT (GETDATE()),
	sys_modified    DATETIME DEFAULT (GETDATE()),
	sys_accessmod   INT,
	sys_accessput   INT,

	title           VARCHAR(255),
	pathname        VARCHAR(128),
	body            TEXT,
	status          INT
);


ALTER TABLE forumtopic ADD is_last INT;
ALTER TABLE forumtopic ADD pinned  INT;
ALTER TABLE forumtopic ADD locked  INT;

UPDATE forumtopic SET is_last = '0' WHERE is_last IS NULL;
UPDATE forumtopic SET pinned  = '0' WHERE pinned  IS NULL;
UPDATE forumtopic SET locked  = '0' WHERE locked  IS NULL;

-- ALTER TABLE suser ADD data text;
-- Добавить поле в котором будет храниться всякая информация
-- в PP уже есть в базе но не описано в datatypes.xml