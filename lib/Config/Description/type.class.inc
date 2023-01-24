<?php
/**
* Класс - хранилище.
* Стоится по etc/datatype.xml /model/datatypes/datatype
*
* @subpackage Application
*/
class PXTypeDescription
{
	public $id = null;
	public $title = null;
	public $parent = null;
	public $childs = [];
	public $references = [];
	public $order = null;

	/** @var PXFieldDescription[] */
	public $fields = [];

	public $groups = ['' => ['name' => null, 'view' => null]]; //TODO|FIXME: need standalone group description ?
	public $fieldGroups = [];
	public $sTypes = [];
	public $dTypes = [];
	public $struct = 'plain';
	public $byDefault = null;

	public function parentType()
	{
		return PXRegistry::getTypes($this->parent);
	}

	/**
	 * Возвращает массив типов потомков для переданного формата
	 */
	public function childTypes()
	{
		$cTypes = [];

		if (count($this->childs) > 0) {
			foreach ($this->childs as $v) {
				if (!$this->issetType($v)) {
					continue;
				}

				if ($v === $this->id) {
					continue;
				}

				$cTypes[$v] =& $this->getType($v);
			}
		}

		return $cTypes;
	}

	public function allowedChilds($object)
	{
		$ac = [];
		if (is_object($object)) {
			$ac = $object->content[OBJ_FIELD_CHILDREN] ?? [];
		}

		if (is_array($object)) {
			$ac = $object[OBJ_FIELD_CHILDREN] ?? [];
		}

		foreach ($ac as $k => $v) {
			if (!$this->issetType($k)) {
				unset($ac[$k]);
			}
		}

		return $ac;
	}

	public function allowedChildTypes(&$object)
	{
		$allowed = $this->allowedChilds($object);

		foreach ($allowed as $format => $v) {
			$allowed[$format] = $this->getType($format);
		}

		return $allowed;
	}

	public function issetType($typeName)
	{
		$allTypes = PXRegistry::getTypes();
		return isset($allTypes[$typeName]) && is_object($allTypes[$typeName]);
	}

	public function &getType($typeName)
	{
		$allTypes = PXRegistry::getTypes();
		return $allTypes[$typeName];
	}

	// static parser factory
	public static function fillAppTypes($domDatatypes, &$app)
	{

		if (!is_array($domDatatypes) || count($domDatatypes) == 0) {
			FatalError("Не определена объектная модель проекта");
		}

		foreach ($domDatatypes as $domDatatype) {
			$type = new static();

			$type->parseTypeAttributes($domDatatype->attributes());

			$groupNodes = $domDatatype->xpath("group");
			foreach ($groupNodes as $groupNode) {
				if (!$groupNode->isXmlNode()) {
					continue;
				}
				$name = $view = null;
				foreach ($groupNode->attributes() as $attr) {
					switch ($attr->name) {
						case 'name':
							$name = pp_simplexml_decode_string($attr->value);
							break;
						case 'view':
							$view = pp_simplexml_decode_string($attr->value);
							break;
					}
				}
				$type->groups[$name] = ['name' => $name, 'view' => $view];
			}

			$fieldsNodes = $domDatatype->xpath('descendant-or-self::attribute');

			foreach ($fieldsNodes as $fieldNode) {
				if (!$fieldNode->isXmlNode()) {
					continue;
				}

				$field = new PXFieldDescription($fieldNode, $app, $type);

				if (is_null($field->storageType)) {
					$field->setStorageType('string');
				}

				if (is_null($field->displayType)) {
					$field->setDisplayType('TEXT');
				}

				if (is_null($field->listed)) {
					$field->listed = (bool)($field->name === 'title');
				}

				if (is_null($field->defaultValue)) {
					$field->getDefaultValueNode($fieldNode);
				}

				$field->getGroupName($fieldNode);
				$field->getComments($fieldNode);

				$type->addField($field);
				$type->assignToGroup($field);
			}

			$app->types[$type->id] = $type;
		}

		static::findParents($app->types);
	}

	public static function findParents(&$types)
	{
		foreach ($types as $k => $v) {
			if (!empty($v->parent)) {
				if (isset($types[$v->parent])) {
					$types[$v->parent]->childs[] = $k;

				} else {
					FatalError("ОШИБКА МОДЕЛИ: {$v->id} не может быть дочерним по отношению к {$v->parent}, описание {$v->parent} отсутствует");
				}
			}
		}
	}

	public function addField(&$field)
	{
		$name = $field->name;
		$this->fields[$name] = $field;
	}

	/**
	 * @param string $name
	 * @return bool
	 */
	public function hasField($name)
	{
		return isset($this->fields[$name]);
	}

	public function assignToGroup($field)
	{
		static $groupId = 0;
		static $fillingGroup;
		if ($fillingGroup !== $field->groupName) {
			$fillingGroup = $field->groupName;
			$groupId++;
		}
		$this->fieldGroups[$groupId][$field->groupName][$field->name] = $field;
	}


	public function parseTypeAttributes($attrs)
	{
		foreach ($attrs as $attr) {
			switch ($attr->name) {
				case 'name'          :
					$this->id = $attr->value;
					break;
				case 'description'   :
					$this->title = pp_simplexml_decode_string($attr->value);
					break;
				case 'orderby'       :
					$this->order = $attr->value;
					break;
				case 'parent'        :
					$this->parent = $attr->value;
					break;
				case 'bydefault'     :
					$this->byDefault = self::_mapByDefaults($attr->value);
					break;
				case 'struct'        :
					$this->struct = $attr->value;
					break;

				default              :
					throw new Exception('Unknown attr name:' . $attr->name);
			}
		}

		if (!isset($this->order)) {
			$this->order = OBJ_FIELD_ORDER;
		}
	}

	/**
	 * Преобразует 'all', 'selected', 'none' в числовые значения.
	 * protected -> public, because used in PXAdminForm
	 *
	 * @param String $string
	 * @return Integer
	 * @see PP_CHILDREN_FETCH_ALL, PP_CHILDREN_FETCH_SELECTED, PP_CHILDREN_FETCH_NONE
	 * @access public
	 */
	public static function _mapByDefaults($string)
	{
		return match ($string) {
			'all' => PP_CHILDREN_FETCH_ALL,
			'selected' => PP_CHILDREN_FETCH_SELECTED,
			'paged' => PP_CHILDREN_FETCH_PAGED,
			'none' => PP_CHILDREN_FETCH_NONE,
			default => NULL,
		};
	}

}
