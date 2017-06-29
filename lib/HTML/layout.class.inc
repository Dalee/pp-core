<?php

use PP\Lib\Html\Layout\LayoutInterface;

/**
 * TODO: Refactor and move this file to src/Lib/Html/Layout
 *
 * Class PXUserHTMLLayout
 */
class PXUserHTMLLayout implements LayoutInterface  {
	var $smarty;
	var $_filters;

	/** @var string  */
	protected $indexTemplate;

	/** @var PXUserHTMLLang */
	protected $lang;

	function __construct() {
		require_once PPCOREPATH . 'vendor/Smarty/Smarty.class.php';

		$this->_initSmarty();

		$this->_filters  = [];
		$this->indexTemplate = 'index.tmpl';

		$this->addTemplateFunction('property', [$this, 'getProperty']);
		$this->addTemplateModifier('property', [$this, 'getPropertyModifier']);
		$this->addTemplateFunction('pager', [$this, 'pager']);

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

		$this->smarty->compile_dir   = CACHE_PATH.'/smarty_templates_c';

		MakeDirIfNotExists($this->smarty->compile_dir);

		$this->smarty->config_dir    = NULL;
		$this->smarty->cache_dir     = NULL;
		$this->smarty->compile_check = true;
		$this->smarty->debugging     = false; // ?
		$this->smarty->caching       = false;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setApp(\PXApplication $app) {
		$this->lang->setTree($app->langTree);

		return $this;
	}

	private function _initLang() {
		require_once PPLIBPATH . 'HTML/lang.class.inc';

		$this->lang = new PXUserHTMLLang();
		$this->addTemplateFunction('lang', array($this->lang, 'lang'));
		$this->addTemplateModifier('lang', array($this->lang, 'lang_modifier'));

		$this->addTemplateFunction('pager', array($this, 'pager'));
		$this->addTemplateFunction('autopager', array($this, 'autopager'));
	}

	function htmlPager($totalObjects, $objectsPerPage, &$smarty, $param) {
		$maxPage = ceil($totalObjects/$objectsPerPage);

		$request     = PXRegistry::getRequest();

		$currentPage = $request->GetVar('page', $this->getVar('FP_' . mb_strtoupper($param['format']) . '_DEFAULT_PAGE', 1));
		$currentPage = (int)preg_replace('/[^0-9]+/'.REGEX_MOD, '', $currentPage);
		$currentPage = $currentPage > 0 ? $currentPage : 1;

		if ($currentPage > $maxPage) {
			$currentPage = $maxPage;
		}

		$smarty->assign('a_per_page', $objectsPerPage);
		$smarty->assign('page',       $currentPage);
		$smarty->assign('pages',      $maxPage);
		$smarty->assign('max_page',   $maxPage);

		if(isset($param['notshow'])) {
			return;
		}
		return $this->html('misc/pager/pages.tmpl');
	}

	function autopager($param, &$smarty) {
		$total      = $this->getVar('FP_' . mb_strtoupper($param['format']) . '_TOTAL');
		$a_per_page = $this->getVar('FP_' . mb_strtoupper($param['format']) . '_PER_PAGE');

		if (!isset($total) || !isset($a_per_page)) {
			return '';
		}

		return $this->htmlPager($total, $a_per_page, $smarty, $param);
	}

	function pager($param, &$smarty) {
		$total = sizeof($param['objects']);

		/* Нужна ли постраничка? */
		$p = mb_strtoupper($param['format']).'_PER_PAGE';

		$a_per_page = (int)PXRegistry::getApp()->getProperty($p, $total);
		if ($a_per_page == 0) {
			return '';
		}

		return $this->htmlPager($total, $a_per_page, $smarty, $param);
	}

	/**
	 * Get property inside template, function
	 *
	 * @param $param
	 * @param $smarty
	 * @return string
	 */
	function getProperty($param, &$smarty) {
		$default = '';
		if (isset($param['default'])) {
			$default = $param['default'];
		}

		return PXRegistry::getApp()->getProperty($param['name'], $default);
	}

	/**
	 * Get property inside template, modifier
	 *
	 * @param string $name
	 * @return string
	 */
	function getPropertyModifier($name) {
		return PXRegistry::getApp()->getProperty($name);
	}

	/**
	 * {@inheritdoc}
	 */
	public function setLang($lang='rus') {
		$this->lang->setLang($lang);

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getLang() {
		return $this->lang;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getSmarty() {
		return $this->smarty;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getIndexTemplate() {
		return $this->indexTemplate;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setContent($content) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getContent(){
		return null;
	}

	function getLangVar($path) {
		$param = array('get' => $path, 'return' => true);
		return $this->lang->lang($param, $this->smarty);
	}

	/**
	 * TODO: remove byReference flag to conform LayoutInterface
	 * {@inheritdoc}
	 */
	public function assign($varName, $variable, $byReference=false) {
		if($byReference) {
			$this->smarty->assign_by_ref($varName, $variable);
		} else {
			$this->smarty->assign($varName, $variable);
		}

		return $this;
	}

	function assignByRef($varName, &$variable) {
		$this->smarty->assign_by_ref($varName, $variable);
	}

	function assignRequest($request) {
		$this->smarty->_get     = $request->GetAllGetData();
		$this->smarty->_post    = $request->GetAllPostData();
		$this->smarty->_cookies = $request->GetAllCookieData();
	}

	function assignArray($variablesArray, $byReference=false) {
		foreach($variablesArray as $k=>$v) {
			$this->Assign($k, $v, $byReference);
		}
	}

	function getVar($varName, $default=NULL) {
		$var = $this->smarty->get_template_vars($varName);
		return (isset($var) ? $var : $default);
	}

	function &getVarByRef($varName) {
		$variable =& $this->smarty->get_template_vars($varName);
		return $variable;
	}

	function changeIndexTemplate($filename='index.tmpl') {
		$this->indexTemplate = $filename;
	}

	function html($template) {
		return $this->smarty->fetch($template);
	}

	function display() {
		$html = $this->html($this->indexTemplate);

		foreach ($this->_filters as $filter => $filterArgs) {
			if (strstr($filter, '::')) {
				$filter = explode('::', $filter);

				if (count($filter) > 2) {
					trigger_error(sprintf('malformed filter %s', join('::', $filter)), E_WARNING);
					continue;
				}
			}

			if (is_callable($filter)) {
				array_unshift($filterArgs, $html, $this);
				$html = call_user_func_array($filter, $filterArgs);
			}
		}

		return $html;
	}

	function addFilter($functionName) {
		$args = func_get_args();
		array_shift($args);

		if (is_callable($functionName)) {
			$this->_filters[is_array($functionName) ? join('::', $functionName) : $functionName] = $args;
		}
	}

	// back-compat. deprecated
	function addModifier($name, $callback) {
		$this->addTemplateModifier($name, $callback);
	}

	function addTemplateModifier($name, $callback) {
		$this->smarty->register_modifier($name, $callback);
	}

	function addTemplateFunction($name, $callback) {
		$this->smarty->register_function($name, $callback);
	}

	function addTemplateBlock($name, $callback, $cacheable = true) {
		$this->smarty->register_block($name, $callback, $cacheable);
	}

	function removeFilter($functionName) {
		unset($this->_filters[$functionName]);
	}
}
