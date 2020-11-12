<?php
/**
 * Картинка
 *
 * storageType image
 * displayType IMAGE|width|height
 */
class PXDisplayTypeImage extends PXDisplayType
{
	public function buildInput($field, $object, $param = NULL)
	{
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: ['width' => null, 'height' => null, 'path' => null, 'size' => null];

		return $this->getAdminWidget($k, $value);
	}

	public function getAdminWidget($k, $value)
	{
		return NLAbstractHTMLForm::BuildImage($k, $value);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		$maxWidth = $this->getMaxWidth($field);
		$maxHeight = $this->getMaxHeight($field);
		return $this->buildCellHtml($object[$field->name], $maxWidth, $maxHeight);
	}

	public function buildCellHtml()
	{
		[$value, $maxWidth, $maxHeight] = func_get_args(); // PHP54 args contract violation
		$html = '<div class="imagepreview image-in-cell"><div class="small" style="width: ' . $maxWidth . 'px; height: ' . $maxHeight . 'px;">';

		if (isset($value['path'])) {
			$width = isset($value['width']) ? $value['width'] : $maxWidth;
			$height = isset($value['height']) ? $value['height'] : $maxHeight;
			[$width, $height] = $this->getDisplaySizes($width, $height, $maxWidth, $maxHeight);
			$html .= '<a href="' . $value['path'] . '" target="_blank"><img src="' . $value['path'] . '" width="' . $width . '" height="' . $height . '"/></a>';
		}

		$html .= '</div>' . (isset($value['path']) ? $value['width'] . 'x' . $value['height'] : '') . '</div>';
		return $html;
	}

	public function getFromRequest($field, $object, $param = NULL)
	{
		return $param['request']->postData->_GetImageVar($field->name);
	}

	public function getMaxWidth($field)
	{
		$s = 50;
		if (isset($field->displayTypeArgs[0])) {
			$s = $field->displayTypeArgs[0];
		}
		return $s;
	}

	public function getMaxHeight($field)
	{
		$s = 50;
		if (isset($field->displayTypeArgs[1])) {
			$s = $field->displayTypeArgs[1];
		} elseif (isset($field->displayTypeArgs[0])) {
			$s = $field->displayTypeArgs[0];
		}
		return $s;
	}

	public function getCellWidth($field)
	{
		// + 22:
		// + 20 for div.image-in-cell {margin: 10px;}
		// + 2 for the border
		return $field && isset($field->displayTypeArgs[0]) ? $field->displayTypeArgs[0] + 22 : 0;
	}

	public function getDisplaySizes($width, $height, $maxWidth, $maxHeight)
	{
		if (($width <= $maxWidth) && ($height <= $maxHeight)) {
			$w = $width;
			$h = $height;
		} else {
			if (($width > $maxWidth) && ($height > $maxHeight)) {
				if (($width / $maxWidth) > ($height / $maxHeight)) {
					$factor = $maxWidth / $width;
				} else {
					$factor = $maxHeight / $height;
				}
			} elseif ($width > $maxWidth) {
				$factor = $maxWidth / $width;
			} else {
				$factor = $maxHeight / $height;
			}
			$w = $width * $factor;
			$h = $height * $factor;
		}
		return [(int)$w, (int)$h];
	}
}

?>
