<?php
include_once('Common/functions.array.inc');
include_once('Common/functions.string.inc');

define('TABLECOLOR1',       '#E7E9ED');
define('TABLECOLOR2',       '#C9CED6');
define('TABLECOLOR3',       '#385A94');

class NLAbstractLayout {
	private $html   = '';
	private $labels = array();

	var $getData;

	public $outerLayout;

	function NLAbstractLayout() {
		$this->getData      = array();

		$this->template_dirs = array(
			BASEPATH.'local/templates/admin/',
			BASEPATH.'libpp/templates/admin/'
		); 
	}

	function setOuterLayout($template) {
		$this->outerLayout = $template;
		
		$this->html        = $this->template($template.'.tmpl');
	}

	function template($filename, $label=null) {
		foreach($this->template_dirs as $dir) {
			if(file_exists($dir.$filename)) {
				$html = file_get_contents($dir.$filename);

				if(is_string($label)) {
					$this->append($label, $html);
				}

				return $html;
			}
		}

		FatalError('Template '.$filename.' dosn\'t exists');
	}


	function _arrayToAttrs($array) {
		if(!sizeof($array)) {
			return '';
		}

		$attrs = array(' ');

		foreach($array as $k=>$v) {
			$attrs[] = $k.'="'.$v.'"';
		}

		return implode(' ', $attrs);
	}

	// private, deprecate
	function setInnerLayout($table) {
		$html  = '<table class="inner-layout">';

		foreach ($table as $rk => $row) {
			$html .= '<tr>';

			foreach ($row as $ck => $col) {
				$td = array();

				if (!empty($col[0])) $td['width']   = $col[0];
				if (!empty($col[1])) $td['style']   = 'height:'.$col[1];
				if (!empty($col[2])) $td['colspan'] = $col[2];
				if (!empty($col[3])) $td['rowspan'] = $col[3];

				$divClass = !empty($col[1]) ? ' class="content"' : '';

				$html .= '<td'.$this->_arrayToAttrs($td).'>';
				$html .= '<div {INNER.'.$ck.'.'.$rk.'.CONTEXT}'.$divClass.'>{INNER.'.$ck.'.'.$rk.'}</div>';
				$html .= '</td>';
			}

			$html .= '</tr>';
		}

		$html .= '</table>';

		$this->assign('OUTER.MAINAREA', $html);
	}

	function setOneColumn() {
		$this->setSimpleInnerLayout(array('100%'), array('100%', ''));
	}

	function setTwoColumns($equalWidth=false) {
		$widths = $equalWidth ? array('50%', '50%') : array('25%', '75%');
		$this->setSimpleInnerLayout($widths, array('100%', ''));
	}

	function setThreeColumns() {
		$this->setSimpleInnerLayout(array('25%', '40%', '35%'), array('100%', ''));
	}

	// deprecate
	function setSimpleInnerLayout($widthArray, $heightArray) {
		$table = array();

		foreach ($heightArray as $hk => $height) {
			$table[$hk] = array();

			foreach ($widthArray as $wk => $width) {
				$table[$hk][$wk] = array($width, $height, NULL, NULL);
			}
		}

		$this->setInnerLayout($table);
	}

	function setOuterForm($action, $method, $enctype, $autoHeight=false) {
		$this->assign('OUTER.FORMBEGIN', '<FORM action="'.$action.'" method="'.$method.'" name="outer" enctype="'.$enctype.'" class="edit'.($autoHeight ? ' autoheight' : '').'">');
		$this->assign('OUTER.FORMEND', '</FORM>');
	}


	function setOuterLogo($image, $text, $width, $height, $href='') {
		if (!empty($image)) {
			$html = '<a href="'.$href.'"><img src="'.$image.'" width="'.$width.'" height="'.$height.'" border="0" alt="'.$text.'" class="logo"></a>';
		} else {
			$pad = round($height-22)/2;
			$html = '<div style="width:'.$width.'; height:'.$height.'; text-align:center; padding:'.$pad.';"><h1><a href="'.$href.'" style="color:#FFFFFF;">'.$text.'</a></h1></div>';
		}

		$this->assign('OUTER.LOGO', $html);
	}

	function setMenu($menuItems, $current, $getParam='area', $buildHref=true) {
		$menu = new PXWidgetMenuTabbed;

		$menu->items    = $menuItems;
		$menu->selected = $current;
		$menu->varName  = $getParam;
		$menu->buildHref = $buildHref;

		$this->assign('OUTER.MENU', $menu);
	}

	function setLogoutForm($href) {
		$this->assign('OUTER.EXIT',  $this->template('form-logout.tmpl'));
		$this->assign('LOGOUT.HREF', $href);
		$this->assign('USER.TITLE', PXRegistry::getUser()->getTitle());
	}

	function setLoginForm($formAction, $formMethod, $namesArray, $valuesArray) {
		$this->assign('OUTER.MAINAREA',   $this->template('form-login.tmpl'));

		$this->assign('LOGIN.FORMACTION', $formAction);
		$this->assign('LOGIN.FORMMETHOD', $formMethod);

		$this->assign('LOGIN.LOGINNAME',    getFromArray($namesArray,  'login'));
		$this->assign('LOGIN.PASSWDNAME',   getFromArray($namesArray,  'passwd'));
		$this->assign('LOGIN.LOGINVALUE',   quot(getFromArray($valuesArray, 'login')));
		$this->assign('LOGIN.REFERERNAME',  getFromArray($namesArray,  'referer'));
		$this->assign('LOGIN.REFERERVALUE', quot(getFromArray($valuesArray, 'referer')));
		$this->assign('LOGIN.AREANAME',     getFromArray($namesArray,  'area'));
		$this->assign('LOGIN.AREAVALUE',    getFromArray($valuesArray, 'area'));
	}

	function setGetVarToSave($key, $value) {
		$this->getData[$key] = $value;
		$this->assign(strtoupper($key), $value);
	}

	function clearGetVar($key) {
		unset($this->getData[$key]);
		$this->assign(strtoupper($key), '');
	}

	function clear($label) {
		$this->assign($label, null);
	}

	// alias for assign
	function set($label, &$value) {
		$this->labels[$label] = array();
		$this->add($label, $value);
	}

	function assign($label, $value) {
		$refValue = $value;
		$this->set($label, $refValue);
	}

	public
	function isWidget($value) {
		return is_object($value) && $value instanceof PXAdminWidgetIF;
	}

	// alias for append
	function add($label, &$value) {
		if(!isset($this->labels[$label]) || !is_array($this->labels[$label])) {
			$this->labels[$label] = array();
		}

		switch(true) {
			case $this->isWidget($value):
				$this->labels[$label][] =& $value;
				break;

			case is_scalar($value):
				$this->labels[$label][] = (string)$value;
				break;

			case is_array($value):
				$this->labels[$label][] = (array)$value;
				break;

			case is_null($value):
				// pass
				break;

			default:
				FatalError('Undefined type for layout content '.d3($value));
				break;
		}
	}

	function append($label, $value) {
		$refValue = $value;
		$this->add($label, $refValue);
	}

	function assignKeyValueList($label, $list, $varName, $selected) {
		$list = new PxAdminList($list);

		$list->setVarName($varName);
		$list->setSelected($selected);
		$list->setGetData($this->getData);

		$this->assign($label, $list);
	}

	function html() {
		// widgets to html
		// replace labels to html
		while(list($label, $widgets) = each($this->labels)) {
			$html = '';

			foreach ($widgets as $w) {
				$html .= $this->isWidget($w) ? $w->html() : $w;
			}

			$this->html = str_replace('{'.$label.'}', $html, $this->html);
		}

		// delete labels without content
		$this->html = preg_replace('/\{\w[\w\d_\.]+[\w\d]\}/', '', $this->html);

		return $this->html;
	}

	function flush($charset = NULL) {
		$result   =  $this->html();
		$response =& PXResponse::getInstance();

		if ($charset) {
			if (defined('DEFAULT_CHARSET') && $charset != DEFAULT_CHARSET) {
				$result = myconv(DEFAULT_CHARSET, $charset, $result);
				$response->setCharset($charset);
			}
		}

		$response->send($result);
	}

	// static
	function _buildHref($key, $value) {
		return NLAbstractLayout::buildHref($key, $value);
	}

	// static
	public static function buildHref($key, $value, $getData=array(), $href="?") {
		$layoutData = PXRegistry::getLayout()->getData;
		$getData    = array_merge($layoutData, $getData);

		parse_str($href, $href_vars);

		foreach ($getData as $k=>$v) {
			if (strval($v) != "" && $k != $key && empty($href_vars[$k])) {
				$href = appendParamToUrl($href, $k, $v);
			}
		}

		$href = parse_url(appendParamToUrl($href, $key, $value));
		return @"?{$href['query']}";
	}
}

?>
