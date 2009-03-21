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
	
	/** 
		@var bool Может ли объект A иметь более одной связи с объектом B по какому-либо дополнительному признаку ? 
		@see PXApplication::FillDataTypes()
	*/
	var $multiple   = false;
	/** 
		@var bool По-умолчанию отображать в форме редактирования ссылок все объекты или только привязанные ?
		@see PXApplication::_mapRefByDefaults()
	*/
	var $byDefault  = false;
	/** 
		@var bool Скрывать или нет конкретный тип связи в форме редактирования ссылок ? Используется для служебных связей.
		@see PXApplication::FillDataTypes()
	*/
	var $hidden     = false;

	public function addField($field) {
		$this->fields[$field->name] = $field;
	}

	//fixme: find better name
	public function getOther($formatName) {
		return (($this->from == $formatName) ? $this->to : $this->from);
	}
}
?>