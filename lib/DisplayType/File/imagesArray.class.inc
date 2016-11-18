<?php
/**
 * Список картинок
 *
 * storageType imagesarray
 * displayType IMAGESARRAY
 */
class PXDisplayTypeImagesarray extends PXDisplayTypeImage {
	function buildInput($field, $object, $param=NULL) {
		$object[$field->name] = isset($object[$field->name]) ? $object[$field->name] : array();

		$items = array_merge(
			$object[$field->name],
			array(
				'null' => array(
					'width'  => null,
					'height' => null,
					'path'   => null,
					'entry'  => '',
					'size'   => null,
				)
			)
		);

		$html = '';

		foreach($items as $value) {
			$html .= $this->buildArrayItem($field->name, $value);
		}


		return $html;
	}

	function buildArrayItem($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'image', $value['entry']);
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetImageVar($field->name, TRUE);
	}

	function buildCell($field, $object, $param=NULL) {
		$html = '';

		foreach((array)$object[$field->name] as $value) {
			$html .= parent::buildCell($field, [$field->name => $value], $param);
		}

		return $html;
	}
}
?>
