<?
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

	function PXSearch(&$app, &$db, $withIndexField = true) {
		$this->app = &$app;
		$this->db  = &$db;
		$this->withIndexField = $withIndexField;

		$this->loadStopwords('а без более бы был была были было быть в вам вас весь во вот все всего мочь можно
								всех всей вы где да даже для до его ее если есть еще же за здесь и из или им их к как который
								которую которая которых ко когда кто ли либо мне может мы на надо наш наша наше не него нее нет ни них но ну о
								об однако он она они оно от очень по под при с со так также такой такая там те тем то
								того тоже той только том ты у уже хотя чего чей чем что чтобы чье чья эта эти это я
								через любой любая этого ru другой другая');
		$this->loadConfig();
		$this->word2stem = array();
		$this->stem2id = array();

		$fname = BASEPATH.'/site/var/word2stem.cdb';
		$this->dbah = null;
		if (file_exists($fname)) {
			$this->dbah = dba_open($fname, 'r', 'cdb');
		}
	}

	function Finish() {
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
			if ($this->withIndexField && !isset($v->fields['index'])) continue;
			$this->config[$k] = array();
			foreach ($v->fields as $type) {
				if ($type->storageType != 'string') continue;
				if ($type->name == 'pathname') continue;
				$this->config[$k][$type->name] = ($type->name == 'title' ? 5 : 1);
			}
			if (count($this->config[$k]) == 0) unset($this->config[$k]);
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
			if (!is_null($s)) $result[] = $s;
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

	function doSearch($query, $parents = NULL, $allowTypes=NULL) {
		preg_match_all("/\w+/s", _StrToLower(_yo2ye($query)), $tmp);
		$stemsArray = array_unique($this->getStemsByArray(array_unique($tmp[0])));

		if (!count($stemsArray)) {
			return array();
		}

		if (!is_null($parents)) {
			$tmpParents = array();
			$tmpJoin = array();
			foreach ($parents as $k => $v) {
				foreach($v as $i) {
					$tmpParents[$i] = $k;
					$tmpJoin[] = $i;
				}
			}
		}

		$totalwords = count($stemsArray);

		$tmp = $this->db->Query("SELECT did, dtype, weightsum, stem FROM sitesearch WHERE stem IN (".join(',', array_map("DbQuoteString", $stemsArray)).")");

		$tmpres = array();
		$dtypes = array();

		foreach($tmp as $k => $v) {
			$key = $v['dtype'].' '.$v['did'];
			if (isset($tmpres[$key])) {
				$tmpres[$key]['sum']   += $v['weightsum'];
				$tmpres[$key]['words'] += 1;
				$tmpres[$key]['diffs'][$v['stem']] = 1;
			} else {
				$tmpres[$key] = array(
					'did'   => $v['did'],
					'dtype' => $v['dtype'],
					'sum'   => $v['weightsum'],
					'words' => 1,
					'diffs' => array($v['stem'] => 1)
				);
			}

			$dtypes[$v['dtype']][$v['did']] = true;
		}

		$tmpres2 = array();
		foreach ($tmpres as $k => $v) {
			if (count($v['diffs']) == $totalwords) {
				$tmpres2[$k] = $v;
			} else {
				unset($dtypes[$v['dtype']][$v['did']]);
			}
		}

		$result = array();

		if(isset($allowTypes) && is_array($allowTypes)) {
			$allowTypes = array_flip($allowTypes);
		}

		if($this->withIndexField) {
			if ($this->app->dbDescription['database']->dbtype == 'mssql') {
				$_indexField = "[index] = '1' AND";
			} else {
				$_indexField = "index = '1' AND";
			}
		} else {
			$_indexField = '';
		}

		foreach($dtypes as $k => $v) {
			if (!count($v)) {
				continue;
			}

			if(is_array($allowTypes) && !isset($allowTypes[$k])) {
				continue;
			}

			if(isset($parents) && is_array($parents) && sizeof($parents) && isset($this->app->types[$k]->fields['parent'])) {
				$withParents = true;
			} else {
				$withParents = false;
			}

			$tmp = $this->db->Query("SELECT * FROM {$k} WHERE id IN (".join(',', array_keys($v)).") AND {$_indexField} status = '1'".(!$withParents ? '' : " AND parent IN (".join(',', array_values($tmpJoin)).")"));

			foreach($tmp as $row) {
				$key =  $k.' '.$row['id'];
				if(!isset($tmpres2[$key])) {
					continue;
				}

				$result[$row['id']] = $tmpres2[$key];
				$result[$row['id']]['data'] = $row;

				if ($withParents) {
					$result[$row['id']]['ancestor'] = $tmpParents[$row['parent']];
				}
			}
		}

		usort($result, 'SortByRelevant');
		return $result;
	}

}

?>
