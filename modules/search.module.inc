<?php
class PXModuleSearch extends PXEngineAPI {
	function indexAdmin() {
		$word = $this->request->getVar('word');

		$this->layout->setSimpleInnerLayout(array('100%'), array('100%'));
		$this->layout->setGetVarToSave('word',      $word);
		$this->layout->setGetVarToSave('charcheck', $this->request->getVar('charcheck'));

		$this->word = trim($word);

		$this->form();

		if (strlen($this->word)) {
			$this->search();
		}
	}

	function search() {
		$notFound = true;

		foreach($this->app->types as $type) {
			$count   = $this->db->getObjectsBySearchWord($type, NULL, $this->word, DB_SELECT_COUNT);

			if($count) {
				$this->searchResult($type, $count);
				$notFound = false;
			}
		}

		if($notFound) {
			$this->layout->append('INNER.0.0', '<h2 class="error">Ничего не найдено</h2>');
		}
	}

	function searchResult($type, $count) {
		$onPage       = $this->app->getProperty('CHILDREN_ON_PAGE', 5);

		$currentOrder = $this->request->getVar($type->id.'_order', $type->order);
		$currentPage  = $this->request->getVar($type->id.'_page', 1);
		$currentPage  = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;


		$objects = $this->db->getObjectsBySearchWordLimited($type, NULL, $this->word, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

		$table = new PXAdminTable($objects, $type, $this->layout->getData);
		$table->setPager($currentPage, $onPage, $count);

		$this->layout->append('INNER.0.0', $table->getTable());
		$this->layout->setGetVarToSave($type->id.'_order', $currentOrder);
	}

	function form() {
		$area = $this->request->getArea('search');
		$word = $this->word;

		$html = 
<<<HTML
	<h2>Поиск</h2>

	<form class="autoheight">
		<input type="hidden" name="area"      value="$area">
		<input type="hidden" name="charcheck" value="йцукен">

		<input type="text"   name="word"      value="$word" style="width: 75%;">
		<input type="submit" value="Найти">
	</form>
HTML;

		$this->layout->append('INNER.0.0', $html);
	}
}
?>
