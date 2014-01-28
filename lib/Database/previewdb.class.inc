<?php

require_once 'database.class.inc';

class PXPreviewDB extends PXDatabase {

	protected function _GetData($format, $status, $mode, $where=NULL, $limit=NULL, $order=NULL, $dontUseCache = false) {
		return parent::_GetData($format, null, $mode, $where, $limit, $order, $dontUseCache);
	}

}
