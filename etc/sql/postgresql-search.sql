DROP TABLE stems;
DROP TABLE searchdata;
DROP TABLE sitesearch;

DROP SEQUENCE stem_key;

-- ----------------------------------------------------------------------------------------------- --

CREATE SEQUENCE stem_key  START 1 INCREMENT 1 MAXVALUE 2147483647 MINVALUE 1 CACHE 1;

CREATE TABLE stems (
	id          INT4 NOT NULL PRIMARY KEY DEFAULT NEXTVAL('stem_key'),
	stem        VARCHAR(50)
);

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

-- ----------------------------------------------------------------------------------------------- --

CREATE UNIQUE INDEX stems_idx ON stems (stem);
CREATE INDEX sdata_stemid_idx ON searchdata (stemid);
CREATE UNIQUE INDEX sdata_uniq_idx ON searchdata (stemid, did);
