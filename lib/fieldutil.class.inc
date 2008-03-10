<?php

class PXFieldUtil {
	/**
	 * Вызывает нужный метод нужного класса
	 *
	 * @param String             $method
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 * @param String             $baseClass
	 *
	 * @return Mixed
	 */
	static function callMethod($method, $field, $object, $param=NULL, $baseClass, $fieldType) {
		$inputClass = PXFieldUtil::getInputClass($field->$fieldType, $baseClass);

		$param['inputClass'] = $inputClass;

        return call_user_func(array($inputClass, $method), $field, $object, $param);
	}

	/**
	 * Возвращает имя класса, описывающего fieldType
	 * Если он есть конечно
	 *
	 * @param String             $fieldType
	 *
	 * @return String
	 */
	static function getInputClass($fieldType, $baseClass) {
		$inputClass = $baseClass.ucfirst(strtolower($fieldType));
		return  class_exists($inputClass) ? $inputClass : $baseClass;
	}
}

?>
