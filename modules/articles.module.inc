<?php
class PXModuleArticles extends PXModule {
	function userIndex() {
		$articles = $this->layout->getVar('articles', array());

		if (isset($this->objects['html'])) {
			foreach ($this->objects['html'] as $o) {
				$key = str_replace('.html', '', $o['pathname']);
				array_key_exists($key, $articles) || ($articles[$key] = $o);
			}
		}

		$this->layout->assign('articles', $articles);
	}
}
