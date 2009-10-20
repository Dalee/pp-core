ALTER TABLE acl_objects ADD objectrule VARCHAR;
UPDATE acl_objects SET objectrule = 'user' WHERE objectrule IS NULL;
INSERT INTO acl_objects (objecttype, what, objectrule) VALUES ('auth', 'admin', 'module');

INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'admin', 'allow', 'module');
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allow', 'module');
