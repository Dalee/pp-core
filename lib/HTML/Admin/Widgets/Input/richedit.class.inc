<?php
class PXInputRichEdit extends PXAdminWidget {
	var $value;
	var $name;
	var $height;
	
	function PXInputRichEdit($name, $value, $height) {
		$this->value  = $value;
		$this->name   = $name;
		$this->height = $height;
	}
	function html() {
		return <<<HTML
			<textarea name="{$this->name}" id="{$this->name}" style="width: 100%; height: {$this->height}px;">{$this->value}</textarea>

			<script type="text/javascript">
				editor_generate('{$this->name}');
			</script>
HTML;
	}
}
?>