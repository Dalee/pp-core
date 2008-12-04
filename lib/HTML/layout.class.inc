<?php
class PXUserHTMLLayout extends PXLayoutInterface  {
	var $smarty;
	var $_filters;

	function __construct() {
		parent::__construct();

		include_once 'Smarty/Smarty.class.php';

		$this->_initSmarty();
		$this->_filters  = array();
		$this->indexTemplate = 'index.tmpl';
		$this->addTemplateFunction('pager', array(&$this, 'pager'));
		$this->_initLang();
	}

	private function _initSmarty() {
		$this->smarty = new Smarty();
		$this->smarty->template_dir  = array(
			BASEPATH.'/local/templates/',
			BASEPATH.'/libpp/templates/'
		);

		$this->smarty->plugins_dir = array(
			BASEPATH.'/local/lib/smarty.plugins',
			BASEPATH.'/libpp/lib/smarty.plugins',
			'plugins', # Smarty default
		);

		$this->smarty->compile_dir   = BASEPATH . DIRECTORY_SEPARATOR . 'tmp'. DIRECTORY_SEPARATOR . 'smarty_templates_c';
		MakeDirIfNotExists($this->smarty->compile_dir);

		$this->smarty->config_dir    = NULL;
		$this->smarty->cache_dir     = NULL;
		$this->smarty->compile_check = true;
		$this->smarty->debugging     = false; // ?
		$this->smarty->caching       = false;

	}

	function setApp($app) {
		$this->app = $app;
		$this->lang->setTree($app->langTree);
	}
	
	private function _initLang() {
		include_once 'HTML/lang.class.inc';
		$this->lang = new PXUserHTMLLang();
		$this->addTemplateFunction('lang', array(&$this->lang, 'lang'));
	}
	
	function setLang($lang='rus') {
		$this->lang->setLang($lang);
	}
	
	function pager($param, &$smarty) {
		$app     = $smarty->_tpl_vars['app'];
		$request = $smarty->_tpl_vars['request'];

		$total = sizeof($param['objects']);

		/* Нужна ли постраничка? */
		$p = strtoupper($param['format']).'_PER_PAGE';

		if (isset($_COOKIE['NEWS_PAGES'])) {
			$a_per_page = $_COOKIE['NEWS_PAGES'];
		} else {
			$a_per_page = $app->GetProperty($p, $total);
		}

		$smarty->assign('a_per_page', $a_per_page);

		if ($a_per_page == 0) {
			return '';
		}

		$page = $request->GetVar('page', 1);
		$page = (int)preg_replace('/[^0-9]+/', '', $page);
		$page = $page > 0 ? $page : 1;

		$max_page = ceil($total/$a_per_page);

		if ($page > $max_page) {
			$page = $max_page;
		}

		$smarty->Assign('page', $page);
		$smarty->Assign('pages', $max_page);
		$smarty->Assign('max_page', $max_page);

		if(isset($param['notshow'])) {
			return;
		}

		return $this->html('misc/pager/pages.tmpl');
	}

	function getLangVar($path) {
		$param = array('get' => $path, 'return' => true);
		return $this->lang->lang($param, $this->smarty);
	}

	function assign($varName, $variable, $byReference=false) {
		if($byReference) {
			$this->smarty->assign_by_ref($varName, $variable);
		} else {
			$this->smarty->assign($varName, $variable);
		}
	}

	function assignRequest(&$request) {
		$this->smarty->_get     = $request->GetAllGetData();
		$this->smarty->_post    = $request->GetAllPostData();
		$this->smarty->_cookies = $request->GetAllCookieData();
	}

	function assignArray($variablesArray, $byReference=false) {
		foreach($variablesArray as $k=>$v) {
			$this->Assign($k, $v, $byReference);
		}
	}

	function getVar($varName) {
		return $this->smarty->get_template_vars($varName);
	}

	function changeIndexTemplate($filename='index.tmpl') {
		$this->indexTemplate = $filename;
	}

	function html($template) {
		return $this->smarty->fetch($template);
	}

	function Display() {
		$html = $this->html($this->indexTemplate);

		foreach($this->_filters as $filter=>$filterArgs) {
			if(function_exists($filter)) {
				array_unshift($filterArgs, $html);
				$html = call_user_func_array($filter, $filterArgs);
			}
		}

		echo $html;
	}

	function addFilter($functionName) {
		$args = func_get_args();
		array_shift($args);

		$this->_filters[$functionName] = $args;
	}

	function addTemplateFunction($templateBlock, $functionName) {
		$this->smarty->register_function($templateBlock, $functionName);
	}

	function removeFilter($functionName) {
		unset($this->_filters[$functionName]);
	}
}

?>
