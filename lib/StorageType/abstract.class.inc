<?php
/**
 * Базовый класс описывающий сохранение, получение из запроса
 * (и что-то еще) полей объекта с определенным storageType
 */
class PXStorageType {
	var $field;
	var $name;

	function PXStorageType(&$field, $name) {
		$this->field = &$field;
		$this->name = $name;
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
	function objectsSortOrder($field, $object, $param=NULL) {
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
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	function proceedFile($field, $object, &$param) {
		return null;
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
	 * Определяет возможность фильтрации вывода текущего StorageType в PXAdminObjectForm::LinksForm
	 */
	function filterable(){
		return false;
	}
}
?>