<?php
	class PXAdminTableMultiCheckboxes extends PXAdminWidget {

		function html() {
			//We draw it BEFORE admin table, so we can change it's content
			$this->parentWidget->insertControl('multiselect', '<input class="multiselect" oid="{{id}}" type="checkbox">');
			$this->parentWidget->showDefaultControls('after');
			
			$layout = PXRegistry::getLayout();
			$layout->assignCSS('/admin/css/admin_table_multiops.css');
			array_map(array($layout, 'assignJS'), array('/admin/js/jq.livequery.js', '/admin/js/jq.json.js', '/admin/js/admin_table_multiops.js'));
			
			return $this->_drawMultiCheckboxes();
		}
		
		function _drawMultiCheckboxes(){
			$objectType = $this->parentWidget->dtype->id;
			return <<<HTML
				<div class="multiselect-all">
					<input id="multiselector_{$objectType}" type="checkbox" disabled="disabled"><label for="multiselector_{$objectType}">Выбрать все объекты</label>
				</div>
HTML;
		}
	}
?>