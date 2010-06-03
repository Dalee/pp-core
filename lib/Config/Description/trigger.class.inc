<?php
/**
 * Класс - хранилище информации о триггерах системы.
 * Стоится по etc/triggers.xml /triggers/type/trigger.
 *
 * @subpackage Application
 */
class PXTriggerDescription {
	private $name         = NULL;
	private $description  = NULL;
	private $type         = NULL;
	private $instance     = NULL;
	private $folder       = NULL;

	public function getTrigger() {
		if($this->instance) {
			return $this->instance;
		}
		$this->load();
		return ($this->instance = new $this->klass());
	}

	function setAttrs($attributes) {
		foreach($attributes as $item) {
			$this->{$item->name} = utf8_decode($item->value);
		}
	}

	function setType($type){
		$this->type = $type;
	}

	function getName() {
		return $this->name;
	}

	function getType() {
		return $this->type;
	}

	/**
	* Подключает файл триггера, проверяет сначала local/triggers, затем libpp/triggers
	*/
	function load() {
		$file = $this->getPathToClass();

		if(is_string($file)) {
			include_once $file;
		}

		$this->klass = sprintf('PXTrigger%s%s', ucfirst($this->type), ucfirst($this->name));
		
		if(!(strlen($this->name) && class_exists($this->klass) && is_subclass_of($this->klass, sprintf("PXAbstract%sTrigger", $this->type)))){
			FatalError("Триггер '{$this->klass}' отсутствует или его интерфейс не совпадает с типом '{$this->type}'");
		}
	}
	
	function getPathToClass() {
		$file = strtolower("triggers/{$this->type}/{$this->name}.trigger.inc");

		if (file_exists(BASEPATH.'/local/'.$file)) {
			return BASEPATH.'/local/'.$file;

		} elseif (file_exists(BASEPATH.'/libpp/'.$file)) {
			return BASEPATH.'/libpp/'.$file;

		} elseif (file_exists(BASEPATH.'/local/plugins/' . $this->folder . '/' .$file)) { 
			return BASEPATH.'/local/plugins/'. $this->folder . '/' .$file;

		} elseif (file_exists(BASEPATH.'/libpp/plugins/' . $this->folder . '/' .$file)) {
			return BASEPATH.'/libpp/plugins/'. $this->folder . '/' .$file;

		}
	}
}
?>
