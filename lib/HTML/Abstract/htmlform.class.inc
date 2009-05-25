<?php
define('IMAGE_DEFAULT', 'i/v.gif');
define('FLASH_DEFAULT', 'i/v.swf');
define('FLASH_DEFAULT_FULL', '/admin/i/v.swf');

class NLAbstractHTMLForm {
	var $disable;

	function NLAbstractHTMLForm() {
		$this->disable = false;
	}

	function _DisabledStatus() {
		// isset($this) -- for PHP5
		return (isset($this) && isset($this->disable) && $this->disable);
	}

	static function BuildFormBegin($action='action.phtml', $method='POST', $enctype='multipart/form-data') {
		return '<form action="'.$action.'" method="'.$method.'" enctype="'.$enctype.'" class="edit">';
	}

	static function BuildFormEnd() {
		return '</form>';
	}

	static function BuildInputCheckBox($name, $on, $value=NULL, $id=NULL) {
		$checkbox = new PXInputCheckbox($name);

		$checkbox->setOn($on);
		$checkbox->setId($id);
		$checkbox->setValue($value);

		$checkbox->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $checkbox->html();
	}

	static function BuildCheckBox($name, $on, $value=NULL, $id=NULL) {
		return NLAbstractHTMLForm::BuildInputCheckBox($name, $on, $value, $id);
	}

	function BuildSubmit($value, $name=NULL) {
		$submit = new PXControlSubmit($value, $name);
		return $submit->html();
	}

	function BuildButton($value, $onClickCode = '') {
		$button = new PXControlButton($value);
		$button->setClickCode($onClickCode);
		$button->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $button->html();
	}

	function BuildReset($value) {
		$reset = new PXControlReset($value);
		$reset->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $reset->html();
	}

	static function BuildClose($value) {
		$close = new PXControlClose($value);
		return $close->html();
	}

	static function BuildHref($key, $value) {
		$href = "?";

		foreach ($this->getData as $k=>$v) {
			$v = urlencode($v);
			if (!empty($v) && $k != $key) $href .= $k.'='.$v.'&';
		}

		$href .= $key.'='.$value.'&';
		$href = substr($href, 0, -1);

		return $href;
	}

	function BuildHidden($name, $value) {
		$hidden = new PXInputHidden($name, $value);

		$hidden->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $hidden->html();
	}

	static function BuildText($name, $value, $height, $maxlength=null) {
		if ($height > 0) {
			$input = new PXInputTextarea($name, $value, $height, $maxlength);
		} else {
			$input = new PXInputInput($name, $value, $maxlength);
		}

		$input->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $input->html();
	}

	static function BuildPassword($name, $value, $canDelete=true) {
		$password = new PXInputPassword($name, $value);
		$password->canDelete($canDelete);
		$password->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $password->html();
	}

	static function BuildColor($name, $value) {
		$color = new PXInputColor($name, $value);
		$color->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $color->html();
	}

	static function BuildRichEdit($name, $value, $height) {
		$richedit = new PXInputRichEdit($name, $value, $height);
		$richedit->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $richedit->html();
	}

	static function BuildDateInline($name, $value) {
		$date = new PXInputDate($name, $value);

		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $date->html();
	}

	static function BuildDateTimeInline($name, $value) {
		$time = new PXInputTime($name, $value);
		$date = new PXInputDate($name, $value);

		$time->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $time->html().' '.$date->html();
	}

	function BuildDateTime($name, $value, $showTime, $showDate, $useCalendar, $valueNotNull = false) {
		$timeCls = $showTime ? 'PXInputTime' : 'PXInputTimeHidden';
		$dateCls = $showDate ? ($useCalendar ? 'PXInputDateCalendar' : 'PXInputDate') : 'PXInputDateHidden';

		$time = new $timeCls($name, $value, $valueNotNull);
		$date = new $dateCls($name, $value, $valueNotNull);

		$time->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $time->html().' '.$date->html();
	}

	static function BuildMultimedia($name, $value, $type='image', $entry=NULL) {
		switch($type) {
			case 'image':
				$input = new PXInputImage($name, $value, $entry);
				break;

			case 'flash':
				$input = new PXInputFlash($name, $value, $entry);
				break;

			case 'file':
				$input = new PXInputFile($name, $value, $entry);
				break;

			default:
				FatalError('Undefined input type '.$type);
		}

		$input->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $input->html();
	}

	function BuildLinkToFile($name, $value) {
		$link = new PXInputLinkToFile($name, $value);
		$link->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $link->html();
	}

	static function BuildImage($name, $value) {
		$image = new PXInputImage($name, $value);
		$image->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $image->html();
	}

	static function BuildFlash($name, $value) {
		$flash = new PXInputFlash($name, $value);
		$flash->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $flash->html();
	}

	static function BuildFile($name, $value) {
		$file = new PXInputFile($name, $value);
		$file->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $file->html();
	}

	function BuildDropDown($name, $value, $values) {
		$dropdown = new PXInputDropDown($name, $value, $values);
		$dropdown->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $dropdown->html();
	}
	
	function BuildParentDropDown($name, $value, $values) {
		$parentdropdown = new PXInputParentDropDown($name, $value, $values);
		$parentdropdown->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $parentdropdown->html();
	}

	static function BuildTable($name, $value, $cols, $height) {
		$colsWidth = array();

		for ($i=0; $i<$cols; $i++) {
			$colsWidth[] = 1;
		}

		return NLAbstractHTMLForm::BuildTableByCols($name, $value, $colsWidth, $height);
	}

	function BuildTableByCols($name, $value, $cols, $height) {
		$table = new PXInputTable($name, $value, $cols, $height);
		$table->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $table->html();
	}

	static function BuildIPAddr($name, $value) {
		$ip = new PXInputIPAddr($name, $value);
		return $ip->html();
	}

	static function BuildUserType($name, $value, $color, &$field) {
		return $field->displayType->name;
	}

	/*
	 *  todo: remove this shit outta of here
	 */
	// static function for request!!!
	function RequestGetUserVar($name, &$request, &$field) {
		return $request->postData->_GetVar($name);
	}
}

?>