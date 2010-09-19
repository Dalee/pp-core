<?php
class PXModuleSearch extends PXModule {
	var $word;

	function userIndex() {
		if(!$query = $this->getSearchText()) {
			return;
		}
		$chapters = $this->getChapters();
		$this->objects['search'] = $this->search($query, $chapters);
	}

	function getSearchText() {
		$query = trim($this->request->getVar('text'));
		if(!strlen($query)) {
			return '';
		}

		$this->layout->assign('text', $query);
		return $query; 
	}

	function getChapters() {
		$chapters = null;
		
		if (isset($this->settings['useParent'])) {
			$t = $this->request->GetVar('ch', array());
			$chapters = array();

			foreach ($t as $k) {
				if(is_numeric($k)) {
					$chapters[$k] = $this->tree->GetDescendantsOrSelf(array($k));
				}
			}
			if(!sizeof($chapters)) {
				$chapters[$this->tree->root->id] = $this->tree->GetDescendantsOrSelf(array($this->tree->root->id));
			}
		} 
		return $chapters;
	}

	function search($query, $chapters) {
		$per_page    = $this->app->getProperty('SEARCH_PER_PAGE', DEFAULT_CHILDREN_PER_PAGE);
		$currentPage = $this->request->getVar('page', 1);

		$search = new PXSearch($this->app, $this->db);
		$res = $search->doSearch($query, $chapters, null, $per_page, $currentPage);
		$search->Finish();

		$this->layout->assign('FP_SEARCH_TOTAL',        $search->getSearchCount());
		$this->layout->assign('FP_SEARCH_PER_PAGE',     $per_page);
		$this->layout->assign('FP_SEARCH_DEFAULT_PAGE', $currentPage);
		
		return $res;
	}
	
	function adminIndex() {
		$this->word = trim($this->request->GetVar('word'));
		$this->searchForm();

		if (strlen($this->word)) {
			$allCount = 0;

			$checkedTypes = $this->request->getVar('d', array());
			foreach($this->app->types as $datatype) {
				if(!isset($checkedTypes[$datatype->id])) {
					continue;
				}

				$allCount += $this->find($datatype);
			}

			if(!$allCount) {
				$this->layout->append('INNER.1.0', '<h2 class="error">Ничего не найдено</h2>');
			}
		}
	}

	function __sortTypes($a, $b) {
		return strcoll($a->title, $b->title);
	}

	function searchForm() {
		$datatypesHTML = '<ul>';

		$types = $this->app->types;
		uasort($types, array($this, '__sortTypes'));

		$checkedTypes = $this->request->getVar('d', array());

		foreach($types as $datatype) {
			$checked = !sizeof($checkedTypes) || isset($checkedTypes[$datatype->id]) ? 'checked' : '';
			$datatypesHTML .= <<<HTML

			<li>
				<input type="checkbox" name="d[{$datatype->id}]" id="d_{$datatype->id}" {$checked}>
				<label for="d_{$datatype->id}">{$datatype->title}</label>
			</li>
HTML;
		}

		$datatypesHTML .= '</ul>';


		$html = <<<HTML
		<style type="text/css">
			fieldset li, fieldset ul {
				padding: 0;
				margin: 0;
				list-style: none;
			}
			
			fieldset {
				margin: 1em 0;
			}
		</style>

		<form>
			<h2>Поиск</h2>

			<input type="hidden" name="area"      value="{$this->area}">
			<input type="hidden" name="charcheck" value="йцукен">

			<input type="text" style="width:80%;" name="word" value="{$this->word}">

			<input type="submit" value="Найти">

			<fieldset>
				<legend>искать по типам данных</legend>
				{$datatypesHTML}

				<button onclick="var inps = document.getElementsByTagName('fieldset')[0].getElementsByTagName('input'); for(i in inps) { inps[i].checked = !inps[i].checked; }; return false;">поменять выделение</button>
			</fieldset>
		</form>
HTML;

		$this->layout->assign('INNER.0.0', $html);
		$this->layout->setGetVarToSave('word', $this->word);

		// build the string for datatypes check
		foreach($checkedTypes as $k => $v){
			$this->layout->setGetVarToSave('d[' . $k . ']', 'on');
		}
		
		$this->layout->setGetVarToSave('charcheck', $this->request->getVar('charcheck'));
	}

	function find(&$datatype) {
		$count   = $this->db->getObjectsBySearchWord($datatype, NULL, $this->word, DB_SELECT_COUNT);

		if($count) {
			$onPage    = $this->app->getProperty('CHILDREN_ON_PAGE', 5);

			$currentPage  = $this->request->getVar($datatype->id.'_page', 1);
			$currentOrder = $this->request->getOrderVar($datatype->id, $datatype->order, $datatype->fields);
			$this->layout->setGetVarToSave($datatype->id.'_order', $currentOrder);

			$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

			$objects = $this->db->getObjectsBySearchWordLimited($datatype, NULL, $this->word, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

			$table = new PXAdminTable($objects, $datatype, $this->layout->getData);
			$table->addToParent('INNER.1.0');

			$table->setPager($currentPage, $onPage, $count);
		}

		return $count;

	}
}
?>
