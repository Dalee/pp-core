<?php

use PP\Lib\Config\Description\InitializableDescriptionInterface;

class PXPluginDescription implements InitializableDescriptionInterface {
	private $name     = null;
	private $class    = null;
	private $instance = NULL;
	private $app;

	public  $params   = array();
	public  $modules  = array();
	public  $triggers = array();

	/**
	 * {@inheritdoc}
	 */
	public function initialize(PXApplication $app) {
		$this->getPlugin($app);
	}

	public function getPlugin($app) {
		if ($this->instance) {
			return $this->instance;
		}

		$this->load();
		return ($this->instance = new $this->class($app, $this));
	}

	function setAttrs($attrs) {
		foreach ($attrs as $item) {
			$this->{$item->name} = pp_simplexml_decode_string($item->value);
		}
	}

	function setParams($params) {
		if (is_object($params)) {
			$params = array($params);
		}

		foreach ($params as $p) {
			$ps = $p->attributes();
			$this->params[$ps[0]->value] = $ps[1]->value;
		}
	}

	function setComponent($type, $field, $dom) {
		$objects = $dom->getChildObjects();

		if (empty($objects)) {
			return;
		}

		foreach ($objects->{$type} as $o) {
			$v = (string)$o;
			$this->{$field}[$v] = $v;
		}
	}

	function setModules($modules) {
		$this->setComponent('module', 'modules', $modules);
	}

	function setTriggers($triggers) {
		$this->setComponent('trigger', 'triggers', $triggers);
	}

	function load() {
		$file = $this->getPathToPlugin();

		if (is_string($file)) {
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
