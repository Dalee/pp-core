<?php
/**
 * Список картинок
 *
 * storageType imagesarray
 * displayType IMAGESARRAY
 */
class PXDisplayTypeImagesarray extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$object[$field->name] = isset($object[$field->name]) ? $object[$field->name] : array();

		$items = array_merge(
			$object[$field->name],
			array(
				'null' => array(
					'width'  => null,
					'height' => null,
					'path'   => null,
					'entry'  => ''
				)
			)
		);

		$html = '';

		foreach($items as $value) {
			$html .= call_user_func(
				array(
					$param['inputClass'],
					'buildArrayItem'
				),
				$field->name,
				$value
			);
		}


		return $html;
	}

	static function buildArrayItem($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'image', $value['entry']);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetImageVar($field->name, TRUE);
	}

	static function buildCell($field, $object, $param=NULL) {
		$html = '';

		foreach($object[$field->name] as $k=>$value) {
			$html .= call_user_func(
				array(
					$param['inputClass'],
					'buildCellHtml'
				),
				$value,
				$field->name.$object['id'].$k
			);
		}

		return $html;
	}

	static function buildCellHtml($value) {
		return PXDisplayTypeImage::buildCellHtml($value);
	}
}
?>