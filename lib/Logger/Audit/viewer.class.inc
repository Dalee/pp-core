<?php
class PXAuditViewer {
	function PXAuditViewer($filters, $cols) {
		$this->cols = $cols;
		$this->filters = $filters;
		$this->db = PXRegistry::getDb();
	}

	function filterToSQL($filters) {
		foreach($filters as $field=>$var) {
			switch($field) {
				case 'description':
					$filters[$field] = $this->db->mapFields($field) . $this->db->LIKE($var);
					break;

				case 'ip':
					$filters[$field] = $this->db->mapFields($field) . "<<=" .  $this->db->mapData($var);
					break;

				case 'date':
				case 'ts':
					if(is_array($var) && checkdate((int)getFromArray($var, 'month'), (int)getFromArray($var, 'day'), (int)getFromArray($var, 'year'))) {
						$var = sprintf('%02d-%02d-%04d 00:00:00', getFromArray($var, 'day'), getFromArray($var, 'month'), getFromArray($var, 'year'));
					}

					if(!is_array($var)) {
						$filters[$field] = "date_trunc('day', ts) = " . $this->db->mapData($var);
					} else {
						unset($filters[$field]);
					}

					break;

				case 'format':
					$filters[$field] = $this->db->mapFields('source') . $this->db->LIKE($var.'/', P_RIGHT);
					break;

				case 'id':
					$filters[$field] = "split_part(source, '/', 2) = '" . $this->db->escapeString($var) ."'";
					break;

				case 'level':
				case 'type':
				case 'source':
				case 'user':
					$filters[$field] = $this->db->mapFields($field) . "=" .  $this->db->mapData($var);
					break;

				default:
					unset($filters[$field]);
					break;
			}
		}

		if (!sizeof($filters)) {
			return '';
		}

		return ' WHERE '.implode(' AND ', $filters);
	}

	function getCount() {
		$cond  = sizeof($this->filters) > 0;
		$count = $this->db->query('SELECT '.(!$cond ? 'max' : 'count').'(id) AS count FROM log_audit ' . ($cond ? $this->filterToSQL($this->filters) : ''));

		return  $count[0]['count'];
	}

	function getData($page, $onPage, $count) {
		$offset  = ($page-1) * $onPage;
		$limit_pair = ($count > $onPage) ? array($onPage, $offset) : NULL;

		$cols = array_map(array($this->db, 'mapFields'), $this->cols);
		$selectSQL = "SELECT " . implode(", ", $cols) . " FROM log_audit_view " . $this->filterToSQL($this->filters) ."ORDER BY id DESC";
		
		return $this->db->query($selectSQL, false, $limit_pair);
	}

	function getErrorLevels() {
		$levels = array();

		$app = PXRegistry::getApp();

		if(isset($app->directory['error-type']->values)) {
			$levels = GetColFromTableWithIndexs($app->directory['error-type']->values, 'title');
			$levels[null] = '- Any -';
		}

		return $levels;
	}
}
?>