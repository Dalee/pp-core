DELETE FROM sgroup;
DELETE FROM acl_objects;

INSERT INTO sgroup (title, parent, allowed, status) values ('Все', NULL, 'a:1:{s:5:"suser";s:1:"1";}', TRUE);
INSERT INTO sgroup (title, parent, allowed, status) values ('Администраторы', (SELECT id FROM sgroup WHERE title='Все' LIMIT 1), 'a:1:{s:5:"suser";s:1:"1";}', TRUE);

INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what, objectrule) VALUES (NULL, NULL, NULL, NULL, 'read', 'user');
INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what, objectrule) VALUES ((SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1), NULL, NULL, NULL, 'admin',  'user');
INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what, objectrule) VALUES ((SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1), NULL, NULL, NULL, 'write',  'user');

INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what, objectrule) VALUES (NULL, NULL, NULL, NULL, 'read', 'module');
INSERT INTO acl_objects (sgroupid, objectid, objectparent, objecttype, what, objectrule) VALUES ((SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1), NULL, NULL, NULL, 'admin',  'module');

UPDATE suser SET parent=(SELECT id FROM sgroup WHERE title='Администраторы' LIMIT 1) WHERE access=16384;