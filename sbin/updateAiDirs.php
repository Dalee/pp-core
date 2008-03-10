#!/usr/local/bin/php -q
<?php
	set_time_limit(0);
	$_SERVER['DOCUMENT_ROOT'] = realpath(dirname(__FILE__).'/../../');

	include_once $_SERVER['DOCUMENT_ROOT'].'/libpp/lib/mainuser.inc';
	$baseDir = BASEPATH.DIRECTORY_SEPARATOR.'site/htdocs/ai/';

	Label('Start');

	/* migrate from datatype.id to datatype/id */
	$files = glob($baseDir.'*.*');

	foreach($files as $f) {
		WorkProgress();
		preg_match('|/(\w+)\.(\d+)$|', $f, $tmp);

		if(sizeof($tmp) !== 3) {
			continue;
		}

		MakeDirIfNotExists($baseDir.$tmp[1]);
		copyr($f, $baseDir.$tmp[1].'/'.$tmp[2]);
		unlinkDir($f);
	}
	WorkProgress(true);

	Label('Done migrate from datatype.id to datatype/id');
	

	/* migrate from datatype/id/field.jpg to datatype/id/field/field.jpg */	
	$app     = new PXApplication(BASEPATH);

	$fields = array();

	foreach($app->types as $type) {
		foreach($type->fields as $field) {
			switch($field->storageType) {
				case 'image':
				case 'imagesarray':
				case 'flash':
				case 'flashsarray':
					break;

				default:
					continue(2);
					break;
			}

			$fields[] = array(
				'type'  => $type->id,
				'field' => $field->name
			);
		}
	}

	foreach($fields as $field) {
		$rule = $baseDir.$field['type'].'/*/'.$field['field'].'.*';
		$files = glob($rule);

		if(!is_array($files)) {
			continue;
		}
		foreach($files as $f) {
			WorkProgress(false, sizeof($files));
			preg_match('|^(.+)/([^\/]+)$|', $f, $tmp);

			if(sizeof($tmp) !== 3) {
				continue;
			}

			$newDir = $tmp[1].'/'.$field['field'].'/';

			MakeDirIfNotExists($newDir);
			copyr($f, $newDir.$tmp[2]);
			unlink($f);
		}

		WorkProgress(true);
	}

	Label('Done migrate from datatype/id/field.jpg to datatype/id/field/field.jpg');
?>
