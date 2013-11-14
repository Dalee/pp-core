<?php
require_once(BASEPATH."libpp/lib/XML/simplexml.classes.inc");


//FIXME: Добавить универсальный способ построения PXTypeDescription, а PXTypeDescriptionProperties убрать к чертям

class PXTypeDescriptionProperties extends PXTypeDescription {

	private function _fieldsAddCbk($field, $key) {
		$this->addField($field);
		$this->assignToGroup($field);
	}
	
	public function __construct($attributes) {
		$this->id = 'properties';
		$this->title = 'Настройки сайта';
		$fields = array();
		foreach ((array)$attributes as $formatString) {
			$field = $this->getField($formatString);
			$fields[$field->groupName][] = $field;
		}
		array_walk_recursive($fields, array($this, '_fieldsAddCbk'));
	}
	
	protected function getField($formatString) {
		$formatString = str_replace('|','&', $formatString);
		$dataDesc     = array();
		parseStrMagic($formatString, $dataDesc);
		
		if(!isset($dataDesc['name'])) {
			FatalError("В конфиге модуля для одного из полей отсутствует обязательный параметр name!");
		}
		list($sectionName, ) = $this->getSectionAndName($dataDesc['name']);

		$dataDesc['description'] = isset($dataDesc['description']) ? pp_simplexml_encode_string($dataDesc['description']) : $dataDesc['name'];
		$dataDesc['storagetype'] = isset($dataDesc['storagetype']) ? $dataDesc['storagetype'] : 'string';
		$dataDesc['displaytype'] = isset($dataDesc['displaytype']) ? $dataDesc['displaytype'] : 'TEXT';
		$app = PXRegistry::getApp();
		if(isset($dataDesc['source'], $app->directory[$dataDesc['source']])) {
			//all directories already filled
			$_tmpSource = $dataDesc['source'];
			unset($dataDesc['source']);
		}
		$dataDesc['displaytype'] = str_replace(',','|', $dataDesc['displaytype']);
		$dataDesc = $this->preparationData($dataDesc);
		
		$field         = new PXFieldDescription($dataDesc, $app, $this);
		$field->listed = false;
		
		if(isset($_tmpSource)) {
			$field->source = $_tmpSource;
			$field->values = &$app->directory[$_tmpSource];
			unset($_tmpSource);
		}
		if(isset($sectionName)) {
			$field->groupName = "Секция [${sectionName}]";
		}
		return $field;
	}
	
	protected function preparationData($data) {
		$attr = new SimpleXMLElement("<attribute/>");
		foreach($data as $k => $v) {
			$attr->addAttribute($k, $v);
		}
		return new PXmlSimplexmlNode($attr);
	}
	
	public function removeField($fieldName) {
		if(isset($this->fields[$fieldName])) {
			$fieldGroup = $this->fields[$fieldName]->groupName;
			unset($this->fields[$fieldName]);
			foreach($this->fieldGroups as $id => $group) {
				if (isset($group[$fieldGroup][$fieldName])) {
					unset($this->fieldGroups[$id][$fieldGroup][$fieldName]);
				}
			}
		}
	}
	
	public function getSectionAndName($param) {
		$param = explode('/', $param, 2);
		count($param) < 2 && array_unshift($param, null);
		return $param;
	}
	
}


class PXModuleProperties extends PXModule {
	var $_settings;
	var $_pseudoDatatype;

	public function __construct($area, $settings) {
		parent::__construct($area, $settings);
		$this->_settings = $settings;
	}

	protected function _fillPseudoDatatype() {
		$this->_pseudoDatatype = new PXTypeDescriptionProperties((array) $this->_settings['attribute']);
	}

	protected function _checkForErrors() {
		if(!isset($this->_settings['attribute']) || !sizeof($this->_settings['attribute'])) {
			return "Сайт не предусматривает пользовательских настроек.";
		}
		if(!is_writable(BASEPATH . '/site/etc/properties.ini')) {
			return "Файл properties.ini не доступен для записи или не существует";
		}
		
		return false;
	}


	public function adminIndex() {
		$layout = $this->layout;
		$db     = $this->db;
		
		$layout->setOneColumn();

		if(($error = $this->_checkForErrors()) !== false) {
			$layout->assignError("INNER.0.0", $error);
			return;
		}

		$this->_fillPseudoDatatype();
		$propObject = $this->loadModel($this->_pseudoDatatype);

		$db->loadDirectoriesByType($this->_pseudoDatatype, $propObject);
		$objectForm = new PXAdminForm($propObject, $this->_pseudoDatatype);

		$objectForm->setAction($this->request->getAction());
		$objectForm->setTitle($this->_pseudoDatatype->title);
		$html = $this->editArea($objectForm->form());

		$layout->Assign("INNER.0.0", $html);
	}

	public function adminAction() {
		if(($error = $this->_checkForErrors()) !== false) {
			FatalError($error);
		}
		
		$this->_fillPseudoDatatype();
		$propObject = $this->request->getContentObject($this->_pseudoDatatype);
		$this->saveModel($this->_pseudoDatatype, $propObject);
	}

	protected function editArea($objectForm) {
		return
<<<HTML
			<h2>{$this->_pseudoDatatype->title}</h2>

			<form action="/admin/action.phtml" method="POST" enctype="multipart/form-data" class="autoheight">
				<input type="hidden" name="area" value="properties">
				{$objectForm}
				<input type="submit" class="button" value="Сохранить">
			</form>
HTML;
	}

	protected static function parsePropertiesIni($withLines = true) {
		$result = array();
		$lineData = array();
		if(file_exists($store = BASEPATH . '/site/etc/properties.ini')) {
			$lines = file($store);
			$matches = array();
			$currentSection = &$result;
			$section = '';
			foreach ($lines as $number => $line) {
				$line=trim($line, "\r\n");
				switch (true) {
					case preg_match('/^\[[\w\d_-]+\]\s*$/', $line):
						if($withLines) {
							unset($lineData["placeholder_{$section}"]);
							$lineData["placeholder_{$section}"] = array();
							$lineData[$number] = $line;
						}
						$section = trim($line, '][ ');
						$currentSection = &$result[$section];
						$currentSection = array();
						break;
					case preg_match('/^(;?[\w\d_-]+)\s*=\s*("?)(.*)\\2\s*$/', $line, $matches):
						list(,$name, , $val) = $matches;
						$currentSection[$name] = str_replace('\"','"', $val);
						$withLines && $lineData[$number][$name] = &$currentSection[$name];
						break;
					default:
						$withLines && $lineData[$number] = $line;
				}
			}
			if($withLines) {
				unset($lineData["placeholder_{$section}"]);
				$lineData["placeholder_{$section}"] = array();
			}
		}
		return array(&$result, &$lineData);
	}
	
	protected function loadModel($format) {
		list($data, ) = self::parsePropertiesIni(false);
		$object = array();
		$fieldsToHide = array();
		$storageParam = array('db'=>$this->db, 'format'=>$format->id);
		foreach ($format->fields as $name => $field) {
			list ($section, $param) = $format->getSectionAndName($name);
			switch(true) {
				case !empty($section) && isset($data[$section][$param]):
					$object[$name] = &$data[$section][$param];
					break;
					
				case empty($section) && isset($data[$param]) && !is_array($data[$param]):
					$object[$name] = &$data[$param];
					break;
					
				case empty($section) ? isset($data[";$param"]) : isset($data[$section][";$param"]):
					$format->removeField($name);
					continue 2;
			}
			$object[$name] = $field->storageType->normalize($field, $object, $storageParam);
		}
		return $object;
	}

	protected function saveModel($format, $object) {
		if(!is_writable($store = BASEPATH . '/site/etc/properties.ini')) {
			return false;
		}

		list($data, $lines) = self::parsePropertiesIni();
		$normParam = array('dbFields' => null, 'dbValues' => null);
		
		foreach ($format->fields as $name => $field) {
			list($value,,) = $field->storageType->normalizeObjectAttribute($field, $object, $normParam);
			list ($section, $param) = $format->getSectionAndName($name);

			empty($section) ? $place = &$data[$param] : $place = &$data[$section][$param];
			isset($place)  || $place = &$lines["placeholder_{$section}"][$param];
			$place = $value;
		}
		
		$toWrite = array();
		foreach ($lines as $k => $line) {
			if(!is_array($line)) {
				$toWrite[] = $line;
			} else {
				foreach($line as $param => $value) {
					if(!empty($value) && is_string($value)) {
						$value = trim(str_replace('"', '\"', $value));
						$toWrite[] = "$param = \"$value\"";
					}
				}
			}
		}

		WriteStringToFile($store, join("\n", $toWrite));
		return true;
	}
}
?>
