<?php

require_once BASEPATH  . '/libpp/modules/acl.module.inc';

class PXModuleSortableAcl extends PXModuleAcl {
	function addNewRuleButton() {
		$this->js();
		return parent::addNewRuleButton();
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
		$options = array(
			'area' => $this->area,
			'sid' => $this->_getSid(),
		);

		$this->layout->assignJs("js/tools/jui.min.js");
		$this->layout->assignInlineJs('var sortableaclOptions = ' . json_encode($options) . ';');
		$this->layout->assignInlineJs(
<<<inlinejs
		jQuery(function($) {
			var positions = { from: -1, to: -1 };
			var tbody = $("table.objects tbody");
			var trs, refreshTrs = function() {
				trs = tbody.children("tr");
			}
			refreshTrs();

			var indexOfItem = function(item) {
				return trs.index(item);
			};
			var fetchIdFromHref = function(href) {
				return ((href || '').match(/id=(\d+)/) || [0,0])[1];
			}
			var idAtIndex = function(index) {
				if (!trs[index]) {
					return 0;
				}
				return (trs[index].id || '').replace(/^[^\d]+/, '') || fetchIdFromHref($(trs[index]).find('a:first').attr('href'));
			};

			// fix width for flowing tr's
			var colWidths = [];
			tbody.find('tr:first>td').each(function(i, el){
				colWidths[i] = $(this).width();
			});

			// hmm. can it be done easier?
			var prepareHelper = function(helper) {
				helper.css({'width':tbody.width()});
				helper.find('td').each(function(i, el){
					$(this).css('width', colWidths[i]+'px');
				});
				return helper;
			};
			var cleanHelper = function(helper) {
				helper.css({'display':'', 'width': ''});
				helper.find('td').each(function(i, el){
					$(this).css('width', '');
				});
			}

			var putRuleAfter = function(id, afterId, callback) {
				var options = {
					area:   sortableaclOptions.area,
					sid:    sortableaclOptions.sid,
					action: 'putafter',
					ajax:   true,
					id:     id,
					after:  afterId
				};

				$.post("/admin/action.phtml", options, callback);
			}

			var redrawZebra = function() {
				trs.filter(':nth-child(even)').addClass("even");
				trs.filter(':nth-child(odd)').removeClass("even");
			}

			// ajaxing clicks on up/down buttons
			var swap = function(a, b) {
				if (a < 0 || b < 0) {
					return false;
				}
				if (a > b) {
					a ^= b ^= a ^= b;
				}
				putRuleAfter(idAtIndex(a), idAtIndex(b), function(data){
					$(trs[b]).after(trs[a]);
					refreshTrs();
					redrawZebra();
				});
			}
			tbody.find('td:first-child a[href*="action=up"]').click(function(e){
				e.preventDefault();
				var row = $(this).closest('tr'), prev = row.prev();
				if (!prev) {
					return;
				}
				swap(indexOfItem(prev), indexOfItem(row));
			});
			tbody.find('td:first-child a[href*="action=down"]').click(function(e){
				e.preventDefault();
				var row = $(this).closest('tr'), next = row.next();
				swap(indexOfItem(row), indexOfItem(next));
			});

			// dragger
			tbody.sortable({
				appendTo: tbody,
				containment: 'parent',

				helper: function(event, helper) {
					return prepareHelper(helper);
				},

				start: function(event, ui) {
					positions = { from: indexOfItem(ui.item), to: -1 };
				},

				change: function(event, ui) {
					positions.to = indexOfItem(ui.item);
				},

				stop: function(event, ui) {
					cleanHelper(ui.item);

					if (positions.to < 0) {
						return; // no changes
					}

					refreshTrs(); // it must be here (before indexOfItem) for refresh trs
					positions.to = indexOfItem(ui.item);

					// redraw zebra!
					redrawZebra();

					// send command
					var id = idAtIndex(positions.to),
					    after = trs[positions.to-1] ? idAtIndex(positions.to-1) : 'first';

					putRuleAfter(id, after);
				}
			});
		});
inlinejs
		);
	}
}

?>
