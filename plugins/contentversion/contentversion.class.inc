<?php
require_once(BASEPATH . 'libpp/plugins/contentversion/data.class.inc');
require_once(BASEPATH . 'libpp/plugins/contentversion/table.class.inc');
require_once(BASEPATH . 'libpp/plugins/contentversion/decorators.php');

class PXPluginContentVersion extends PXAbstractPlugin {
	private $request;
	private $app;
	private $db;
	private $layout;

	function initSet() {
		$this->request = PXRegistry::getRequest();
		$this->app     = PXRegistry::getApp();
		$this->db      = PXRegistry::getDB();

		$id = $this->request->getId();
		$this->type = $this->request->getVar('format');
		$this->object = $id != null ? $this->db->getObjectById($this->app->types[$this->type], $id) : null;
	}

	function popupAction() {
		$this->initSet();
		$res = array('contentversion' => false, 'main' => true);
		$action = $this->request->getAction();
		if(empty($this->object) || empty($this->type) || !in_array($action, array_keys($res))) {
			return;
		}

		switch($action) {
			case 'contentversion': 
				$this->backVersion();
				break;

			case 'main':
				$this->saveInVersion();
				break;

			default:
				break;
		}

		return $res[$action];
	}

	function saveInVersion() {
		$obj = $this->object;
		$obj['sys_original'] = $obj['id'];
		$obj['id']           = null;
		$obj['sys_modifyer'] = PXRegistry::getUser()->id;
		$obj['sys_version']  = $this->getEndVersion();

		$obj['id'] = $this->db->addContentObject($this->app->types[$this->type], $obj);
		if(!is_null($obj['id'])) {
			foreach ($this->app->types[$this->type]->fields as $k=>$v) {
				if(is_array($obj[$v->name]) && isset($obj[$v->name]['path'])) {
					$this->cloneFile($v, $obj);
				}
			}
			PXAuditLogger::getLogger($this->app, $this->db)->info('Версия номер ' . $obj['sys_version'], $this->type .'/'. $obj['id']);
		}
		return $obj['id'];
	}

	private function backVersion() {
		$vId = $this->request->getVar('version');
		if(is_null($vId)) {
			return;
		}
		$obj = current($this->db->getObjectsByWhere($this->app->types[$this->type], null, 'sys_original =' . $this->object['id'] . ' AND sys_version = ' . $vId . 'OR sys_version IS NOT NULL'));
		if(empty($obj)) {
			return;
		}

		$this->saveInVersion();
		$obj['id'] = $this->object['id'];
		$vers = $obj['sys_version'];
		$obj['sys_version'] = null;

		foreach ($this->app->types[$this->type]->fields as $k=>$v) {
			if(is_array($obj[$v->name]) && isset($obj[$v->name]['path'])) {
				$this->cloneFile($v, $obj);
			}
		}

		$this->db->ModifyContentObject($this->app->types[$this->type], $obj);
		PXAuditLogger::getLogger($this->app, $this->db)->info('Из версии' . $vers,  $this->app->types[$this->type]->id .'/'. $obj['id']);
	}

	private function getEndVersion() {
		$sql = <<<SQL
			SELECT max(sys_version) FROM {$this->type}
					WHERE sys_original = {$this->object['id']}
SQL;
		$ver = current($this->db->query($sql));
		return empty($ver) ? 1 : $ver['max'] + 1;
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
