#!/usr/local/bin/php -q
<?php
	set_time_limit(0);
	$_SERVER['DOCUMENT_ROOT'] = realpath(dirname(__FILE__).'/../../');

	include_once $_SERVER['DOCUMENT_ROOT'].'/libpp/lib/mainuser.inc';

	Label('Start');
	$engine = new PXEngineSbin();
	$engine->init();

	$app =& $engine->app;
	$db  =& $engine->db;

	$types = array();

	foreach($app->types as $typeId=>$type) {
		foreach($type->fields as $field) {
			if ($field->displayType !== 'LINKTOFILE') {
				if($field->name !== 'id') {
					unset($app->types[$typeId]->fields[$field->name]);
				}

				continue;
			}

			$types[$type->id][] = $field->name;
		}
	}

	foreach($types as $format=>$fields) {
		$type = $app->types[$format];

		$where = array();
		foreach($fields as $f) {
			$where[] = '('.$f.' IS NOT NULL AND '.$f." NOT LIKE 'a:0:{}')";
		}

		$objects = $db->getObjectsByWhere($type, null, implode(' AND ', $where));

		$modify = 0;
		foreach($objects as $o) {
			WorkProgress();

			$sql = array();

			foreach($fields as $f) {
				$old = serialize($o[$f]);

				$o[$f]['dir'] = preg_replace('|(^[^/])|', '/\\1', $o[$f]['dir']);
				$o[$f]['dir'] = preg_replace('|([^/]$)|', '\\1/', $o[$f]['dir']);

				$new = serialize($o[$f]);

				if($new == $old) {
					continue;
				}

				$sql[] = $f." = '".$new."'";
			}

			if(!sizeof($sql)) {
				continue;
			}

			$query = 'UPDATE '.$format.' SET '.implode(', ', $sql).' WHERE id = '.$o['id'];

			$db->modifyingQuery($query);
			$modify++;
		}
		WorkProgress(true);

		Label(sizeof($objects).' '.$format.', '.$modify.' modify');
	}

	Label('Done');
?>
