<?php

class PXAdminTableBottomLabel extends PXAdminWidget
{
    public function html()
    {
        $objectType = $this->parentWidget->dtype->id;
        return <<<HTML
				<div class="multiple-bottom-controls">Операции с отмеченными:</div>
				<div dtype="{$objectType}" class="display-none"></div>
HTML;
    }
}
