<?php
/**
 * Класс описание ссылок между типами данных.
 * стоится по etc/datatype.xml /model/references/reference/
 *
 * @subpackage Application
 */
class PXRefDescription {
	var $name       = NULL;
	var $from       = NULL;
	var $to         = NULL;

	var $filterFrom = NULL;
	var $filterTo   = NULL;

	var $fields     = array();
	var $restrictBy = NULL;

	var $load       = 'automatic';
	
	var $multiple   = false;
	var $byDefault  = false;

	public function addField($field) {
		$this->fields[$field->name] = $field;
	}

	//fixme: find better name
	public function getOther($formatName) {
		return (($this->from == $formatName) ? $this->to : $this->from);
	}
}
?>