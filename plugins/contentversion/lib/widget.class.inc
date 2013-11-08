<?php
class PXVersion extends PXAdminWidget {
	private $request;
	private $app;
	private $db;
	private $layout;

	function __construct($part) {
		$this->part = $part;
	}

	private $children_on_page = 20;

	private function initVars() {
		$this->request = PXRegistry::getRequest();
		$this->app     = PXRegistry::getApp();
		$this->db      = PXRegistry::getDB();
		$this->layout  = PXRegistry::getLayout();

		$id = $this->request->getId();
		$this->type = $this->request->getVar('format');
		$this->object = $id != null ? $this->db->getObjectById($this->app->types[$this->type], $id) : null;
	}

	function html_leftcontrols() {
		if($this->is_version_object()) {
			$html[] = NLAbstractHTMLForm::BuildHidden('sys_original', $this->object['sys_original']);
			$html[] = NLAbstractHTMLForm::BuildSubmit('Принять версию');
			
			return join($html);
		}

		return '';
	}

	function html_content() {
		if($this->is_version_action()) {
			if(!$this->is_version_object()) {
				return $this->showList();
			} else {
				return $this->showVersion();
			}
		} else {
			if($this->is_version_object()) {
				PXResponse::getInstance()
					->redirect(NLAbstractLayout::buildHref('action', 'contentversion'));
			}
		}

		return '';
	}

	function html_title() {
		if($this->is_version_object()) {
			return sprintf('Версия %s объекта %s', $this->object['sys_version'], $this->type);
		}

		return '';
	}

	function html_menu() {
		return '';
	}

	function is_version_action() {
		return PXRegistry::getRequest()->getAction() == 'contentversion';
	}

	function is_version_object() {
		return isset($this->object['sys_version']) && !is_null($this->object['sys_version']);
	}

	public function html() {
		$this->initVars();
		$method = "html_{$this->part}";

		return $this->$method();
	}

	private function showVersion() {
		$html[] = "";
		$table = array();

		foreach($this->app->types[$this->type]->fields as $k => $v) {
			$table[] = <<<HTML
				<tr>
					<td>{$v->description}</td>
					<td>{$this->object[$v->name]}</td>
				</tr>
HTML;

		}
		$table = implode("", $table);
		$html[] = <<<HTML
		<table class="mainform">
			{$table}
		</table>
HTML;

		return join($html);
	}

	private function showList() {
		$cols = array(
			'back' => '',
			'sys_original' => 'Номер версии',
			'sys_modifyer'  => 'Автор',
			'sys_created'  => 'Дата создания'
		);

		$page    = $this->request->getVar('_page', 1);
		$onPage  = $this->children_on_page;

		$data = new PXVersionsData($this->type, "sys_original = " . $this->object['id'] . " AND sys_version <> -1 OR sys_version IS NOT NULL");
		$count = $data->getCount();

		if($count == 0) {
			return 'Нет версий';
		} else {
			$data = $data->getData($page, $onPage, $count);
			$html[] = $this->table($cols, $data, $caption = 'Список версий');
			$html[] = $this->pager($page, $onPage, $count);

			return join($html);
		}

		return '';
	}

	private function table($cols, $data, $caption) {
		$table = new PXVersionsTable($cols);
		$table->setDict('datatypes', $this->app->types[$this->type]);
		$table->setData($data);
		$table->setCaption($caption);
		return $table->html();
	}

	private function pager($page, $onPage, $count) {
		$param = $this->request->getAllGetData();

		foreach($param as $k => $v) {
			$param[$k] = myconv(DEFAULT_CHARSET, CHARSET_UTF8, $param[$k]);
		}

		$pager = new PXAdminPager($page, $onPage, $count, '',  $param);
		return $pager->html();
	}

}
?>
