<?php
class PXInputRichEdit extends PXAdminWidget {
	var $value;
	var $name;
	var $height;
	var $container;
	var $toLayout;
 
	function PXInputRichEdit($name, $value, $height) {
		$this->value     = $value;
		$this->name      = $name;
		$this->height    = $height;
		$this->container = $name . '_id';
		$this->toLayout  =& PXRegistry::getLayout();
		$this->favoriteEditor = null; //default value
	}
	
   /**
	* @param String $name String id of the RichEditor.
	* @param Array  $data RichEditor Helper. Struct - array('name' => 'My Favorite Editor', 'init_script' => $helper_object)
	*               $helper_object must have callable method init($richEdit), which returns string with JS initializer for the current rich edit area (optional) and $richEdit is a link to this widget
	*/
	function addEditor($name, $data) {
		PXInputRichEdit::_staticEditorsContainer('add', array($name => $data));
	}
 
	function removeEditor($name) {
		PXInputRichEdit::_staticEditorsContainer('delete', $name);
	}
 
	function _staticEditorsContainer($action, $args = null){ // it's PHP4, baby, no static support ;(
		static $container = array();
		switch($action){
			case 'fetch':
				return $container;
			case 'add':
				$container[key($args)] = reset($args);
				return true;
			case 'delete':
				unset($container[$args]);
				return true;
		}
	}
 
	function setFavoriteEditor($redactor) {
		if(!is_null($redactor)) {
			$this->favoriteEditor = $redactor;
		}
	}
 
	function _initEditor() {
		$editors = PXInputRichEdit::_staticEditorsContainer('fetch');
		$embedding = array(
			"types" => getColFromTableWithIndexs($editors, 'name'),
			"data" => null,
			"current" => null
		);
 
		$embedding['current'] = is_null($this->favoriteEditor) || !isset($embedding['types'][$this->favoriteEditor]) ? key($embedding['types']) : $this->favoriteEditor;
		if(isset($editors[$embedding['current']]['init_script']) && is_callable(array($editors[$embedding['current']]['init_script'], 'init'))){
			$embedding['data'] = $editors[$embedding['current']]['init_script']->init($this);
		}
 
		return $embedding;
	}
 
	function html() {
		$embedding = $this->_initEditor();
		$types     = new PXInputDropDown(null, $embedding['current'], $embedding['types']);
		$types     = $types->html();
		return <<<HTML
			<div class="redactor">
				<label>Выберите редактор</label>
				{$types}
			</div>
			<textarea name="{$this->name}" id="{$this->container}" style="width: 100%; height: {$this->height}px;" {$this->disable}>{$this->value}</textarea>
			{$embedding['data']}
HTML;
	}
}
?>