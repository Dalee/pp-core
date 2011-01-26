<?php

final class PXMultipleRegionsReflexer {
	public $db;
	public $app;

	private $where;
	private $format;
	private $formatId;
	private $parent = null;

	private static $store  = array();
	private static $wheres = array();

	const REFLEX_FIELD = 'sys_reflex_id';

	public function __construct($regions, $where, $format) {
		$this->regions = $regions;
		$this->where   = preg_replace("#\s+#", " ", $where);

		$this->format   = $format;
		$this->formatId = $format->id;

		PXRegistry::assignToObject($this);

		if(isset($this->app->types[$format->parent]) && $regions->isRegionalObject($this->app->types[$format->parent])) {
			$this->parent = $this->app->types[$format->parent];
		}
	}

	public function getWhere() {
		/* if disabled: do not change where */
		if (PXMultiRegions::getInstance()->isDisabled()) {
			return $this->where;
		}
	
		if (isset(self::$wheres[$this->where])) {
			return self::$wheres[$this->where];
		}

		if ($this->regions->isRegionalObject($this->format) || is_object($this->parent)) {
			$this->setReflexId();
		}

		if (is_numeric($this->getReflexId())) {
			$oldWhere = $this->where;
			self::$wheres[$oldWhere] = $this->where = $this->modifyWhere();
		}

		return $this->where;
	}

	private function modifyWhere() {
		$reflexId = $this->getReflexId();
		
		$modified = preg_replace("#({$this->formatId}\.)?(parent) in ([^\)\s]+)#i", "$1$2 in $3,'{$reflexId}'",  $this->where);
		$modified = preg_replace("#({$this->formatId}\.)?(parent) = ([^\)\s]+)#i", "$1$2 in ($3,'{$reflexId}')", $modified);

		return $modified;
	}

	private function getReflexId() {
		return !empty(self::$store[$this->formatId]['refId']) 
			? self::$store[$this->formatId]['refId']
			: null;
	}

	private function getParentType() {
		return !empty(self::$store[$this->formatId]['parent']) 
			? self::$store[$this->formatId]['parent']
			: null;
	}

	private function setReflexId() {
		$reflexId = $this->getReflexId();

		preg_match(sprintf("#%s\.(id|parent) = [^0-9]*?([0-9]+)#", $this->formatId) , $this->where, $m);

		# or not id in where
		# or saved ReflexId == currentReflexId
		# or saved TypeParent == currentTypeParent
		if(!isset($m[2]) || $reflexId == $m[2] || ($reflexId && $this->getParentType() == $this->format->parent)) {
			return;
		}

		$result = $this->db->query(
			$s = sprintf("select id, %s from %s where %s = '%s'",
				self::REFLEX_FIELD,
				$this->parent ? $this->parent->id : $this->formatId,
				$this->parent ? "id" : $m[1],
				$m[2]
			)
		);

		if (!empty($result)) {
			$result = reset($result);
		}

		if (isset($result[self::REFLEX_FIELD])) {
			self::$store[$this->formatId] = array(
				"refId"  => $result[self::REFLEX_FIELD],
				"parent" => $this->format->parent);
		}
	}
}

?>
