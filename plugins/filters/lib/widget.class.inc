<?php

require_once dirname(__FILE__) . '/additional_loader.class.inc';

class PXFiltersAdminWidget extends PXAdminWidget {
	private $pluginFolder;

	function __construct($pluginFolder) {
		$this->pluginFolder = $pluginFolder;
		parent::__construct();
	}
	
	function html() {
		$this->request = PXRegistry::getRequest();
		$this->layout  = PXRegistry::getLayout();

		$this->layout->assignInlineJS($this->js());
		$this->layout->assignInlineCSS($this->css());

		$app = PXRegistry::getApp();

		$filters = $this->request->getVar('filters', array());
		$filters_html = array();

		$dtype = @$this->parentWidget->dtype;

		if (!$dtype) {
			return;
		}

		foreach ($dtype->fields as $field) {
			if (!$field->listed || !$field->storageType->filterable()) continue;
			$filter_value = @htmlspecialchars($filters[$dtype->id][$field->name]);
			$filters_html[] = sprintf('<div><input type="text" value="%s" name="filters[%s][%s]" /> &ndash; <label>%s</label></div>', $filter_value, $dtype->id, $field->name, $field->description);
		}

		array_unshift($filters_html, '<p>Используйте ^ для поиска в начале строки, $ &mdash; в конце строки.</p>');
		
		foreach (PXFiltersPluginAdditionalLoader::getAdditionalFilters($dtype->id, $this->pluginFolder) as $additionalFilterClass) {
			/* @var $additionalFilterClass PXAdditionalFilter */
			$filters_html[] = $additionalFilterClass->html();
		}
		
		$filters_html[] = '<button class="run_f">Применить</button>';

		if (!empty($filters[$dtype->id])) {
			$filters_html[] = sprintf('<button dtype="%s" class="clr_f">Сбросить</button>', $dtype->id);
		}

		$collapsed = !empty($filters[$dtype->id]) ? '' : ' collapsed';

		return join(array(
			'<fieldset class="filters', $collapsed, '">',
				'<legend><span>&nbsp;&nbsp;&nbsp;</span> Фильтры</legend>',
				'<div class="wrap">',
					join($filters_html),
				'</div>',
			'</fieldset>'
		));
	}

	function js() {
		return <<<JAVASCRIPT
$(function() {
	var erase_filters = function(name) {
		var loc = window.location.search;
		var pattern_str = name != undefined ? "filters\\\[" + name + "\\\][^&]+&?" : "filters[^&]+&?";
		var pattern = new RegExp(pattern_str, "gi");
		return loc.replace(pattern, '').replace(/&$/, '');
	};
	
	$(".run_f").click(function() {
		var search_properties = {};
		var loc = erase_filters().split('&');
		var tmp = [];

		for (i in loc) {
			tmp = loc[i].split('=');
			search_properties[tmp[0].replace('?', '')] = tmp[1];
		}

		$('input[name^="filters"],select[name^="filters"]').filter(function() {
			return $(this).val().length;
		}).each(function(i, item) {
			var name = $(item).attr('name');
			var type = $(item).attr('type');
			if (type == 'radio' || type == 'checkbox') {
				if ($(item).attr('checked')) {
					search_properties[name] = $(item).attr('value');
				}
			} else {
				search_properties[name] = $(item).val();
			}
		});

		var join = [];
		for (i in search_properties) {
			if(i != 'charcheck') {
				join.push(i + '=' + encodeURIComponent(search_properties[i]));
			}
		}
	
		join.push('charcheck=' + encodeURIComponent('йцукен'));

		window.location.search = '?' +  join.join('&');
		return false;
	});

	// FIXME kill all filters set var with filtes name here!
	$(".clr_f").click(function() {
		window.location.search = erase_filters($(this).attr('dtype'));
		return false;
	});

	$("fieldset.filters legend").click(function() {
		$(this).closest('fieldset').toggleClass('collapsed');
		return false;
	});
});
JAVASCRIPT;
	}

	function css() {
		return <<<CSS
fieldset.filters {
	margin-bottom: 20px;
	border: 1px solid #385A94;
}

fieldset.filters.collapsed div.wrap {
	display: none;
}

fieldset.filters.collapsed legend span {
	background: url('/admin/i/icon/plus.gif') no-repeat 0 0;
}

fieldset.filters legend span {
	background: url('/admin/i/icon/minus.gif') no-repeat 0 0;
}

fieldset.filters.collapsed {
	border-top: 1px solid #385A94;

	border-left: none;
	border-right: none;
	border-bottom: none;

	margin-bottom: 0;
}

fieldset.filters input, 
fieldset.filters select, 
fieldset.filters button {
	margin: 3px;
}
CSS;
	}
}

class PXOnPageAdminWidget extends PXAdminWidget {
	private $pluginFolder;
	private $flag;

	function __construct($pluginFolder, $flag=null) {
		$this->pluginFolder = $pluginFolder;
		$this->flag = $flag;
		parent::__construct();
	}

	function html() {
		if ($this->flag == 'before') {
			return '<span style="float: left; margin-left: 2px"> Страница: </span>';
		} elseif($this->flag == 'after'){
			return  $this->onPageControll();
		}

		return '';
	}

	//HARD
	function onPageControll() {
		// list on page: 10,30,50,100
		$this->onPageValues = explode(',', PXAbstractPlugin::getParam('filters', 'on_page'));

		$req = PXRegistry::getRequest();
		$param   = PXAdminPager::getPerPageParamByFormatId($this->parentWidget->getPagerName());
		$tmp = array();

		foreach ($this->onPageValues as $i => $t) {
			$selected = $req->getVar($param, 20) == $t ? 'selected' : '';
			$tmp[$i] = sprintf("<a class='%s' href='%s'>%s</a>", $selected, NLAbstractLayout::buildHref($param, $t), $t);
		}

		return "Показывать на странице: " . join($tmp);
	}
}

?>
