<?php
class PXInputImageStatic extends PXInputImage {
	function html() {
		$this->setparam();

		$html  = '<div class="imagepreview">';
		$html .= $this->preview();
		$html .= '</div>';

		return $html;
	}}
?>