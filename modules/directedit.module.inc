<?php
class PXModuleDirectEdit extends PXModule {
	function adminIndex() {
		$ppTypes = array();
		foreach($this->app->types as $type) {
			$ppTypes[] = $type->id.':"'.$type->title.'"';
		}

		$this->layout->setOneColumn();
		$this->layout->assign('OUTER.MAINAREA', $this->layout->template('modules/directedit.tmpl'));
		$this->layout->assign('PP.TYPES', implode(', ', $ppTypes));
	}
}
?>