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
		@see PXRefDescription::_mapRefByDefaults()
	*/
	var $byDefault  = false;
	/**
		@var bool Скрывать или нет конкретный тип связи в форме редактирования ссылок ? Используется для служебных связей.
		@see PXApplication::FillDataTypes()
	*/
	var $hidden     = false;

	public
	function addField($field) {
		$this->fields[$field->name] = $field;
	}

	//fixme: find better name
	public
	function getOther($formatName) {
		return (($this->from == $formatName) ? $this->to : $this->from);
	}

	static function fillReferences($domReferences, &$app){
		if (!is_array($domReferences) || count($domReferences) == 0) {
			return;
		}

		foreach ($domReferences as $domReference) {
			$attrs = $domReference->attributes();
			$ref   = new PXRefDescription();
			$nodes = $domReference->childNodes();
			foreach ($attrs as $attr) {
				switch (trim($attr->name)) {
					case 'name':              $ref->name             = $attr->value; break;
					case 'from':              $ref->from             = $attr->value; break;
					case 'to':                $ref->to               = $attr->value; break;
					case 'restrictby':        $ref->restrictBy       = $attr->value; break;
					case 'filter-to':         $ref->filterTo         = $attr->value; break;
					case 'filter-from':       $ref->filterFrom       = $attr->value; break;
					case 'load':              $ref->load             = $attr->value; break;
					case 'multiple':          $ref->multiple         = true;         break;
					case 'bydefault':         $ref->byDefault        = $ref->_mapRefByDefaults($attr->value); break; //all or linked
					case 'hidden':            $ref->hidden           = true;         break;
				}
			}

			foreach ($nodes as $node) {
				switch ($node->nodeName()) {
					case 'attribute':
						$field     = new PXFieldDescription($node, $app, $ref);
						if(is_null($field->storageType)) {
							$field->setStorageType('string');
						}

						if(is_null($field->displayType)) {
							$field->setDisplayType('TEXT');
						}

						$ref->addField($field);
						break;
					default:
						throw new Exception('Unknown node name in reference: ' . $node->nodeName());
				}
			}
			$app->references[$ref->name] = $ref;
			if (!isset($app->types[$ref->from]) || !isset($app->types[$ref->to])) {
				FatalError("ОШИБКА МОДЕЛИ: Связь <em>{$ref->name}</em> невозможна, отсутствует один из типов");
			}
			$app->types[$ref->from]->references[$ref->to] = &$app->references[$ref->name];
			$app->types[$ref->to]->references[$ref->from] = &$app->references[$ref->name];
		}
	}

	function _mapRefByDefaults($string){
		switch($string){
			default:
			case 'all':
				return false;
			case 'linked':
				return true;
		}
	}
}
