<?php

class PXStorageTypeBlockcontent extends PXStorageTypeVirtual {

	public function normalize($field, $object, &$param) {
		$db = $param['db'];
		$app = $db->app;

		$ownerType   = $param['format'];
		$ownerField  = $field->name;
		$ownerId     = $object['id'];

		return new PXBlockContentLazyLoader($app, $db, $ownerType, $ownerId, $ownerField);
	}
}
