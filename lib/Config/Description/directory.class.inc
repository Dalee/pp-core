<?php
/**
 * ëÌÁÓÓ - ÈÒÁÎÉÌÉÝÅ ÄÁÎÎÙÈ Ï ëÁÔÁÌÏÇÅ (Directory).
 * óÔÏÉÔÓÑ ÐÏ etc/datatypes.xml /directories/directory
 *
 * @subpackage Application
 */
class PXDirectoryDescription {

	var $name;
	var $load;
	var $loaded;
	var $schema;
	var $source;
	var $sourceDir;
	var $displayField;
	var $location;
	var $values;

	public function __construct($name) {
		$this->name   = $name;
		$this->values = array();
		$this->loaded = false;
	}

	public function GetList() {
		$retArray = array();
		foreach ($this->values as $v) {
			$retArray[$v['id']] = $v[$this->displayField];
		}
		return $retArray;
	}

	public function SetLoaded($addDefault = true) {
		if ($addDefault && !isset($this->values[0]) && !isset($this->values[''])) {
			$this->values[''] = array(
				'id' => '',
				$this->displayField => '---'
			);
		}
		$this->loaded = true;
	}
}
?>