<?php

/**
 * Список картинок
 *
 * storageType imagesarray
 * displayType IMAGESARRAY
 */
class PXDisplayTypeImagesarray extends PXDisplayTypeImage
{
	public function buildInput($field, $object, $param = NULL)
	{
		$object[$field->name] ??= [];

		$items = array_merge(
			$object[$field->name],
			[
				'null' => [
					'width' => null,
					'height' => null,
					'path' => null,
					'entry' => '',
					'size' => null,
				],
			]
		);

		$html = '';

		foreach ($items as $value) {
			$html .= $this->buildArrayItem($field->name, $value);
		}


		return $html;
	}

	public function buildArrayItem($name, $value)
	{
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'image', $value['entry']);
	}

	public function getFromRequest($field, $object, $param = NULL)
	{
		$request = PXRegistry::getRequest();
		$filesFromRequest = $request->GetUploadFile($field->name, true);
		$edited = $request->GetUploadFile($field->name . '_edit', true);
		if (is_array($edited['name'] ?? null)) {
			$filesFromRequest['edit'] = $edited;
		}
		return $filesFromRequest;
	}

	public function buildCell($field, $object, $param = NULL)
	{
		if (empty($object[$field->name]) || !is_array($object[$field->name])) {
			return '';
		}

		$html = '';

		foreach ($object[$field->name] as $value) {
			$html .= parent::buildCell($field, [$field->name => $value], $param);
		}

		return $html;
	}
}
