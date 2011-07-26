<?php
class PXTabbedMenuRegionChanger extends PXAdminWidget {
	function html() {
		if(
			isset(PXRegistry::getApp()->types[PXMultiRegions::REGION_MARK]) && //FIXME: in the future this behaviour will be managed via plugins abstraction
			PXRegistry::getLayout()->outerLayout === 'index'
		){
			$regions     = PXMultiRegions::getInstance();
			$regionsData = $regions->getRegionsAsDirectory();

			if (empty($regionsData)) {
				return;
			}

			unset($regionsData[null]);

			if(count($regionsData) == 1) {
				return $this->_static($regionsData);
			}

			return $this->_selector($regions->getRid(), $regionsData);
		}

		return '';
	}

	private function _static($regionsData) {
		return '<span class="multipleregions-current-region">Текущий регион <strong>'. reset($regionsData) .'</strong></span>';
	}

	private function _selector($rid, $regionsData) {
		$uri = quot(PXRegistry::getRequest()->GetRequestUri());

		$varName           = PXMultiRegions::REGION_MARK . '[rid]';
		$regionsData[null] = '- ПОКАЗАТЬ ВСЕ РЕГИОНЫ -';

		$regionSelector    = new PXInputDropDown($varName, $rid, $regionsData);

		$selectorHtml      = $regionSelector->html();

		return <<<HTML
			<span class="multipleregions-choose-region">Выберите регион:</span>

			<div id="region_selector">
				<form method="POST" action="{$uri}">&nbsp;
					{$selectorHtml}
				</form>
			</div>

			<script type="text/javascript">
				document.getElementById('region_selector').getElementsByTagName('form')[0].elements["{$varName}"].onchange = function() {
					this.form.submit();
					return true;
				}
			</script>
HTML;
	}
}
?>
