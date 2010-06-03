<?php
class PXWidgetTabs extends PXAdminWidget {
	var $items;
	var $selected;

	function PXWidgetTabs($items, $buildHref=true) {
		$this->items      = $items;
		$this->varName    = 'area';
		$this->buildHref  = $buildHref;
	}

	function setSelected($selected) {
		$this->selected = $selected;
	}

	function setVarName($varName) {
		$this->varName = $varName;
	}

	function html() {
		$current = $this->selected;
		$html = array();

		$html[] = '<TABLE border="0" cellpadding="0" cellspacing="0" style="margin-top: 5px;"><TR>';

		$i = 0;
		foreach ($this->items as $k=>$v) {
			if ($i == 0) {
				if ($current == $k) {
					$html[] = '<TD><IMG src="i/tab/1-on.gif" width="4" height="27" alt=""></TD>';
					$currentI = 1;
				} else {
					$html[] = '<TD><IMG src="i/tab/1-off.gif" width="4" height="27" alt=""></TD>';
				}

			} else {
				if ($current == $k) {
					$html[] = '<TD><IMG src="i/tab/n.gif" width="4" height="27" alt=""></TD>';
					$currentI = $i+1;

				} else {
					if (isset($currentI) && $currentI == $i) {
						$html[] = '<TD><IMG src="i/tab/p.gif" width="4" height="27" alt=""></TD>';
					} else {
						$html[] = '<TD><IMG src="i/tab/off.gif" width="4" height="27" alt=""></TD>';
					}
				}
			}

			if ($current == $k) {
				$bg = 'on';
			} else {
				$bg = 'off';
			}
			$href = $this->buildHref ? NLAbstractLayout::buildHref($this->varName, $k) : '?'.$this->varName.'='.$k; 

			$html[]   = '<TD nowrap align="center" class="tabs" background="i/tab/bg-'.$bg.'.gif"><A href="'.$href.'" class="tabs">';
			$html[]   = $v;
			$html[]   = '</A>';
			$html[]   = '</TD>';
			$i++;
		}

		if (isset($currentI) && $currentI == sizeof($this->items)) {
			$html[] = '<TD><IMG src="i/tab/l-on.gif" width="4" height="27" alt=""></TD>';
		} else {
			$html[] = '<TD><IMG src="i/tab/l-off.gif" width="4" height="27" alt=""></TD>';
		}

		$html[] = '</TR></TABLE>';

		return join($html);
	}

}
?>
