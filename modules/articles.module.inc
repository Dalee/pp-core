<?php
class PXModuleArticles extends PXAbstractModule {
	function userIndex() {
		$articles = $this->layout->getVar('articles', array());
 
		if(isset($this->objects['html'])) {
			foreach($this->objects['html'] as $o) {
				$articles[str_replace('.html', '', $o['pathname'])] = $o;
			}
		}
 
		$this->layout->Assign('articles', $articles);
	}
}
 
?>
