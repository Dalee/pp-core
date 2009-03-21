<?php
class PXInputFlash extends PXInputImage {
	var $_default;
	var $_default_full;
	
	function PXInputFlash($name, $value, $entry = NUll) {
		$this->_default = 'i/v.swf';
		$this->_default_full = '/admin/i/v.swf';
		$this->value = $value;
		$this->name = $name;
		$this->entry = $entry ? $entry : NULL;
		$this->type = 'flash';
	}
	
	function preview() {
		return <<<HTML
			<div>
				<script type="text/javascript">ShowFlash('i/l.swf?path={$this->p}', 100, 100, '{$this->name}{$this->distension}src');</script>
			</div>
HTML;
	}
}
?>