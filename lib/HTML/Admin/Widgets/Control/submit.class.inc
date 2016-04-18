<?php

class PXControlSubmit extends PXAdminWidget {
	var $value;
	var $name = null;
	var $accessKey = null;

	function PXControlSubmit($value, $name = null, $accessKey = null) {
		$this->value = $value;
		$this->name = $name;
		$this->accessKey = $accessKey;
	}

	function html() {
		return sprintf(
			'<input type="submit"%s value="%s" accesskey="%s"%s>',
			!empty($this->name) ? ' name="'.$this->name.'"' : '',
			quot($this->value),
			!empty($this->accessKey) ? quot($this->accessKey) : 's',
			$this->disable
		);
	}
}
