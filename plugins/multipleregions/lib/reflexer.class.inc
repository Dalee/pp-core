<?php

final class PXMultipleRegionsReflexer {
	public $db;
	public $app;

	private $where;
	private $format;
	private $formatId;
	private $parent = null;

	static $wheres     = array();
	static $parentType = null;
	static $reflexId   = null;

	const REFLEX_FIELD = 'sys_reflex_id';

	public function __construct($regions, $where, $format) {
		$this->regions = $regions;
		$this->where = preg_replace("#\s+#", " ", $where);

		$this->format   = $format;
		$this->formatId = $format->id;

		PXRegistry::assignToObject($this);

		if(isset($this->app->types[$format->parent]) && $regions->isRegionalObject($this->app->types[$format->parent])) {
			$this->parent = $this->app->types[$format->parent];
		}
	}

	public function getWhere() {
		if(isset(self::$wheres[$this->where])) {
			return self::$wheres[$this->where];
		}

		if($this->regions->isRegionalObject($this->format) || is_object($this->parent)) {
			$this->setReflexId();
		}

		if(is_numeric(self::$reflexId)) {
			$oldWhere = $this->where;
			self::$wheres[$oldWhere] = $this->where = $this->modifyWhere();
		}

		return $this->where;
	}

	private function modifyWhere() {
		$reflexId = self::$reflexId;
		
		$modified = preg_replace("#({$this->formatId}\.)?(parent) in ([^\)\s]+)#i", "$1$2 in $3,'{$reflexId}'",  $this->where);
		$modified = preg_replace("#({$this->formatId}\.)?(parent) = ([^\)\s]+)#i", "$1$2 in ($3,'{$reflexId}')", $modified);

		return $modified;
	}

	private function setReflexId() {
		$reflexId = self::$reflexId;

		preg_match(sprintf("#%s\.(id|parent) = [^0-9]*?([0-9]+)#", $this->formatId) , $this->where, $m);

		# or not id in where
		# or saved ReflexId == currentReflexId
		# or saved TypeParent == currentTypeParent
		if(!isset($m[2]) || $reflexId == $m[2] || ($reflexId && self::$parentType == $this->format->parent)) {
			return;
		}

		$result = $this->db->query(
			sprintf("select id, %s from %s where %s = '%s'",
				self::REFLEX_FIELD,
				$this->parent ? $this->parent->id : $this->formatId,
				$this->parent ? "id" : $m[1],
				$m[2]
			)
		);

		if (!empty($result)) {
			$result = reset($result);
		}
		
		if(isset($result[self::REFLEX_FIELD])) {
			self::$reflexId   = $result[self::REFLEX_FIELD];
			self::$parentType = $this->format->parent;
		}
	}
}

?>
