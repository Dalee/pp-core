<?php
/**
 * Базовый класс описывающий ввод, вывод
 * (и что-то еще) полей объекта с определенным
 * displayType
 *
 * Описывается displayType обычно в таком формате ИМЯ|ПАРАМЕТР1|ПАРАМЕТР2
 * В ПАРАМЕТР1 обычно указывается ширина контрола,
 * в настоящее время она нигде не используется, сохраняется
 * совместимости для
 */
class PXDisplayType {
	var $display;
	var $name;

	function PXDisplayType(&$display, $name) {
		$this->display = &$display;
		$this->name = $name;
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
	function getFromRequest($field, $object, $param=NULL) {
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
	function preModifyObject($field, $object, $param=NULL) {
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
	function buildRow($field, $object, $param=NULL) {
		$even = PXDisplayType::getRow();

		$html = "\n";
		$html .= '<tr'.(!$even ? ' class="even"' : '' ).'>';

		$html .= '<th>'.$field->description.': </th>';
		$html .= '<td>';

		$html .= $this->buildInput($field, $object, $param);
		$html .= $this->comments($field, $object, $param);

		$html .= '</td>';
		$html .= '</tr>';

		return $html;
	}

	function comments($field, $object, $param=NULL) {
		return $field->comments;
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
	function buildArray($field, $object, $param=NULL) {
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
	function buildArrayItem($name, $value) {
	}

	/**
	 * Возвращает чет/нечет строки с полем объекта в
	 * формах PXAdminObjectForm
	 *
	 * @param Boolean            $even
	 *
	 * @return Boolean
	 */
	function getRow() {
		static $even = false;

		$even = !$even;
		return $even;
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
	function buildInput($field, $object, $param=NULL) {
		return '<span class="error">Вывод поля типа '.$field->displayType->name.' еще не описан</span>';
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
	function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $field->displayType->name.' - '.$value;
	}


	protected function getSubDirectoryIfExists($field, $object, $value) {
		$_ = "";

		if($directory = @PXRegistry::getApp()->directory[$field->source]) {
			$dir_object = $directory->values[$object[$field->name]];
			$_ =  (string) @$dir_object[$directory->displayField];
		}

		return strlen($_) ? $_ : $value;
	}
}
?>
