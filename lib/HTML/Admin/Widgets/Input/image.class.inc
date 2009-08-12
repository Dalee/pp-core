<?php
class PXInputImage extends PXInputFile {
	var $_default;
	var $p;
	var $width;
	var $height;

	function PXInputImage($name, $value, $entry = NULL) {
		$this->_default = 'i/v.gif';
		$this->value = $value;
		$this->name = $name;
		$this->entry = $entry;
		$this->type = 'image';
	}

	function setparam() {
		$this->distension = is_null($this->entry) ? '' : '['.$this->entry.']';

		$w = isset($this->value['width'])  ? $this->value['width']  : '100';
		$h = isset($this->value['height']) ? $this->value['height'] : '100';
		$this->p = isset($this->value['path'])   ? $this->value['path']   : $this->_default;

		if($w > 100 || $h > 100) {
			$this->width  = ($w - $h) > 0 ? 100 : floor(100 * $w/$h);
			$this->height = ($w - $h) > 0 ? floor(100 * $h/$w) : 100;
		} else {
			$this->width  = $w;
			$this->height = $h;
		}
	}
	
	function preview() {		
		return <<<HTML
			<div>
				<img src="{$this->p}" id="{$this->name}{$this->distension}src" width="{$this->width}" height="{$this->height}">
			</div>
HTML;
	}
	
	function param() {
		$html = '<input id="'.$this->name.$this->distension.'width"  type="text" readonly value="'.$this->value['width'].'" >';
		$html .= '<span>ширина</span>';

		$html .= '<input id="'.$this->name.$this->distension.'height" type="text" readonly value="'.$this->value['height'].'">';
		$html .= '<span>высота</span>';

		$html .= '<input id="'.$this->name.$this->distension.'size"   type="text" readonly value="'.($this->value['path'] ? filesize(BASEPATH.'/site/htdocs/'.$this->value['path']) : NULL).'">';
		$html .= '<span>размер</span>';
		
		return $html;
		
	}
}
?>