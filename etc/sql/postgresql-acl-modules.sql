ALTER TABLE acl_objects ADD COLUMN objectrule VARCHAR;
UPDATE acl_objects SET objectrule = 'user';