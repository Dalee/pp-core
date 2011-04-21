<?php

require_once BASEPATH  . '/libpp/modules/acl.module.inc';

class PXModuleSortableAcl extends PXModuleAcl {
	function addNewRuleButton() {
		$buttonHtml = parent::addNewRuleButton();
		return $buttonHtml . '<script>'. $this->js() . '</script>';
	}

	function sort() {
		@parse_str($this->request->getVar('sorts'), $sorts);

		if (empty($sorts)) return;

		$this->db->transactionBegin();

			foreach($sorts as $order => $id) {
				$this->db->modifyingQuery(sprintf("UPDATE %s SET sys_order = '%s' WHERE id = '%s'", $this->sqlTable, $order, $id));
			}

		$this->db->transactionCommit();

		if ($this->request->isXmlHttpRequest()) die('ok');
	}

	function adminAction() {
		if ($this->request->getVar('action') == 'sort') {
			$this->sort();
		}

		return parent::adminAction();
	}

	function js() {
		return trim('$(function() { 
			$("head").append($("<script>").attr("src", "js/tools/jui.min.js")); 

			var positions_slice = {
				start: 0,
				stop: 0
			};

			$("table.objects tbody").sortable({
				start: function(event, ui) {
					positions_slice.start = $("table.objects tbody tr").index(ui.item);
				},

				stop: function(event, ui) {
					positions_slice.stop = $("table.objects tbody tr").index(ui.item);
					var range = [positions_slice.start, positions_slice.stop].sort();

					var i = range[0];

					var srlz = {};

					while (i<=range[1]) {
						srlz[i] = $($("table.objects tbody tr").get(i)).find("a:first").attr("href").match(/id=(\d+)/)[1];
						i++;
					}

					var options = {
						area: "acl",
						sorts: $.param(srlz),
						action: "sort"
					};

					$.post("/admin/action.phtml", options, function(data) {
					});
				}
			});
		});');
	}
}

?>
