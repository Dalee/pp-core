<?php
include_once('Common/functions.array.inc');
include_once('Common/functions.string.inc');

define('TABLECOLOR1',       '#E7E9ED');
define('TABLECOLOR2',       '#C9CED6');
define('TABLECOLOR3',       '#385A94');

class NLAbstractLayout {
	var $html   = '';
	var $labels = array();
	var $blocks = array();

	var $getData;

	function NLAbstractLayout() {
		$this->getData      = array();

		$this->template_dirs = array(
			BASEPATH.'local/templates/admin/',
			BASEPATH.'libpp/templates/admin/'
		); 
	}

	function setOuterLayout($template) {
		$this->html = $this->template($template.'.tmpl');
	}

	function template($filename) {
		foreach($this->template_dirs as $dir) {
			if(file_exists($dir.$filename)) {
				return file_get_contents($dir.$filename);
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

	function setTwoColumns() {
		$this->setSimpleInnerLayout(array('25%', '75%'), array('100%', ''));
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
			$html = '<a href="'.$href.'"><img src="'.$image.'" width="'.$width.'" height="'.$height.'" border="0" alt="'.$text.'"></a>';
		} else {
			$pad = round($height-22)/2;
			$html = '<div style="width:'.$width.'; height:'.$height.'; text-align:center; padding:'.$pad.';"><h1><a href="'.$href.'" style="color:#FFFFFF;">'.$text.'</a></h1></div>';
		}

		$this->assign('OUTER.LOGO', $html);
	}

	function setMenu($menuItems, $current, $getParam='area', $buildHref=true) {
		$menu = new PXWidgetTabbedMenu($menuItems, $buildHref);
		$menu->setSelected($current);
		$menu->setVarName($getParam);

		$this->assign('OUTER.MENU', $menu);
	}

	function setLogoutForm($href) {
		$this->assign('OUTER.EXIT',  $this->template('form-logout.tmpl'));
		$this->assign('LOGOUT.HREF', $href);
	}

	function setLoginForm($formAction, $formMethod, $namesArray, $valuesArray) {
		$this->assign('OUTER.MAINAREA',   $this->template('form-login.tmpl'));

		$this->assign('LOGIN.FORMACTION', $formAction);
		$this->assign('LOGIN.FORMMETHOD', $formMethod);

		$this->assign('LOGIN.LOGINNAME',    getFromArray($namesArray,  'login'));
		$this->assign('LOGIN.PASSWDNAME',   getFromArray($namesArray,  'passwd'));
		$this->assign('LOGIN.LOGINVALUE',   getFromArray($valuesArray, 'login'));
		$this->assign('LOGIN.REFERERNAME',  getFromArray($namesArray,  'referer'));
		$this->assign('LOGIN.REFERERVALUE', getFromArray($valuesArray, 'referer'));
		$this->assign('LOGIN.AREANAME',     getFromArray($namesArray,  'area'));
		$this->assign('LOGIN.AREAVALUE',    getFromArray($valuesArray, 'area'));
	}

	function setGetVarToSave($key, $value) {
		$this->getData[$key] = $value;
		$this->assign(strtoupper($key), is_array($value) ? 'ARRAY' : $value);
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
		switch(true) {
			case is_object($value):
				unset($this->labels[$label]);
				$this->blocks[$label] = array();
				$this->blocks[$label][] =& $value;
				break;

			case is_bool($value):
			case is_integer($value):
			case is_string($value):
				unset($this->blocks[$label]);
				$this->labels[$label] = (string)$value;
				break;

			case is_null($value):
				unset($this->labels[$label]);
				unset($this->blocks[$label]);
				break;

			default:
				FatalError('Undefined type for layout content '.d3($value));
				break;
		}
	}

	function assign($label, $value) {
		$refValue = $value;
		$this->set($label, $refValue);
	}

	// alias for append
	function add($label, &$value) {
		switch(true) {
			case is_object($value):
				if(!isset($this->blocks[$label]) || !is_array($this->blocks[$label])) {
					$this->blocks[$label] = array();
				}

				$this->blocks[$label][] =& $value;
				break;

			case is_bool($value):
			case is_integer($value):
			case is_string($value):
				if(!isset($this->labels[$label]) || !is_string($this->labels[$label])) {
					$this->labels[$label] = '';
				}

				$this->labels[$label] .= (string)$value;
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
		foreach($this->blocks as $label=>$widgets) {
			$html = '';

			foreach($widgets as $w) {
				$html .= $w->html();
			}

			$this->assign($label, $html);
		}

		// replace labels to html
		foreach ($this->labels as $label=>$html) {
			$this->html = str_replace('{'.$label.'}', $html, $this->html);
		}

		// delete labels without content
		$this->html = preg_replace('/\{\w[\w\d_\.]+[\w\d]\}/', '', $this->html);

		return $this->html;
	}

	function flush($charset = NULL) {
		$result = $this->html();

		if($charset) {
			if(defined('DEFAULT_CHARSET') && $charset != DEFAULT_CHARSET) {
				$result = myconv(DEFAULT_CHARSET, $charset, $result);

			} else {
				$charset = null;
			}
		}

		$response =& PXResponse::getInstance();
		$response->setCharset($charset);
		$response->send($result);
	}

	// static
	function _buildHref($key, $value) {
		return NLAbstractLayout::buildHref($key, $value);
	}

	// static
	function buildHref($key, $value) {
		$href = "?";

		$layout =& PXRegistry::getLayout();

		foreach ($layout->getData as $k=>$v) {
			if (strval($v) != "" && $k != $key) {
				if (is_array($v)) {
					foreach ($v as $sk=>$sv) {
						$href .= $k.'[]='.urlencode($sv).'&';
					}

				} else {
					$href .= $k.'='.urlencode($v).'&';
				}
			}
		}

		$href .= $key.'='.$value.'&';
		$href = substr($href, 0, -1);

		return $href;
	}
}

?>
