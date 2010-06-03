<?php
class PXVersion extends PXAdminWidget {
	private $request;
	private $app;
	private $db;
	private $layout;

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

	public function html() {
		$html = '';
		$this->initVars();
		if($this->request->getAction() != 'contentversion') {
			return '';
		}

		if(!isset($this->object) || !isset($this->type)) {
			$this->layout->append('OUTER.CONTENT','Не указан объект');
			return;
		}

		if(!is_null($this->object['sys_version'])) {
			$this->showVersion();
		} else {
			$this->showList();
		}
		return '';
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
		$this->layout->append('OUTER.CONTENT', implode(" ", $html));
		$this->layout->setMenu(array(), null);
		$this->layout->assignTitle(sprintf('Версия %s объекта %s', $this->object['sys_version'], $this->type));
		$this->layout->assign('OUTER.LEFTCONTROLS',  '');
		$this->layout->assign('OUTER.RIGHTCONTROLS', '');
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

		$data = new PVersionData($this->type, "sys_original = " . $this->object['id'] . " AND sys_version <> -1 OR sys_version IS NOT NULL");
		$count = $data->getCount();
		if($count == 0) {
			$this->layout->append('OUTER.CONTENT', 'Нет версий');
		} else {
			$data = $data->getData($page, $onPage, $count);
			$this->table($cols, $data, $caption = 'Список версий');
			$this->pager($page, $onPage, $count);
		}
	}

	private function table($cols, $data, $caption) {
		$table = new PVersionsTable($cols);
		$table->setDict('datatypes', $this->app->types[$this->type]);
		$table->setData($data);
		$table->setCaption($caption);
		$this->layout->append('OUTER.CONTENT', $table->html());
	}

	private function pager($page, $onPage, $count) {
		$param = $this->request->getAllGetData();

		foreach($param as $k => $v) {
			$param[$k] = myconv('k', 'u', $param[$k]);
		}

		$pager = new PXAdminPager($page, $onPage, $count, '',  $param);
		$this->layout->append('OUTER.CONTENT', $pager->html());
	}

}
?>
