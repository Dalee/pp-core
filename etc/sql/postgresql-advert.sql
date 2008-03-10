DROP TABLE adbanner2adplace;
DROP TABLE adbanner;
DROP TABLE adplace;
DROP TABLE adcampaign;
DROP TABLE adstat;
DROP SEQUENCE adstat_key;

CREATE SEQUENCE adstat_key INCREMENT BY 1 NO MAXVALUE NO MINVALUE CACHE 1;

CREATE TABLE adstat (
	id       INTEGER NOT NULL PRIMARY KEY DEFAULT NEXTVAL('adstat_key'),
	ts       TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	adbanner INTEGER,
	adplace  INTEGER,
	"show"   INTEGER,
	click    INTEGER
);


CREATE TABLE adplace (
	id              INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order       INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	sys_accessput   INT4,
	title           VARCHAR(256),
	type            VARCHAR(128),
	status          BOOL
);

CREATE TABLE adcampaign (
	id              INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order       INT4,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	sys_accessput   INT4,
	allowed         VARCHAR(1024),

	title           VARCHAR(256),
	status          BOOL
);

CREATE TABLE adbanner (
	id              INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('main_key'),
	sys_order       INT4,
	parent          INT4 NOT NULL REFERENCES adcampaign ON DELETE CASCADE ON UPDATE CASCADE,
	sys_owner       INT4 DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE,
	sys_created     TIMESTAMP DEFAULT now(),
	sys_modified    TIMESTAMP DEFAULT now(),
	sys_accessmod   INT4,
	title           VARCHAR(256),
	type            VARCHAR(128),
	dynamic         BOOLEAN,
	weight          INT4,
	body            TEXT,
	reference       VARCHAR(256),
	video           VARCHAR(256),
	bgcolor         VARCHAR(6),
	shows           INT4,
	clicks          INT4,
	status          BOOL
);

CREATE TABLE adbanner2adplace (
	adbannerid   INT4 NOT NULL REFERENCES adbanner ON DELETE CASCADE ON UPDATE CASCADE,
	adplaceid    INT4 NOT NULL REFERENCES adplace ON DELETE CASCADE ON UPDATE CASCADE
);

-- ALTER TABLE struct ADD COLUMN adplaceid1 INT4;
-- ALTER TABLE struct ALTER COLUMN adplaceid1 SET DEFAULT NULL;
-- ALTER TABLE struct ADD CONSTRAINT "fkadplace1" FOREIGN KEY (adplaceid1) REFERENCES adplace(id)
-- ON UPDATE CASCADE ON DELETE SET DEFAULT;

-- ALTER TABLE struct ADD COLUMN adplaceid2 INT4;
-- ALTER TABLE struct ALTER COLUMN adplaceid2 SET DEFAULT NULL;
-- ALTER TABLE struct ADD CONSTRAINT "fkadplace2" FOREIGN KEY (adplaceid2) REFERENCES adplace(id)
-- ON UPDATE CASCADE ON DELETE SET DEFAULT;

ALTER TABLE adcampaign ADD COLUMN parent INT4;
ALTER TABLE adcampaign ALTER COLUMN parent SET DEFAULT NULL;
ALTER TABLE adcampaign ADD CONSTRAINT "fkparent" FOREIGN KEY (parent) REFERENCES adcampaign(id)
ON UPDATE CASCADE ON DELETE SET DEFAULT;
