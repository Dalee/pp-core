<?php
/**
 * Базовый класс описывающий сохранение, получение из запроса
 * (и что-то еще) полей объекта с определеннм storageType
 *
 * Типичный способ обращения к статическим методам класса:
 * PXStorageType::callMethod($method, $field, $object, $param);
 *
 */
class PXStorageType {
	/**
	 * Вызывает нужный метод нужного класса
	 *
	 * @param String             $method
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function callMethod($method, $field, $object, $param=NULL) {
		return PXFieldUtil::callMethod($method, $field, $object, $param, __CLASS__, 'storageType');
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
	static function objectsSortOrder($field, $object, $param=NULL) {
		return $field->description;
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
	static function getFromRequest($field, $object, &$param) {
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
	static function normalizeObjectAttribute($field, $object, &$param) {
		return array(NULL, $param['dbFields'], $param['dbValues']);
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
	static function searchLikeQuery($field, $object, &$param) {
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
	static function proceedFile($field, $object, &$param) {
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
	static function normalize($field, $object, &$param) {
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
	static function createPathname($field, $object, &$param) {
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
	static function notInDb($field, $object, $param=NULL) {
		return false;
	}
}
?>