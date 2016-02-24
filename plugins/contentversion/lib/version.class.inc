<?

class PXContentVersion {
	public $app;
	public $db;

	public $object = array();
	public $objectInDb = array();

	public $type;
	private $newObject;

	public function __construct($type, $object, $objectInDb=false, $newObject=true) {
		PXRegistry::assignToObject($this);

		$this->newObject  = $newObject;

		$this->versionId  = (int) PXRegistry::getRequest()->getVar('version');
		$this->type       = $type;

		$this->original   = $object;
		$this->object     = $object;
		$this->objectInDb = $objectInDb;

		$this->app->plugins['contentversion']->params['no_modify_query'] = true;
	}

	public static function find($type, $object, $objectInDb) {
		$versionObject = new PXContentVersion($type, $object, $objectInDb, false);

		$versionObject->original = $object;

		$versionObject->object = $versionObject->versionId < 1
			? $object
			: $versionObject->findBySysOriginal();

		return $versionObject;
	}

	# it is original object
	# find by versionId
	private function findBySysOriginal() {
		$where = sprintf("sys_original = '%s' and sys_version = '%s'", $this->object['id'], $this->versionId);
		return pos($this->db->getObjectsByWhere($this->type, null, $where));
	}

	public function save() {
		return $this->newObject ? $this->insert() : $this->update();
	}

	private function insert() {
		$this->createNewVersion();
		$this->updateOriginal();
	}

	private function update() {
		$this->createNewVersion();
		$this->backToVersion();
	}

	private function updateOriginal() {
		$this->skip();

		$this->db->modifyContentObject($this->type, $this->original);

		$this->unskip();
	}

	public static function deleteOriginal($type, $id) {
		PXRegistry::getDB()->modifyingQuery("update {$type->id} set status = false where id = '{$id}'");
	}

	private function backToVersion() {
		$o = $this->object;

		$o['id'] = $o['sys_original'];

		$o['sys_original'] = null;
		$o['sys_modifyer'] = null;
		$o['sys_version']  = null;

		$o['is_version'] = 1;

		return $this->db->modifyContentObject($this->type, $o);
	}

	private function createNewVersion() {
		$o = $this->objectInDb;

		##no create version for status and sys fields change
		##TODO: write me
		$diff = array_diff($o, $this->object);

		if(empty($diff)) {
			return;
		}

		$lastVersion = $this->getLastVersion();

		$o['sys_original'] = $o['id'];
		$o['sys_modifyer'] = PXRegistry::getUser()->id;
		$o['sys_version']  = $lastVersion['max'] + 1;
		$o['id'] = null;

		$id = $this->db->addContentObject($this->type, $o);
		//$this->proccedFiles($o['sys_original'], $id, $lastVersion);
	}

	#TODO: WRITE ME/*{{{*/
	private function proccedFiles($from, $to, $lastVersion) {
		$notDbFields = array_filter($this->type->fields, array($this, "filterFields"));
		$typeDir = BASEPATH . "/site/htdocs/ai/{$this->type->id}/";

		foreach(array_keys($notDbFields) as $k) {
			if(!isset($this->original[$k])) {
				$edited[] = $k;
			}
		}

		d2($this->original);

		/*if(file_exists($typeDir . $o['id'])) {
			copyr($typeDir . "/" . $o['sys_original']  . "/" . $k, $typeDir . "/" . $id . "/" . $k);
		}*/
	}/*}}}*/

	private function getLastVersion() {
		$sql = sprintf("select id, max(sys_version) from %s where sys_original = '%s' group by id order by id desc",
			$this->type->id, $this->original['id']);

		$max = pos($this->db->query($sql));
		return !$max ? array('max' => 0, 'id' => null) : $max;
	}

	##
	## utils
	##

	function readAttribute($field) {
		return !isset($this->object[$field]) ? null : $this->object[$field];
	}

	function writeAttribute($field, $value) {
		$this->object[$field] = $value;
	}

	private function skip() {
		$this->app->plugins['contentversion']->params['skip_trigger'] = 1;
	}

	private function unskip() {
		unset($this->app->plugins['contentversion']->params['skip_trigger']);
	}

	private function log($message, $source) {
		PXAuditLogger::getLogger($this->app, $this->db)->info($message, $source);
	}

	private function filterFields($field) {
		return !$field->storageType->storedInDb() && !$field->storageType->name == 'virtual';
	}
}


?>
