<?php

use PP\Lib\Datastruct\Tree;

/**
 * Класс - хранилище данных о Каталоге (Directory).
 * Стоится по etc/datatypes.xml /directories/directory
 *
 * @subpackage Application
 */
class PXDirectoryDescription {
	public $name;
	public $load;
	public $loaded;
	public $schema;
	public $source;
	public $sourceDir;
	public $displayField;
	public $parentField;
	public $location;
	public $values;
	public $filter;

	public $sourceKeyField;
	public $sourceDisplayField;
	public $sourceItem;
	public $sourceDataFields;

	public function __construct($name) {
		$this->name        = $name;
		$this->values      = [];
		$this->filter      = [];
		$this->loaded      = false;
		$this->parentField = false;
	}

	public function getList() {
		$retArray = [];

		foreach ($this->values as $v) {
			$retArray[$v['id']] = $v[$this->displayField];
		}

		return $retArray;
	}

	public function GetTree($saveOrphans = false) {
		$retArray = [];

		if (count($this->values) && $this->parentField !== false) {
			$retArray = new Tree($this->values, 'id', $this->parentField, 'title', $saveOrphans);
		}

		return $retArray;
	}

	public function SetLoaded($addDefault = true) {
		if ($addDefault && !isset($this->values[0]) && !isset($this->values[''])) {
			$this->values[''] = [
				'id' => '',
				$this->displayField => '---'
            ];
		}

		$this->loaded = true;
	}
}
