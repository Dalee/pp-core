<?php

class PXPluginContentVersion extends PXAbstractPlugin {
	private $request;
	private $app;
	private $db;
	private $layout;
	
	private $have_original;
	public $version;

	const SQL_SELECT_VERSION     = "sys_original = '%s' and sys_version = '%s'";
	const SQL_SELECT_MAX_VERSION = "select max(sys_version) from %s where sys_original = '%s'";

	private $meta = array(
		"remove"         => array("saveInVersion", true),
		"directremove"   => array("saveInVersion", true),
		"main"           => array("saveInVersion", true),
		"contentversion" => array("backVersion",   false));

	function initialize($app) {/*{{{*/
		$plugin_path = dirname($this->description->getPathToPlugin());

		PXLoader::getInstance($plugin_path . "/lib")
			-> load("PXVersion", "widget.class.inc")
			-> load("PXVersionsData",  "data.class.inc")
			-> load("PXVersionsTable", "table.class.inc")
			-> load("PXVersionsTab",   "tab.class.inc");
		
		require_once ($plugin_path . "/triggers/database/version.trigger.inc");
		require_once ($plugin_path . "/triggers/system/version.trigger.inc");
		require_once ($plugin_path . "/triggers/storagetype/version.trigger.inc");

		$app
			->registerTrigger("database",    array("name"=>"version"))
			->registerTrigger("system",      array("name"=>"version"))
			->registerTrigger("storagetype", array("name"=>"version"));

		if ($app->engine->engineClass() == PX_ENGINE_ADMIN) {
			require_once $plugin_path . '/decorators.php';
		}
	}/*}}}*/

	public function invoke($method, $type, $object, $version=false) {/*{{{*/
		$this->app     = PXRegistry::getApp();
		$this->db      = PXRegistry::getDB();
		$this->type    = $type;
		$this->object  = $object;
		$this->version = $version;

		return $this->$method();
	}/*}}}*/

	function __call($m, $a) {/*{{{*/
		if($_ = @$this->meta[$m]) {
			$returned_value = array_pop($_);

			foreach($_ as $method) {
				$this->$method();
			}

			return $returned_value;
		}

		return true;
	}/*}}}*/

	private function log($message, $source) {/*{{{*/
		PXAuditLogger::getLogger($this->app, $this->db)->info($message, $source);
	}/*}}}*/

	function saveInVersion() {
		$obj = $this->object;

		$obj['sys_original'] = $obj['id'];
		$obj['id']           = null;
		$obj['sys_modifyer'] = PXRegistry::getUser()->id;
		$obj['sys_version']  = $this->getEndVersion();

		$obj['id'] = $this->db->addContentObject($this->app->types[$this->type], $obj);

		if(!is_null($obj['id'])) {
			/*foreach ($this->app->types[$this->type]->fields as $k=>$v) {
				if(is_array($obj[$v->name]) && isset($obj[$v->name]['path'])) {
					$this->cloneFile($v, $obj);
				}
			}*/

			$this->log("Версия номер {$obj['sys_version']}", "{$this->type}/{$obj['id']}");
		}

		return $obj['id'];
	}

	private function getVersionObject($vId) {
		$type = PXRegistry::getApp()->types[$this->type];

		$this->description->params['no_modify_query'] = true;

		if(!$this->version) {
			$_ = pos($this->db->getObjectsByWhere($type, true, sprintf(self::SQL_SELECT_VERSION, $this->object['id'], (int) $vId)));
			return $_;
		}

		$obj = $this->object;
		$original = $this->db->getObjectById($type, $this->object['sys_original']);
		!empty($original) && $this->object = $original;

		$this->description->params['no_modify_query'] = false;
		
		return $obj;
	}

	public function backVersion() {
		$vId = (int) PXRegistry::getRequest()->getVar('version');
		if($vId < 1) return;

		$obj = $this->getVersionObject($vId);
		if(empty($obj)) return;

		if($this->object['id'] != $obj['id']) {
			$this->saveInVersion();
		}

		$obj['i_am_version'] = $this->version;
		$obj['sys_version']  = null;

		$this->log("Из версии {$vId}", sprintf("%s/%s", PXRegistry::getApp()->types[$this->type]->id, $obj['id']));
		$this->db->ModifyContentObject($this->app->types[$this->type], $obj);
	}

	private function getEndVersion() {
		$ver = pos($this->db->query(sprintf(self::SQL_SELECT_MAX_VERSION, $this->type, $this->object['id'])));
		return empty($ver) ? 1 : $ver['max'] + 1;
	}

	private function cloneFile($field, &$obj) {/*{{{*/
		$p = array('donor' => $this->object, 'format' => $this->app->types[$this->type]->id);

		$basepath = BASEPATH.'/site/htdocs';
		
		if(is_null($obj['sys_version'])) {
			if(is_link($basepath.$obj[$field->name]['path']) && $link = readlink($basepath.$obj[$field->name]['path'])) {
				$link = str_replace($basepath, '', $link);
				unlink($basepath.$this->object[$field->name]['path']);
				$obj[$field->name]['path'] = $link;
			}
			$p = array('donor' => $obj, 'format' => $this->app->types[$this->type]->id);
			$field->storageType->cloneFile($field, $this->object, $p);
		
		} elseif($obj['sys_version'] == 1) {
			$field->storageType->cloneFile($field, $obj, $p);
		
		} else {
			$versions = $this->db->getObjectsByWhere($this->app->types[$this->type], null, "sys_original = " . $this->object['id'] . " AND sys_version <> -1 OR sys_version IS NOT NULL");
			$cT = fileatime($basepath.$this->object[$field->name]['path']);
			foreach($versions as $v) {
				if(isset($v[$field->name]['path']) && !is_link($basepath.$v[$field->name]['path']) && $cT == fileatime($basepath.$v[$field->name]['path'])) {
					$p = array('donor' => $v, 'format' => $this->app->types[$this->type]->id);
					$field->storageType->cloneFile($field, $obj, $p, true);
					return;
				}
			}
			$field->storageType->cloneFile($field, $obj, $p);
		}
	}/*}}}*/

}
?>
