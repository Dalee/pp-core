<?php
/**
 * Базовый класс описывающий сохранение, получение из запроса
 * (и что-то еще) полей объекта с определенным storageType
 */
class PXStorageType {

	const defaultSQLType = 'VARCHAR';

	var $field;
	var $name;
	var $triggers;

	public function __construct(&$field, $name) {
		$this->field = &$field;
		$this->name  = $name;
	}

	/**
	 * Возвращает заголовок таблицы объектов
	 * возможно с ссылкой на изменение порядка
	 * сортировки объектов
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	function objectsSortOrder($field, $object, &$param) {
		return $field->description;
	}

	function objectsSortOrderString($field, $object, &$param) {
		$html = '<a ';

		switch($param['order']) {
			case ($field->name.' DESC') :
				$html .= ' class="down"';
				$order =  $field->name;
				$title = '';
				break;

			case $field->name :
				$html .= ' class="up"';
				$order = $field->name.' DESC';
				$title = ' в обратном порядке';
				break;

			default:
				$order =  $field->name;
				$title = '';
				break;
		}

		$html .= ' href="'.$param['table']->_BuildHref($param['dtype'].'_order', $order).'"';
		$html .= ' title="Сортировать по полю &bdquo;'.$field->description.'&ldquo;'.$title.'">'.$field->description.'</a>';

		return $html;
	}

	/**
	 * Возвращает нормированное значение поля из запроса
	 *
	 * @param PXFieldDescription $field
	 * @param array              $object
	 * @param array|null         $param
	 *
	 * @return mixed
	 */
	function getFromRequest($field, $object, &$param) {
		return $object[$field->name];
	}

	/**
	 * Возвращает нормированное значение
	 * поля перед сохранением объекта в базу
	 *
	 * @param PXFieldDescription $field
	 * @param array              $object
	 * @param array|null         $param
	 *
	 * @return array
	 */
	function normalizeObjectAttribute($field, $object, &$param) {
		return array(NULL, $param['dbFields'], $param['dbValues']);
	}

	function normalizeObjectAttributeString($field, $object, &$param) {
		$k = $field->name;
		$v = $object[$k];

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	/**
	 * Возвращает условие поиска (SQL) некоего слова
	 * в поле
	 *
	 * @param PXFieldDescription $field
	 * @param array              $object
	 * @param array|null         $param
	 *
	 * @return string
	 */
	function searchLikeQuery($field, $object, &$param) {
		return '';
	}

	/**
	 * Сохраняет файлы из $_FILES в site/htdocs/ai
	 * Подготавливает данные к сохранению в базу
	 *
	 * @param PXFieldDescription $field
	 * @param array              $object
	 * @param array|null         $param
	 *
	 * @return string
	 */
	function proceedFile($field, &$object, &$param, $allowed = null) {
		return isset($object[$field->name]) ? $object[$field->name] : null;
	}

	/**
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array $param
	 * @return null
	 */
	function cloneFile($field, $object, &$param) {
		return null;
	}

	/**
	 * Возвращает нормированное значение
	 * поля после выборки объекта из базы
	 *
	 * @param PXFieldDescription $field
	 * @param array              $object
	 * @param array|null         $param
	 *
	 * @return mixed|null
	 */
	function normalize($field, $object, &$param) {
		return isset($object[$field->name]) ? $object[$field->name] : null;
	}

	/**
	 * Генерит pathname объекта
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	function createPathname($field, $object, &$param) {
		return '';
	}

	/**
	 * Вызывается в PXDatabase::_createSelector при составлении
	 * списка полей объекта извлекаемых из базы
	 * Возвращает флаг, извлекать ли поле из базы
	 *
	 * @return bool
	 */
	function storedInDb() {
		return !$this->notInDb($this->field, null, null);
	}

	/**
	 * @deprecated Use !storedInDb()
	 */
	function notInDb($field, $object, $param = null) {
		return false;
	}

	/**
	 * Вызывается при работе с полем sys_meta
	 * Возвращает необходима ли выгрузка/сохранение значения в sys_meta
	 *
	 * @return Boolean
	 */
	function storedInMeta() {
		return $this->inMeta($this->field, null, null);
	}

	/**
	 * @deprecated Use storedInMeta()
	 */
	function inMeta($field, $object, $param = null) {
		return !$this->storedInDb();
	}

	/**
	 * Определяет возможность фильтрации вывода текущего StorageType в PXAdminObjectForm::LinksForm
	 */
	function filterable(){
		return false;
	}

	/**
	 * @see PXDatabase::_createSelector()
	 */
	function createSelector($field, $object, $param = null) {
		return $field->typeDescription->id.'.'.PXRegistry::getDB()->mapFields($field->name);
	}

	protected static function parseConfigArguments($attrNode, $fieldNode) {
		$args = explode("|", $attrNode->value);
		array_shift($args);

		return $args;
	}

	public static function getClassByName($name) {
		$baseClass = 'PXStorageType';

		$inputClass = $baseClass.mb_ucfirst(mb_strtolower($name));
		if (!class_exists($inputClass)) {
			trigger_error("Class {$inputClass} not found, use {$baseClass} instead", E_USER_NOTICE);
			return $baseClass;
		}
		return $inputClass;
	}

	public static function parseConfig($attrNode, $fieldNode) {
		$name = preg_replace('/\|.+$/'.REGEX_MOD, '', $attrNode->value);

		$cls = self::getClassByName($name);
		$args = call_user_func(array($cls, 'parseConfigArguments'), $attrNode, $fieldNode);

		return array($cls, $name, $args);
	}
}
