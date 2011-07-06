<?php
class PXInputDropDown extends PXAdminWidget {
	var $name;
	var $value;
	var $values = array();
	
	var $multiple = '';
	var $size     = '';
	
	function PXInputDropDown($name, $value, $values) {
		$this->name = $name;
		$this->value = $value;
		$this->values = $values;
	}
	
	function setMultiple($multiple){
		$this->multiple = $multiple ? 'multiple="multiple"' : '';
	}
	
	function setSize($size){
		$this->size = is_numeric($size) ? 'size="'.$size.'"' : '';
	}
	
	function html() {
		$html     = squeeze(sprintf('<select name="%s" %s %s %s>', $this->name, $this->disable, $this->multiple, $this->size), ' ');
		$empty    = array(null => 0);

		$selected = max($empty, array_flip((array)$this->value));
		
		foreach ($this->values as $k=>$title) {
			$selectedHTML = '';
			$value        = $k;
			
			//if there is no empty value and no "bydefault" value, try to use value == 0
			if(isset($selected[$k]) || ($selected == $empty && empty($k) && (($k = null) || true))) {
				$selectedHTML = ' selected';
				unset($selected[$k]);
			}
			
			$html .= sprintf('<option value="%s"%s>%s</option>', $value, $selectedHTML, htmlspecialchars($title));
		}
		
		foreach($selected as $unknown => $k){
			$html .= sprintf('<option value="%s" selected>[нет в списке - "%s"]</option>', $unknown, $unknown);
		}

		$html .= '</select>';

		return $html;
	}
}
?>
