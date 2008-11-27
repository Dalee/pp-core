<?php

/**
 * ������� ����� ����������� ����, �����
 * (� ���-�� ���) ����� ������� � �����������
 * displayType
 *
 * �������� ������ ��������� � ����������� ������� ������:
 * PXDisplayType::callMethod($method, $field, $object, $param);
 *
 * ����������� displayType ������ � ����� ������� ���|��������1|��������2
 * � ��������1 ������ ����������� ������ ��������,
 * � ��������� ����� ��� ����� �� ������������, �����������
 * ������������� ���
 */
class PXDisplayType {
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
		return PXFieldUtil::callMethod($method, $field, $object, $param, __CLASS__, 'displayType');
	}

	/**
	 * ���������� � PXRequest::GetContentObject ���
	 * ���������� ����� ������� �� �������
	 * ���������� �������� ����
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->getVar($field->name);
	}


	/**
	 * ���������� � PXDatabase::ModifyContentObject �����
	 * ������������� ����� �������
	 * ���������� ����, ���������� �� ������������ ����
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Boolean
	 */
	static function preModifyObject($field, $object, $param=NULL) {
		return false;
	}

	/**
	 * ���������� html ��� ������ � ����� ������� �
	 * ������� ����� PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildRow($field, $object, $param=NULL) {
		$even = PXDisplayType::__even($param['inputClass']);

		$html = "\n";
		$html .= '<tr'.(!$even ? ' class="even"' : '' ).'>';

		$html .= '<th>'.$field->description.': </th>';
		$html .= '<td>';

		$html .= PXDisplayType::callMethod('buildInput', $field, $object, $param);

		$html .= '</td>';
		$html .= '</tr>';

		return $html;
	}

	/**
	 * ���������� html ��� ���� ��������/����� PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildArray($field, $object, $param=NULL) {
	}

	/**
	 * ���������� html ��� ������ � ����� ������� �
	 * ������ ��������/����� PXAdminObjectForm
	 *
	 * @param String             $name
	 * @param Array              $value
	 *
	 * @return String
	 */
	static function buildArrayItem($name, $value) {
	}

	static function __even($className) {
		static $even = false;

		$even = call_user_func(
			array(
				$className,
				'getRow'
			),
			$even
		);

		return $even;
	}

	/**
	 * ���������� ���/����� ������ � ����� ������� �
	 * ������ PXAdminObjectForm
	 *
	 * @param Boolean            $even
	 *
	 * @return Boolean
	 */
	static function getRow($even) {
		return !$even;
	}

	/**
	 * ���������� html � ���������� ����� ��� ���� ������� �
	 * ������ PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildInput($field, $object, $param=NULL) {
		return '<span class="error">����� ���� ���� '.$field->displayType.' ��� �� ������</span>';
	}

	/**
	 * ���������� html � ������� �������� ���� ������� �
	 * �������� PXAdminTable
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $field->displayType.' - '.$value;
	}
}
?>