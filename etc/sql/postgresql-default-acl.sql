-- чистим все: 
DELETE FROM acl_objects;

-- даем доступ для авторизации
INSERT INTO acl_objects (objecttype, what, access, objectrule) VALUES ('auth', 'admin', 'allo', 'module');
INSERT INTO acl_objects (objecttype, what, access, objectrule) VALUES ('suser', 'read', 'allo', 'user');

-- разрешаем админам править и читать все
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'write', 'allo', 'user');
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'read',  'allo', 'user');
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'admin', 'allo', 'user');

-- разрешаем админам доступ ко всем модулям
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'admin', 'allo', 'module');

-- открываем админам некоторые закладки в меню
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allo', 'module', 'main');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allo', 'module', 'auditlog');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allo', 'module', 'acl');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allo', 'module', 'macl');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = 'Администраторы'), 'viewmenu', 'allo', 'module', 'objects');