<?php

class PXStorageTypeBlockcontent extends PXStorageTypeVirtual {

	public function normalize($field, $object, &$param) {
		$ownerType   = $param['format'];
		$ownerField  = $field->name;
		$ownerId     = $object['id'];

		return new PXBlockContentLazyLoader($ownerType, $ownerId, $ownerField);
	}
}
