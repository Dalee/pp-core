<?php
class PXTabbedMenuRegionChanger extends PXAdminWidget {
	function html() {
		if(
			isset(PXRegistry::getApp()->types[PXMultiRegions::REGION_MARK]) && //FIXME: in the future this behaviour will be managed via plugins abstraction
			PXRegistry::getLayout()->outerLayout === 'index'
		){
			$regions = PXMultiRegions::getInstance();
			$regionsData = $regions->getRegionsAsDirectory();

			if($regionsData){
				if(PXRegistry::getUser()->can('manage_region')) {
					return $this->_selector($regions, $regionsData);
				} else {
					return $this->_static($regions, $regionsData);
				}
			}
		}

		return '';
	}

	private function _static($regions, $regionsData) {
		$rId = $regions->getRid();
		$curReg = isset($regionsData[$rId]) ? $regionsData[$rId] : 'не определен';
		return '<span class="multipleregions-current-region">Текущий регион <strong>'.$curReg.'</strong></span>';
	}

	private function _selector($regions, $regionsData) {
		$uri = PXRegistry::getRequest()->GetRequestUri();

		$varName           = PXMultiRegions::REGION_MARK . '[rid]';
		$regionsData[null] = '- ПОКАЗАТЬ ВСЕ РЕГИОНЫ -';
		$regionSelector    = new PXInputDropDown($varName, $regions->getRid(), $regionsData);

		$selectorHtml      = $regionSelector->html();

		return <<<HTML
			<span class="multipleregions-choose-region">Выберите регион:</span>

			<div id="region_selector">
				<form method="POST" action="{$uri}">&nbsp;
					{$selectorHtml}
				</form>
			</div>

			<script type="text/javascript">
				document.getElementById("region_selector").firstChild.elements["{$varName}"].onchange = function() {
					this.form.submit();
					return true;
				}
			</script>
HTML;
	}
}
?>
