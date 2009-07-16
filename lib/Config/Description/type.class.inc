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
	var $fieldGroups  = array();

	/**
	 * Возвращает массив типов потомков для переданного формата
	 */
	
	function childTypes() {
		$cTypes = array();

		if(count($this->childs) > 0){
			foreach ($this->childs as $v) {
				if(!$this->issetType($v)) {
					continue;
				}

				if ($v === $this->id){
					continue;
				}

				$cTypes[$v] =& $this->getType($v);
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
			if (!$this->issetType($k)) {
				unset($ac[$k]);
			}
		}

		return $ac;
	}

	function allowedChildTypes(&$object) {
		$allowed = $this->allowedChilds($object);

		foreach($allowed as $format=>$v) {
			$allowed[$format] = $this->getType($format);
		}

		return $allowed;
	}

	function issetType($typeName) {
		$allTypes =& PXRegistry::getTypes();
		return isset($allTypes[$typeName]) && is_object($allTypes[$typeName]);
	}

	function &getType($typeName) {
		$allTypes =& PXRegistry::getTypes();
		return $allTypes[$typeName];
	}

	// static parser factory
	function fillAppTypes(&$dom, &$app) {
		$domDatatypes = $dom->xpath("/model/datatypes/datatype");

		if (!is_array($domDatatypes) || count($domDatatypes) == 0) {
			FatalError("Не определена объектная модель проекта");
		}

		foreach ($domDatatypes as $domDatatype) {
			$type  = new PXTypeDescription();


			$type->parseTypeAttributes($domDatatype->attributes());

			/*

			FIXME:
				right xpath query $domDatatype->xpath('/descendant-or-self::attribute')
				$dom->xpath("//datatype[@name='{$type->id}']//attribute") used for some projects on php 4.3.11,
				that do not understand right xpath

			*/

			$fieldsNodes = $dom->xpath("//datatype[@name='{$type->id}']//attribute");

			foreach ($fieldsNodes as $fieldNode) {
				if ($fieldNode->nodeType() != 1) {
					continue;
				}

				$field = new PXFieldDescription($fieldNode->attributes(), $app, $type);

				if(is_null($field->storageType)) {
					$field->setStorageType('string');
				}

				if(is_null($field->displayType)) {
					$field->setDisplayType('TEXT');
				}

				if(is_null($field->listed)) {
					$field->listed = (bool) ($field->name === 'title');
				}

				if(is_null($field->defaultValue)) {
					$field->getDefaultValueNode($fieldNode, $dom);
				}

				$field->getGroupName($fieldNode, $dom);
				$field->getComments($fieldNode, $dom);

				$type->addField($field);
			}

			$app->types[$type->id] = $type;
		}

		PXTypeDescription::findParents($app->types);
		
	}

	// static
	function findParents(&$types) {
		foreach ($types as $k=>$v) {
			if (!empty($v->parent)) {
				if (isset($types[$v->parent])) {
					$types[$v->parent]->childs[] = $k;

				} else {
					FatalError("ОШИБКА МОДЕЛИ: {$v->id} не может быть дочерним по отношению к {$v->parent}, описание {$v->parent} отсутствует");
				}
			}
		}
	}

	function addField(&$field) {
		$name = $field->name;

		$this->fields[$name] = $field;
		$this->fieldGroups[$field->groupName][$name] =& $this->fields[$name];
	}


	function parseTypeAttributes($attrs) {
		foreach ($attrs as $attr) {
			switch ($attr->name) {
				case 'name'          : $this->id            = $attr->value; break;
				case 'description'   : $this->title         = utf8_decode($attr->value); break;
				case 'orderby'       : $this->order         = $attr->value; break;
				case 'parent'        : $this->parent        = $attr->value; break;
				case 'bydefault'     : $this->byDefault     = $this->_mapByDefaults($attr->value); break;
				case 'struct'        : $this->struct        = $attr->value; break;
				case 'outputhandler' : $this->outputHandler = $attr->value; break;

				default              : d2("Unknown attr name: ".$attr->name); break;
			}
		}

		if (!isset($this->order)) {
			$this->order = 'sys_order';
		}
	}

	/**
	* Преобразует 'all', 'selected', 'none' в числовые значения.
	*
	* @see PP_CHILDREN_FETCH_ALL, PP_CHILDREN_FETCH_SELECTED, PP_CHILDREN_FETCH_NONE
	* @access protected
	* @param String $string
	* @return Integer
	*/
	function _mapByDefaults($string) {
		switch($string) {
			case 'all'     : return PP_CHILDREN_FETCH_ALL;
			case 'selected': return PP_CHILDREN_FETCH_SELECTED;
			case 'none'    : return PP_CHILDREN_FETCH_NONE;
			default        : return NULL;
		}
	}

}

?>
