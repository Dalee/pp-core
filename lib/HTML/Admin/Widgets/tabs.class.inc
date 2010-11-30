<?php

class PXWidgetTabs extends PXAdminComplexWidget {
	const PREPEND = 0;
	const APPEND  = 1;

	public $blocks = array(
		self::PREPEND => array(),
		self::APPEND  => array());

	public $items;
	public $selected;
	public $varName = 'area';
	public $buildHref = true;

	function setSelected($selected) {
		$this->selected = $selected;
	}

	function setVarName($varName) {
		$this->varName = $varName;
	}

	function html() {
		$this->decorate();

		$current = PXRegistry::getRequest()->getVar($this->varName, 'main');

		$html[] = "<ul class='topmenu'>";
		$html[]  = join($this->blocks[self::PREPEND]);

		foreach ($this->items as $k=>$v) {
			$href   = $this->buildHref ? NLAbstractLayout::buildHref($this->varName, $k) : '?'.$this->varName.'='.$k;
			$html[] = sprintf("<li %s><a href='%s'>%s</a><b></b></li>", ($k  === $current ? "class='current'" : ""), $href, $v);
		}
		
		$html[] = join($this->blocks[self::APPEND]);
		$html[] = "</ul>";
		
		return join($html);
	}
}

?>
