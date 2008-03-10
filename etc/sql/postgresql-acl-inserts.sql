DELETE FROM sgroup;
DELETE FROM acl_objects;

INSERT INTO sgroup (title, parent, allowed, status) values ('Все', NULL, 'a:1:{s:5:"suser";s:1:"1";}', TRUE);
INSERT INTO sgroup (title, parent, allowed, status) values ('Администраторы', (SELECT id FROM sgroup WHERE title='Все' LIMIT 1), 'a:1:{s:5:"suser";s:1:"1";}', TRUE);

INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what) VALUES (NULL, NULL, NULL, NULL, 'read');
INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what) VALUES ((SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1), NULL, NULL, NULL, 'admin');
INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what) VALUES ((SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1), NULL, NULL, NULL, 'write');

UPDATE suser SET parent=(SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1) WHERE access=16384;