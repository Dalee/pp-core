<?php

/**
 * Базовый класс описывающий ввод, вывод
 * (и что-то еще) полей объекта с определеннм
 * displayType
 *
 * Типичный способ обращения к статическим методам класса:
 * PXDisplayType::callMethod($method, $field, $object, $param);
 *
 * Описывается displayType обычно в таком формате ИМЯ|ПАРАМЕТР1|ПАРАМЕТР2
 * В ПАРАМЕТР1 обычно указывается ширина контрола,
 * в настоящее время она нигде не используется, сохраняется
 * совместимости для
 */
class PXDisplayType {
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
		return PXFieldUtil::callMethod($method, $field, $object, $param, __CLASS__, 'displayType');
	}

	/**
	 * Вызывается в PXRequest::GetContentObject при
	 * извлечении полей объекта из запроса
	 * Возвращает значение поля
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
	 * Вызывается в PXDatabase::ModifyContentObject перед
	 * нормализацией полей объекта
	 * Возвращает флаг, пропустить ли нормализацию поля
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
	 * Возвращает html для строки с полем объекта в
	 * главной форме PXAdminObjectForm
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
	 * Возвращает html для форм Картинки/Файлы PXAdminObjectForm
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
	 * Возвращает html для строки с полем объекта в
	 * формах Картинки/Файлы PXAdminObjectForm
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
	 * Возвращает чет/нечет строки с полем объекта в
	 * формах PXAdminObjectForm
	 *
	 * @param Boolean            $even
	 *
	 * @return Boolean
	 */
	static function getRow($even) {
		return !$even;
	}

	/**
	 * Возвращает html с элементами ввода для поля объекта в
	 * формах PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildInput($field, $object, $param=NULL) {
		return '<span class="error">Вывод поля типа '.$field->displayType.' еще не описан</span>';
	}

	/**
	 * Возвращает html с выводом значения поля объекта в
	 * таблицах PXAdminTable
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