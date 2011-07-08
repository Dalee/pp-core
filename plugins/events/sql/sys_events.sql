create table sys_events (
	id serial,
	callback  text,
	params    text,
	sys_created timestamp default now()
) with oids;
