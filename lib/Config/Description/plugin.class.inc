<?php
class PXPluginDescription {
	private $name     = null;
	private $class    = null;
	private $instance = NULL;

	public function initialize($app) {
		$this->getPlugin($app);
	}

	public function getPlugin($app) {
		if($this->instance) {
			return $this->instance;
		}
		$this->load();
		return ($this->instance = new $this->class($app, $this));
	}

	function setAttrs($attrs) {
		foreach($attrs as $item) {
			$this->{$item->name} = utf8_decode($item->value);
		}
	}

	function load() {
		$file = $this->getPathToPlugin();

		if(is_string($file)) {
			require_once($file);
		}
	}

	function getName() {
		return $this->name;
	}

	function getPathToPlugin() {
		$file = strtolower("plugins/{$this->name}/{$this->name}.class.inc");
		if (file_exists(BASEPATH.'/local/'.$file)) {
			return BASEPATH.'/local/'.$file;

		} elseif (file_exists(BASEPATH.'/libpp/'.$file)) {
			return BASEPATH.'/libpp/'.$file;
		}
	}

	function __sleep() {
		$o_vars = get_object_vars($this);
		unset($o_vars['instance']);
		return array_keys($o_vars);
	}
}
?>
