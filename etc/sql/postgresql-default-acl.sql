-- ������ ���: 
DELETE FROM acl_objects;

-- ���� ������ ��� �����������
INSERT INTO acl_objects (objecttype, what, access, objectrule) VALUES ('auth', 'admin', 'allo', 'module');
INSERT INTO acl_objects (objecttype, what, access, objectrule) VALUES ('suser', 'read', 'allo', 'user');

-- ��������� ������� ������� � ������ ���
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'write', 'allo', 'user');
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'read',  'allo', 'user');
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'admin', 'allo', 'user');

-- ��������� ������� ������ �� ���� �������
INSERT INTO acl_objects (sgroupid, what, access, objectrule) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'admin', 'allo', 'module');

-- ��������� ������� ��������� �������� � ����
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'viewmenu', 'allo', 'module', 'main');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'viewmenu', 'allo', 'module', 'auditlog');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'viewmenu', 'allo', 'module', 'acl');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'viewmenu', 'allo', 'module', 'macl');
INSERT INTO acl_objects (sgroupid, what, access, objectrule, objecttype) VALUES ((SELECT id FROM sgroup WHERE title = '��������������'), 'viewmenu', 'allo', 'module', 'objects');