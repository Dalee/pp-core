<?php
/**
* Класс - хранилище.
* Стоится по etc/datatype.xml /model/datatypes/datatype
*
* @subpackage Application
*/
class PXTypeDescription {
	var $id            = NULL;
	var $title         = NULL;
	var $parent        = NULL;
	var $childs        = array();
	var $references    = array();
	var $order         = NULL;
	var $fields        = array();
	var $sTypes        = array();
	var $dTypes        = array();
	var $struct        = 'plain';
	var $byDefault     = NULL;
	var $outputHandler = NULL;

	var $allTypes      = array();

	public function addField($field) {
		$this->fields[$field->name] = $field;
	}

	/**
	 * Возвращает массив типов потомков для переданного формата
	 */
	function childTypes() {
		$cTypes = array();

		if(count($this->childs) > 0){
			foreach ($this->childs as $v) {
				if(!isset($this->allTypes[$v])) {
					continue;
				}

				if (!is_object($this->allTypes[$v])){
					continue;
				}

				if ($v === $this->id){
					continue;
				}

				$cTypes[$v] =& $this->allTypes[$v];
			}
		}

		return $cTypes;
	}

	function allowedChilds($object) {
		$cTypes = $this->childTypes();

		$ac = array();
		if (is_object($object)) {
			$ac = isset($object->content[OBJ_FIELD_CHILDREN]) ? $object->content[OBJ_FIELD_CHILDREN] : array();
		}

		if (is_array($object)) {
			$ac = isset($object[OBJ_FIELD_CHILDREN]) ? $object[OBJ_FIELD_CHILDREN] : array();
		}

		foreach ($ac as $k=>$v) {
			if (!isset($this->allTypes[$k])) {
				unset($ac[$k]);
			}
		}

		return $ac;
	}

	function allowedChildTypes(&$object) {
		$allowed = $this->allowedChilds($object);

		foreach($allowed as $format=>$v) {
			$allowed[$format] = $this->allTypes[$format];
		}

		return $allowed;
	}
}

?>