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

	function PXStorageType(&$field, $name) {
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
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	function getFromRequest($field, $object, &$param) {
		return $object[$field->name];
	}

	/**
	 * Возвращает нормированное значение
	 * поля перед сохранением объекта в базу
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Array
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
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	function searchLikeQuery($field, $object, &$param) {
		return '';
	}

	/**
	 * Сохраняет файлы из $_FILES в site/htdocs/ai
	 * Подготавливает данные к сохранению в базу
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	function proceedFile($field, &$object, &$param, $allowed = null) {
		return isset($object[$field->name]) ? $object[$field->name] : NULL;
	}


	function cloneFile($field, $object, &$param) {
		return null;
	}

	/**
	 * Возвращает нормированное значение
	 * поля после выборки объекта из базы
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	function normalize($field, $object, &$param) {
		return isset($object[$field->name]) ? $object[$field->name] : NULL;
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
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Boolean
	 */
	function notInDb($field, $object, $param=NULL) {
		return false;
	}

	/**
	 * Вызывается при работе с полем sys_meta
	 * Возвращает необходима ли выгрузка/сохранение значения в sys_meta
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Boolean
	 */
	function inMeta($field, $object, $param=NULL) {
		return $this->notInDb($field, $object, $param);
	}

	/**
	 * Определяет возможность фильтрации вывода текущего StorageType в PXAdminObjectForm::LinksForm
	 */
	function filterable(){
		return false;
	}

	/*
		see PXDatabase::_createSelector()
	*/
	function createSelector($field, $object, $param=null) {
		return $field->typeDescription->id.'.'.PXRegistry::getDB()->mapFields($field->name);
	}

	static protected
	function parseConfigArguments($attrNode, $fieldNode) {
		$args = explode("|", $attrNode->value);
		$name = $args[0];

		array_shift($args);

		return $args;
	}

	static public
	function getClassByName($name) {
		$baseClass = 'PXStorageType';

		$inputClass  = $baseClass.ucfirst(strtolower($name));
		return class_exists($inputClass) ? $inputClass : $baseClass;
	}

	static public
	function parseConfig($attrNode, $fieldNode) {
		$name = preg_replace('/\|.+$/', '', $attrNode->value);

		$cls = self::getClassByName($name);
		$args = call_user_func(array($cls, 'parseConfigArguments'), $attrNode, $fieldNode);

		return array($cls, $name, $args);
	}
}
?>
