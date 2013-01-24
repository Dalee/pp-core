<?php

require_once dirname(__FILE__) . '/additional_loader.class.inc';

class PXFiltersAdminWidget extends PXAdminWidget {
	protected $pluginFolder;
	protected $ignoredTypes;
	protected $dtype             = null;
	protected $additionalFilters = array();
	
	public function __construct($pluginFolder) {
		parent::__construct();
		$this->pluginFolder  = $pluginFolder;
	}
	
	public /* override */ function setParent($widget) {
		parent::setParent($widget);
		if(isset($widget->dtype)){
			$this->dtype = $widget->dtype;
			$this->additionalFilters = PXFiltersPluginAdditionalLoader::getAdditionalFilters($this->dtype->id, $this->pluginFolder);
		}
	}
	
	public /* override */ function html() {
		$ignoredTypes = array_map('trim', explode(',', PXAbstractPlugin::getParam('filters', 'ignore')));
		if (empty($this->dtype) || in_array($this->dtype->id, $ignoredTypes)) {
			return '';
		}
		$request = PXRegistry::getRequest();
		$layout  = PXRegistry::getLayout();
		$app     = PXRegistry::getApp();
		
		$layout->assignInlineJS($this->js());
		$layout->assignInlineCSS($this->css());
		
		$filters = $request->getVar('filters', array());

		$filters_html = '<p>Используйте ^ для поиска в начале строки, $ &mdash; в конце строки.</p>';
		
		foreach ($this->dtype->fields as $field) {
			if (!$field->listed || !$field->storageType->filterable()) continue;
			$filter_value = @htmlspecialchars($filters[$this->dtype->id][$field->name]);
			
			switch(true){
				case $field->storageType instanceof PXStorageTypeBoolean:
					$spformat = '<label><input type="radio" name="filters[' . $this->dtype->id . '][' . $field->name .']" value="%s"%s/>%s</label> ';
					
					$filters_html .= '<div>';
					$filters_html .= sprintf($spformat, '',  empty($filter_value) ? ' checked="checked"' : '', 'Все');
					$filters_html .= sprintf($spformat, 'f', $filter_value == 'f' ? ' checked="checked"' : '', 'Нет');
					$filters_html .= sprintf($spformat, 't', $filter_value == 't' ? ' checked="checked"' : '', 'Да');
					$filters_html .= sprintf('&ndash; <label>%s</label></div>', $field->description);
					break;
				default:
					$filters_html .= sprintf('<div><input type="text" value="%s" name="filters[%s][%s]" /> &ndash; <label>%s</label></div>', $filter_value, $this->dtype->id, $field->name, $field->description);
			}
		}
		
		foreach($this->additionalFilters as $additionalFilter){
			$filters_html .= $additionalFilter->html();
		}
		
		$filters_html .= '<button class="run_f">Применить</button>';

		if (!empty($filters[$this->dtype->id])) {
			$filters_html .= sprintf('<button dtype="%s" class="clr_f">Сбросить</button>', $this->dtype->id);
		}

		$collapsed = !empty($filters[$this->dtype->id]) ? '' : ' collapsed';

		return
<<<HTML
			<fieldset class="filters{$collapsed}">
				<legend><span>&nbsp;&nbsp;&nbsp;</span> Фильтры</legend>
				<div class="wrap">
					$filters_html
				</div>
			</fieldset>
HTML;
	}

	function js() {
		return
<<<JAVASCRIPT
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
		return
<<<CSS
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

		$req     = PXRegistry::getRequest();
		$param   = PXAdminPager::getPerPageParamByFormatId($this->parentWidget->getPagerName());
		$tmp     = array();

		foreach ($this->onPageValues as $i => $t) {
			$selected = $req->getVar($param, 20) == $t ? 'selected' : '';
			$tmp[$i]  = sprintf("<a class='%s' href='%s'>%s</a>", $selected, NLAbstractLayout::buildHref($param, $t), $t);
		}

		return "Показывать на странице: " . join($tmp);
	}
}

?>
