<?php
/**
 * ������� ����� ����������� ����������, ��������� �� �������
 * (� ���-�� ���) ����� ������� � ����������� storageType
 *
 * �������� ������ ��������� � ����������� ������� ������:
 * PXStorageType::callMethod($method, $field, $object, $param);
 *
 */
class PXStorageType {
	/**
	 * �������� ������ ����� ������� ������
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
	 * ���������� ��������� ������� ��������
	 * �������� � ������� �� ��������� �������
	 * ���������� ��������
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
	 * ���������� ������������� �������� ���� �� �������
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
	 * ���������� ������������� ��������
	 * ���� ����� ����������� ������� � ����
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
	 * ���������� ������� ������ (SQL) ������� �����
	 * � ����
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
	 * ��������� ����� �� $_FILES � site/htdocs/ai
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
	 * ���������� ������������� ��������
	 * ���� ����� ������� ������� �� ����
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
	 * ������� pathname �������
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
	 * ���������� � PXDatabase::_createSelector ��� �����������
	 * ������ ����� ������� ����������� �� ����
	 * ���������� ����, ��������� �� ���� �� ����
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