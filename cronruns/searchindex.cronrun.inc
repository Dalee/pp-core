<?
class PXCronRunSearchIndex extends PXAbstractCronRun {
	var $name;

	function PXCronRunSearchIndex() {
		$this->name = 'Индексация данных для поиска';
	}

	function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		$r = $this->_reindex();
		return array("status" => 0, "note" => "Успешно! obj/words: {$r[0]}/{$r[1]}");
	}

	function _reindex($reloadFlag = false, $quietFlag = false, $withIndexField = true) {
		ini_set('display_errors', 0);
		if(isset($_SERVER['argv'][2]) && _StrToLower($_SERVER['argv'][2]) === 'full') {
			$reloadFlag = true;
		}

		$app = PXRegistry::getApp();
		$db  = PXRegistry::getDb();

		$dbtype = $app->dbDescription['database']->dbtype;

		$search  = new PXSearch($withIndexField);

		if ($dbtype == 'pgsql') {

		} elseif ($dbtype == 'mssql') {
			$makequote = create_function('$a', 'return "\'".str_replace("\'", "\'\'", $a)."\'";');
		}

		$expired = array();

		$whereQuery = ($dbtype == 'pgsql') ? "status = '1' " : "status = '1' ";

		if($withIndexField) {
			$whereQuery .= ($dbtype == 'pgsql') ? "AND \"index\" = '1'" : "AND [index]   = '1'";
		}

		if (!$quietFlag) Label("start");
		if (!$quietFlag)  Label("loading data");

		if ($reloadFlag) {
			foreach ($search->config as $k => $v) {
				WorkProgress();
				$expired[$k] = array();

				$tmp = $db->query("SELECT id, ".join(", ", array_keys($v))." FROM {$k} WHERE ".$whereQuery, true);

				foreach($tmp as $i => $row) {
					foreach($row as $rk => $rv) {
						if ($rk === 'id') continue;

						$row[$rk] = _StrToLower($rv);
					}

					$expired[$k][$i] = $row;
				}
			}

			WorkProgress(true);

			if (!$quietFlag) Label("deleting data");

			$db->modifyingQuery("DELETE FROM sitesearch");

			if ($dbtype == 'pgsql') {
				if (!$quietFlag) Label("vacuum");
				$db->Query("VACUUM sitesearch", true);
				$db->Query("VACUUM ANALYZE sitesearch", true);
			}

		} else {
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

			if (!$quietFlag) Label("deleting data");

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
					$db->ModifyingQuery("DELETE FROM sitesearch WHERE did IN (".join(',', array_keys($delKeys)).")");
				}
			}

			if ($dbtype == 'pgsql') {
				if (!$quietFlag) Label("vacuum");
				$db->Query("VACUUM sitesearch", true);
				$db->Query("VACUUM ANALYZE sitesearch", true);
			}

			foreach ($search->config as $k => $v) {
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
					$q = "SELECT id, ".join(", ", array_keys($v))." FROM {$k} WHERE ".$whereQuery;
					$q = $q." AND id IN (".join(',', array_keys($selKeys[$k])).")";
					$tmp = array_merge($tmp, $db->query($q, true));
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
		}

		$totalr = 0;
		$tmp = array();
		static $dt; //Workaround for PHP 4.4.x bug, memory corruption !

		foreach ($expired as $corrupted_dt => $v) {
			$dt = $corrupted_dt;
			WorkProgress();
			$totalc = count($v);

			if (!$quietFlag) Label("idx: {$dt} ({$totalc})");

			foreach ($v as $k => $row) {
	//				if (!$quietFlag) echo --$totalc, " / ", $totalr;
				++$totalr;
				$totalWeights = array();

				foreach ($search->config[$dt] as $field => $weight) {
					preg_match_all("/[\xa3\xc0-\xdfa-z\d]+/s", $search->strip($row[$field]), $m);

					foreach($m[0] as $word) {
						$stem = $search->getStemByWord(_yo2ye($word));

						if (is_null($stem)) continue;

						if (!isset($totalWeights[$stem])) $totalWeights[$stem] = 0;

						$totalWeights[$stem] += $weight;
					}
				}

				foreach ($totalWeights as $stem => $weight) {
					$tmp[] = array($stem, $row['id'], $dt, $weight);
				}
	//				if (!$quietFlag) echo "\r\x1b[K";
			}
		}
		WorkProgress(true);
		$search->finish();

		$totalt = count($tmp);
		if ($totalt > 0) {
			if (!$quietFlag) Label("copy rec (". count($tmp).")");

			if ($dbtype == 'pgsql') {
				if($db->ModifyingCopy('sitesearch', array('stem', 'did', 'dtype', 'weightsum'), $tmp) !== true){
				    if(!$quietFlag) Label('Error while saving index !');
				}
												    

			} elseif ($dbtype == 'mssql') {
				$k = 0;

				foreach ($tmp as $v) {
					WorkProgress();
					$x = $db->Query("INSERT INTO sitesearch (stem, did, dtype, weightsum) VALUES (".join(",", array_map($makequote, $v)).")", false);
	//					if (!$quietFlag && ($k%1000 == 0)) echo date('r'), "\t\t\t(", $k++,")\n";
				}
				WorkProgress(true);
			}
		}

		if ($dbtype == 'pgsql') {
			if (!$quietFlag) Label("vacuum");

			$db->Query("VACUUM sitesearch", true);
			$db->Query("VACUUM ANALYZE sitesearch", true);
		}

		$db->ClearCache();

		if (!$quietFlag) Label("end");

		return array($totalr, $totalt);
	}

}

?>
