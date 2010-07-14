<?php

class PXPluginContentVersion extends PXAbstractPlugin {
	function initialize($app) {
		PXLoader::getInstance($this->path . "/lib")
			-> load("PXContentVersion", "version.class.inc")
			-> load("PXVersion",        "widget.class.inc")
			-> load("PXVersionsData",   "data.class.inc")
			-> load("PXVersionsTable",  "table.class.inc")
			-> load("PXVersionsTab",    "tab.class.inc");
		
		require_once ($this->path . "/triggers/database/version.trigger.inc");
		require_once ($this->path . "/triggers/system/version.trigger.inc");
		require_once ($this->path . "/triggers/storagetype/version.trigger.inc");

		$this->loadTrigger("PXTriggerDatabaseVersion");
		$this->loadTrigger("PXTriggerStoragetypeVersion");
		$this->loadTrigger("PXTriggerSystemVersion");

		if ($app->isAdminEngine()) {
			require_once $this->path . '/decorators.php';
		}
	}


	private function cloneFile($field, &$obj) {
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
	}

}
?>
