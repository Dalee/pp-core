<?php
function SortByRelevant($a, $b) {
	if (($r = NumericCmp($b['words'], $a['words'])) != 0) {
		return $r;
	} else {
		return NumericCmp($b['sum'], $a['sum']);
	}
}

function _yo2ye($s) {
	return strtr($s, "ёЁ", "еЕ");
}

class PXSearch {
	var $app;
	var $db;

	var $stopwords;
	var $config;

	var $word2stem;
	var $stem2id;

	var $dbah;

	var $withIndexField;

	function __construct($withIndexField = true) {
		$this->app = PXRegistry::getApp();
		$this->db  = PXRegistry::getDB();
		$this->withIndexField = $withIndexField;
		$this->searchCount    = 0;

		$this->loadStopwords(
			'а без более бы был была были было быть в вам вас весь во вот все всего мочь можно
			всех всей вы где да даже для до его ее если есть еще же за здесь и из или им их к как который
			которую которая которых	ко когда кто ли либо мне может мы на надо наш наша наше не него нее нет ни них но ну о
			об однако он она они оно от очень по под при с со так также такой такая там те тем то
			того тоже той только том ты у уже хотя чего чей чем что чтобы чье чья эта эти это я
			через любой любая этого http www ru другой другая'
		);

		$this->loadConfig();
		$this->word2stem = array();
		$this->stem2id = array();

		$fname = BASEPATH.'/site/var/word2stem.cdb';
		$this->dbah = null;
		if (file_exists($fname)) {
			$this->dbah = dba_open($fname, 'r', 'cdb');
		}
	}

	function doSearch($query, $parents = NULL, $allowTypes=NULL, $limit = NULL, $page = NULL) {
		$this->parents = $parents;

		$stemsArray = $this->_getStemsArray($query);
		if (!count($stemsArray)) {
			return array();
		}

		$sql = $this->_getStemsSQL($stemsArray, $allowTypes);
		$this->searchCount = $this->_searchCount($sql);

		if(!$this->searchCount) {
			return array();
		}

		list($dtypes, $tmpres) = $this->_loadStems($sql, $limit, $page);

		return $this->searchObjects($dtypes, $tmpres);
	}

	function finish() {
		if(is_resource($this->dbah)) {
			dba_close($this->dbah);
		}
	}

	function loadStopwords($s) {
		preg_match_all("/\w+/s", $s, $m);
		$this->stopwords = array();
		foreach ($m[0] as $v) {
			$this->stopwords[$v] = true;
		}
	}

	function loadConfig() {
		$this->config = array();
		foreach($this->app->types as $k => $v) {
			if ($this->withIndexField && !isset($v->fields['index'])) {
				continue;
			}

			$this->config[$k] = array();

			foreach ($v->fields as $type) {
				if ($type->storageType->name != 'string') continue;
				if ($type->name == 'pathname'     ) continue;
				if ($type->noindex                ) continue;

				$this->config[$k][$type->name] = ($type->name == 'title' ? 5 : 1);
			}

			if (count($this->config[$k]) == 0) {
				unset($this->config[$k]);
			}
		}
	}

	function getStemByWord($word) {
		if (strlen($word) <= 2 || strlen($word) > 30) {
			return NULL;
		}

		if (isset($this->stopwords[$word])) {
			return NULL;
		}

		if (isset($this->word2stem[$word])) {
			return $this->word2stem[$word];
		}

		if(!is_resource($this->dbah)) {
			return NULL;
		}

		if (($stem = dba_fetch($word, $this->dbah)) !== false) {
			if (isset($this->stopwords[$stem])) {
				return NULL;
			}

			$this->word2stem[$word] = $stem;
			return $stem;

		} else {
			$this->word2stem[$word] = $word;
			return $word;
		}

		return NULL;
	}

	function getStemsByArray($a) {
		$result = array();
		foreach ($a as $v) {
			$s = $this->getStemByWord($v);

			if (!is_null($s)) {
				$result[] = $s;
			}
		}
		return $result;
	}

	function strip($s) {
		$s = preg_replace("/<SCRIPT.*?\\/SCRIPT>/is", '', $s); //
		$s = preg_replace("/<.*?>/s", ' ', $s);
		$s = preg_replace("/&nbsp;/is", ' ', $s);
		$s = html_entity_decode($s, ENT_QUOTES); // FIXME: 'KOI8-R' ?
		$s = preg_replace("/&.*?;/s", ' ', $s);
		return $s;
	}

	function escapeString($word) {
		return "'".$this->db->escapeString($word)."'";
	}

	private
	function _getStemsSQL($stemsArray, $allowTypes) {
		$where = array(
			"stem IN (".join(',', array_map(array($this, 'escapeString'), $stemsArray)).")"
		);

		if(isset($allowTypes) && is_array($allowTypes)) {
			$allowTypes = array_flip($allowTypes);
			$where[] = "dtype IN (".join(',', array_map(array($this, 'escapeString'), $allowTypes)).")";
		}

		foreach(PXRegistry::getApp()->triggers->search as $t) {
			$t->getTrigger()->onBeforeSearchStems($this->config, $where);
		}

		$where = implode(' AND ', $where);
		$totalwords = count($stemsArray);

		return <<<SQL
			SELECT 
				did,
				dtype,
				count(*) as words,
				sum(weightsum) as sum
			FROM
				sitesearch
			WHERE
				{$where}
			GROUP BY 
				dtype, did
			HAVING
				count(*) = {$totalwords}
			ORDER BY 
				words DESC, sum DESC
SQL;

	}

	private
	function _searchCount($sql) {
		$tmp = $this->db->Query("SELECT count(*) FROM (" . $sql . ") AS tmp");

		if (!isset($tmp[0]['count'])) {
			return 0;
		}

		return $tmp[0]['count'];
	}

	private
	function _getLimitPair($limit, $page) {
		if($limit && $page) {
			return array(
				$limit,
				$limit * ($page - 1)
			);
		} else {
			return null;
		}
	}

	private
	function _loadStems($sql, $limit, $page) {
		$tmp = $this->db->query($sql, false, $this->_getLimitPair($limit, $page));

		$tmpres = array();
		$dtypes = array();

		foreach($tmp as $k => $v) {
			if(!isset($this->app->types[$v['dtype']])) {
				continue;
			}

			$key = $v['dtype'].' '.$v['did'];
			$tmpres[$key] = array (
				'did'   => $v['did'],
				'dtype' => $v['dtype'],
				'sum'   => $v['sum'],
				'words' => $v['words']
			);

			$dtypes[$v['dtype']][$v['did']] = true;
		}

		return array($dtypes, $tmpres);
	}

	private
	function _getStemsArray($query) {
		$query = preg_replace("/&(#\d+|[a-z]+);/s", '', $query);
		preg_match_all("/\w+/s", _StrToLower(_yo2ye($query)), $tmp);
		return array_unique($this->getStemsByArray(array_unique($tmp[0])));
	}

	function _withParents($type) {
		return isset($this->parents) && is_array($this->parents) && sizeof($this->parents) && isset($this->app->types[$type]->fields['parent']);
	}

	function searchObjects($dtypes, $tmpres) {
		$result = array();

		if (!is_null($this->parents)) {
			$tmpParents = array();
			$tmpJoin    = array();

			foreach ($this->parents as $k => $v) {
				foreach($v as $i) {
					$tmpParents[$i] = $k;
					$tmpJoin[] = $i;
				}
			}
		}

		foreach ($dtypes as $k => $v) {
			if (!count($v)) {
				continue;
			}

			$where = array();
			$where[] = sprintf("id IN ('%s')", join("','", array_keys($v)));

			if($this->withIndexField) {
				$where[] = $this->db->__mapUpdateData('index', '1');
			}

			$where[] = "status = '1'";

			if ($this->_withParents($k)) {
				$where[] = sprintf("parent IN ('%s')", join("','", array_values($tmpJoin)));
			}

			foreach(PXRegistry::getApp()->triggers->search as $t) {
				$t->getTrigger()->onBeforeSearchObjects($k, $where);
			}

			$tmp = $this->db->getObjectsByWhere($this->app->types[$k], true, $where);

			foreach($tmp as $row) {
				$key =  $k.' '.$row['id'];

				if(!isset($tmpres[$key])) {
					continue;
				}

				foreach(PXRegistry::getApp()->triggers->search as $t) {
					$t->getTrigger()->onAfterSearchObjects($k, $row);
				}

				$result[$key] = $tmpres[$key];
				$result[$key]['data'] = $row;

				if ($this->_withParents($k)) {
					$result[$key]['ancestor'] = $tmpParents[$row['parent']];
				}
			}
		}

		usort($result, 'SortByRelevant');
		return $result;
	}

	function getSearchCount() {
		return $this->searchCount;
	}
}
?>
