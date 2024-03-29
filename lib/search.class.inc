<?php
function SortByRelevant($a, $b) {
	if (($r = NumericCmp($b['words'], $a['words'])) != 0) {
		return $r;
	} else {
		return NumericCmp($b['sum'], $a['sum']);
	}
}

function _yo2ye($s) {
	return str_replace('Ё', 'Е', str_replace('ё', 'е', (string) $s));
}

class PXSearch
{
	public $app;
	public $db;

	public $stopwords;
	public $config;

	public $word2stem;
	public $stem2id;

	public $dbah;

	public function __construct(public $withIndexField = true)
	{
		$this->app = PXRegistry::getApp();
		$this->db = PXRegistry::getDB();
		$this->searchCount = 0;

		$this->loadStopwords(
			'а без более бы был была были было быть в вам вас весь во вот все всего мочь можно
			всех всей вы где да даже для до его ее если есть еще же за здесь и из или им их к как который
			которую которая которых	ко когда кто ли либо мне может мы на надо наш наша наше не него нее нет ни них но ну о
			об однако он она они оно от очень по под при с со так также такой такая там те тем то
			того тоже той только том ты у уже хотя чего чей чем что чтобы чье чья эта эти это я
			через любой любая этого http www ru другой другая'
		);

		$this->loadConfig();
		$this->word2stem = [];
		$this->stem2id = [];

		$fname = CACHE_PATH . '/word2stem.cdb';
		$this->dbah = null;
		if (file_exists($fname)) {
			$this->dbah = dba_open($fname, 'r', 'cdb');
		}
	}

	public function doSearch($query, $parents = NULL, $allowTypes = NULL, $limit = NULL, $page = NULL)
	{
		$this->parents = $parents;

		$stemsArray = $this->_getStemsArray($query);
		if (!(is_countable($stemsArray) ? count($stemsArray) : 0)) {
			return [];
		}

		$sql = $this->_getStemsSQL($stemsArray, $allowTypes);
		$this->searchCount = $this->_searchCount($sql);

		if (!$this->searchCount) {
			return [];
		}

		[$dtypes, $tmpres] = $this->_loadStems($sql, $limit, $page);

		return $this->searchObjects($dtypes, $tmpres);
	}

	public function finish()
	{
		if (is_resource($this->dbah)) {
			dba_close($this->dbah);
		}
	}

	public function loadStopwords($s)
	{
		preg_match_all("/\w+/s" . REGEX_MOD, (string) $s, $m);
		$this->stopwords = [];
		foreach ($m[0] as $v) {
			$this->stopwords[$v] = true;
		}
	}

	public function loadConfig()
	{
		$this->config = [];
		foreach ($this->app->types as $k => $v) {
			if ($this->withIndexField && !isset($v->fields['index'])) {
				continue;
			}

			$this->config[$k] = [];

			foreach ($v->fields as $type) {
				if ($type->storageType->name != 'string') continue;
				if ($type->name == 'pathname') continue;
				if ($type->noindex) continue;

				$this->config[$k][$type->name] = ($type->name == 'title' ? 5 : 1);
			}

			if (count($this->config[$k]) == 0) {
				unset($this->config[$k]);
			}
		}
	}

	public function getStemByWord($word)
	{
		if (mb_strlen((string) $word) <= 2 || mb_strlen((string) $word) > 30) {
			return NULL;
		}

		if (isset($this->stopwords[$word])) {
			return NULL;
		}

		if (isset($this->word2stem[$word])) {
			return $this->word2stem[$word];
		}

		if (!is_resource($this->dbah)) {
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
	}

	public function getStemsByArray($a)
	{
		$result = [];
		foreach ($a as $v) {
			$s = $this->getStemByWord($v);

			if (!is_null($s)) {
				$result[] = $s;
			}
		}
		return $result;
	}

	public static function strip($s)
	{
		$s = preg_replace("/<SCRIPT.*?\\/SCRIPT>/is" . REGEX_MOD, '', (string) $s); //
		$s = preg_replace("/<.*?>/s" . REGEX_MOD, ' ', $s);
		$s = preg_replace("/&nbsp;/is" . REGEX_MOD, ' ', $s);
		$s = html_entity_decode($s, ENT_QUOTES, DEFAULT_CHARSET);
		$s = preg_replace("/&.*?;/s" . REGEX_MOD, ' ', $s);
		return $s;
	}

	public function escapeString($word)
	{
		return "'" . $this->db->escapeString($word) . "'";
	}

	private
	function _getStemsSQL($stemsArray, $allowTypes)
	{
		$where = [
			"stem IN (" . join(',', array_map($this->escapeString(...), $stemsArray)) . ")",
		];

		if (isset($allowTypes) && is_array($allowTypes)) {
			$allowTypes = array_flip($allowTypes);
			$where[] = "dtype IN (" . join(',', array_map($this->escapeString(...), $allowTypes)) . ")";
		}

		foreach (PXRegistry::getApp()->triggers->search as $t) {
			$t->getTrigger()->onBeforeSearchStems($this->config, $where);
		}

		$where = implode(' AND ', $where);
		$totalwords = is_countable($stemsArray) ? count($stemsArray) : 0;

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
	function _searchCount($sql)
	{
		$tmp = $this->db->Query("SELECT count(*) FROM (" . $sql . ") AS tmp");

		if (!isset($tmp[0]['count'])) {
			return 0;
		}

		return $tmp[0]['count'];
	}

	private
	function _getLimitPair($limit, $page)
	{
		if ($limit && $page) {
			return [
				$limit,
				$limit * ($page - 1),
			];
		} else {
			return null;
		}
	}

	private
	function _loadStems($sql, $limit, $page)
	{
		$tmp = $this->db->query($sql, false, $this->_getLimitPair($limit, $page));

		$tmpres = [];
		$dtypes = [];

		foreach ($tmp as $k => $v) {
			if (!isset($this->app->types[$v['dtype']])) {
				continue;
			}

			$key = $v['dtype'] . ' ' . $v['did'];
			$tmpres[$key] = [
				'did' => $v['did'],
				'dtype' => $v['dtype'],
				'sum' => $v['sum'],
				'words' => $v['words'],
			];

			$dtypes[$v['dtype']][$v['did']] = true;
		}

		return [$dtypes, $tmpres];
	}

	private
	function _getStemsArray($query)
	{
		$query = preg_replace("/&(#\d+|[a-z]+);/s" . REGEX_MOD, '', (string) $query);
		preg_match_all("/[а-яa-z]+/s" . REGEX_MOD, (string) _StrToLower(_yo2ye($query)), $tmp);
		return array_unique($this->getStemsByArray(array_unique($tmp[0])));
	}

	public function _withParents($type)
	{
		return isset($this->parents) && is_array($this->parents) && sizeof($this->parents) && isset($this->app->types[$type]->fields['parent']);
	}

	public function searchObjects($dtypes, $tmpres)
	{
		$result = [];

		if (!is_null($this->parents)) {
			$tmpParents = [];
			$tmpJoin = [];

			foreach ($this->parents as $k => $v) {
				foreach ($v as $i) {
					$tmpParents[$i] = $k;
					$tmpJoin[] = $i;
				}
			}
		}

		foreach ($dtypes as $k => $v) {
			if (!(is_countable($v) ? count($v) : 0)) {
				continue;
			}

			$where = [];
			$where[] = sprintf("id IN ('%s')", join("','", array_keys($v)));

			if ($this->withIndexField) {
				$where[] = $this->db->__mapUpdateData('index', '1');
			}

			$where[] = "status = '1'";

			if ($this->_withParents($k)) {
				$where[] = sprintf("parent IN ('%s')", join("','", array_values($tmpJoin)));
			}

			foreach (PXRegistry::getApp()->triggers->search as $t) {
				$t->getTrigger()->onBeforeSearchObjects($k, $where);
			}

			$tmp = $this->db->getObjectsByWhere($this->app->types[$k], true, $where);

			foreach ($tmp as $row) {
				$key = $k . ' ' . $row['id'];

				if (!isset($tmpres[$key])) {
					continue;
				}

				foreach (PXRegistry::getApp()->triggers->search as $t) {
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

	public function getSearchCount()
	{
		return $this->searchCount;
	}
}
