<?php
class PXCronRunSearchIndex extends PXAbstractCronRun {
	var $name = 'Индексация данных для поиска';

	function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		$this->db = $db;

		ini_set('display_errors', 0);
		$r = $this->_reindex();

		return array("status" => 0, "note" => "Успешно! obj/words: {$r[0]}/{$r[1]}");
	}

	function _init($reloadFlag, $quietFlag, $withIndexField) {
		if(isset($_SERVER['argv'][2]) && _StrToLower($_SERVER['argv'][2]) === 'full') {
			ini_set('memory_limit','-1'); //FIXME: this indexer must be completely rewritten, in full index mode it devour memory like a hungry Hulk :)
			$reloadFlag = true;
		}

		$this->quietFlag      = $quietFlag;
		$this->withIndexField = $withIndexField;
		$this->reloadFlag     = $reloadFlag;

		$this->search = new PXSearch($withIndexField);
	}

	function _reindex($reloadFlag = false, $quietFlag = false, $withIndexField = true) {
		$this->_init($reloadFlag, $quietFlag, $withIndexField);

		$this->label("start");
		$this->label("loading data");

		$expired = $this->reloadFlag ? $this->reIndexFull() : $this->indexUpdates();

		$stemsWeights = array();
		$totalr = $this->calcStemsWeight($expired, $stemsWeights);
		$totalt = $this->newRecords($stemsWeights);

		$this->_deleteNonExistents();

		$this->vacuum();

		$this->label("end");

		return array($totalr, $totalt);
	}

	function whereQuery() {
		$where = array();
		$where[] = $this->db->__mapUpdateData('status', '1');

		if($this->withIndexField) {
			$where[] = $this->db->__mapUpdateData('index', '1');
		}
		return implode(' AND ', $where);
	}

	function label($message) {
		if (!$this->quietFlag) {
			Label($message);
		}
	}

	function reIndexFull() {
		$expired = array();

		foreach ($this->search->config as $k => $v) {
			WorkProgress();
			$expired[$k] = array();

			$tmp = $this->db->query("SELECT id, ".join(", ", array_keys($v))." FROM {$k} WHERE ".$this->whereQuery(), true);

			foreach($tmp as $i => $row) {
				foreach($row as $rk => $rv) {
					if ($rk === 'id') continue;

					$row[$rk] = _StrToLower($rv);
				}

				$expired[$k][$i] = $row;
			}
		}

		WorkProgress(true);

		$this->label("deleting data");

		$this->delete();
		$this->vacuum();

		return $expired;
	}

	function indexUpdates() {
		$expired = array();

		$fname = BASEPATH.'/site/var/search.idx';
		if (!file_exists($fname)) {
			WriteStringToFile($fname, '');
		}

		$f = fopen($fname, "r+");
		flock($f, LOCK_EX);
		$nums = array();
		$acts = array();

		while (!feof($f)) {
			WorkProgress();

			$arr = explode("/", fgets($f));

			if (sizeof($arr) < 4) break;

			if ($arr[1] == 'U') {
				$acts['U'][$arr[3]][$arr[2]] = true;
				$acts['D'][$arr[2]] = true;

			} elseif ($arr[1] == 'D') {
				if (isset($acts['U'][$arr[3]][$arr[2]])) {
					unset($acts['U'][$arr[3]][$arr[2]]);
				}

				$acts['D'][$arr[2]] = true;
			}
		}

		WorkProgress(true);

		fseek($f, 0, SEEK_SET);
		ftruncate($f, 0);
		flock($f, LOCK_UN);
		fclose($f);

		$this->label("deleting data");

		if (isset($acts['D']) && count($acts['D'])) {
			$toDelete = array();
			$row = 0;
			$count = 0;
			foreach($acts['D'] as $k=>$v) {
				$toDelete[$row][$k] = $v;
				$count++;

				if($count >= 500) {
					$count = 0;
					$row++;
				}
			}

			foreach($toDelete as $delKeys) {
				$this->delete("did IN (".join(',', array_keys($delKeys)).")");
			}
		}

		$this->vacuum();

		foreach ($this->search->config as $k => $v) {
			WorkProgress();
			if (!isset($acts['U'][$k]) || !count($acts['U'][$k])) continue;

			$expired[$k] = array();

			$toSelect = array();
			$row = 0;
			$count = 0;
			foreach($acts['U'] as $kk=>$vv) {
				$toSelect[$row][$kk] = $vv;
				$count++;

				if($count >= 500) {
					$count = 0;
					$row++;
				}
			}

			$tmp = array();
			foreach($toSelect as $selKeys) {
				$q = "SELECT id, ".join(", ", array_keys($v))." FROM {$k} WHERE ".$this->whereQuery();
				$q = $q." AND id IN (".join(',', array_keys($selKeys[$k])).")";
				$tmp = array_merge($tmp, $this->db->query($q, true));
			}

			foreach($tmp as $i => $row) {
				foreach($row as $rk => $rv) {
					if ($rk === 'id') continue;
					$row[$rk] = _StrToLower($rv);
				}

				$expired[$k][$i] = $row;
			}
		}
		WorkProgress(true);

		return $expired;
	}

	function calcStemsWeight($expired, &$stemsWeights) {
		$totalr = 0;

		static $dt; //Workaround for PHP 4.4.x bug, memory corruption !

		foreach ($expired as $corrupted_dt => $v) {
			$dt = $corrupted_dt;
			WorkProgress();
			$totalc = count($v);

			$this->label("idx: {$dt} ({$totalc})");

			foreach ($v as $k => $row) {
				++$totalr;
				$totalWeights = array();

				foreach ($this->search->config[$dt] as $field => $weight) {
					preg_match_all("/[\xa3\xc0-\xdfa-z\d]+/s", $this->search->strip($row[$field]), $m);

					foreach($m[0] as $word) {
						$stem = $this->search->getStemByWord(_yo2ye($word));

						if (is_null($stem)) {
							continue;
						}

						if (!isset($totalWeights[$stem])) {
							$totalWeights[$stem] = 0;
						}

						$totalWeights[$stem] += $weight;
					}
				}

				foreach ($totalWeights as $stem => $weight) {
					$stemsWeights[] = array($stem, $row['id'], $dt, $weight);
				}
			}
		}
		WorkProgress(true);
		$this->search->finish();
	}

	function newRecords($stemsWeights) {
		$total = count($stemsWeights);
		if ($total > 0) {
			$this->label("copy rec ($total})");

			$isOK = $this->db->modifyingCopy('sitesearch', array('stem', 'did', 'dtype', 'weightsum'), $stemsWeights);

			if($isOK !== true){
				$this->label('Error while saving index !');
			}
		}

		return $total;
	}

	function vacuum() {
		$this->label("vacuum");
		$this->db->vacuumTable("sitesearch");
		$this->db->clearCache();

	}

	function _deleteNonExistents() {
		$tree = $this->db->getObjects($this->db->types['struct'], true, DB_SELECT_TREE);

		$sitesearch = $this->db->query("SELECT did, dtype FROM sitesearch");

		$idsForDelete = array();

		$tmp = array();
		foreach($sitesearch as $row) {
			WorkProgress();
			$tmp[$row['dtype']][] = $row['did'];
		}
		WorkProgress(true);

		foreach($tmp as $dtype => $ids) {
			WorkProgress();
			$ids = array_unique($ids);

			if (!isset($this->db->types[$dtype]->fields['parent'])) {
				$idsForDelete[$dtype] = $ids;

			} else {
				$actualObjects = $this->db->query("SELECT id, parent FROM " . $dtype ."  WHERE id IN (" . implode(',', $ids) . ") AND status=true");

				foreach($actualObjects as $k => $tmpId) {
					if(!isset($tree->leafs[$tmpId['parent']])) {
						unset($actualObjects[$k]);
					}
				}

				$diff = array_diff($ids, GetColFromTable($actualObjects, 'id'));
				if (!empty($diff)) {
					$idsForDelete[$dtype] = $diff;
				}
			}
		}

		WorkProgress(true);

		foreach($idsForDelete as $dtype => $ids) {
			$this->delete(array(
				"dtype ='{$dtype}'",
				"did IN (" . implode(',', $ids) . ")"
			));
		}
	}

	function delete($where = null) {
		$sql = 'DELETE FROM sitesearch';

		if(is_array($where)) {
			$sql .= ' WHERE '.implode(' AND ', $where);
		}

		$this->db->query($sql);
	}
}

?>