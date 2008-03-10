<?
//sql view required:
//CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts, 'DD.MM.YYYY') as date, "level", "type", "source", "user", "ip", "description" FROM log_audit;


/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends PXAbstractModule {

	var $settings;
	var $area;

	function PXModuleAuditLog($area, $settings) {
		$this->settings = $settings;
		$this->area     = $area;
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {

		$layout->setSimpleInnerLayout(array('100%'), array('100%'));

		$html = "<H2>Журнал аудита</H2>";

		$page    = $request->GetVar('page', 1);
		$onPage  = $app->GetProperty('CHILDREN_ON_PAGE', 15);
		$offset  = ($page-1) * $onPage;

		//filter
		$filter['date']	  = $request->getVar("date", null);
		$filter['level']  = $request->getVar("level", null);
		$filter['type']   = $request->getVar("type", null);
		$filter['source'] = $request->getVar("source", null);
		$filter['login']  = $request->getVar("login", null);
		$filter['ip']     = $request->getVar("ip", null);
		$filter     = array_filter($filter, array($this, "_nullFilter"));
		$filterSize = sizeof($filter);

		$sql  = "SELECT \"ts\", \"date\", \"level\", \"type\", \"source\", \"user\", \"ip\", description FROM log_audit_view ";
		$where = "";
		if ($filterSize >= 1) {
			$i = 1;
			$where .= "WHERE ";
			foreach ($filter as $k=>$v) {
				$layout->setGetVarToSave($k, $v);
				$where .= "\"{$k}\" = '{$v}' ";
				if ($i++ < $filterSize) {
					$where .= "AND ";
				}
			}
			$sql .= $where;
		}

		$sql .= "ORDER BY id DESC";
		$data = $db->query($sql, false, array($onPage, $offset));

		$count = $db->query('SELECT COUNT(*) FROM log_audit_view '.$where, false);
		$count = (int)$count[0]['count'];

		$html .= '<table class="objects">';
		$html .= '<colgroup><col width="150"><col width="80"><col width="80"><col width="100"><col width="80"><col width="100"></colgroup>';
		$html .= '<tr><th>Дата/Время</th><th>Уровень</th><th>Тип</th><th>Тип/Объект</th><th>Логин</th><th>IP адрес</th><th>Сообщение</th></tr>';

		$errorTypes = null;
		if (isset($app->directory['error-type'])) {
			$errorTypes = $app->directory['error-type']->values;
		}

		foreach ($data as $row) {
			$html .= '<tr class="filemanager">';
			$html .= '<td>'.$row['ts'].'</td>';
			if (!empty($errorTypes)) {
				$html .= '<td>'.$errorTypes[$row['level']]['title'].'</td>';
			} else {
				$html .= '<td>'.$row['level'].'</td>';
			}
			$html .= '<td>'.$row['type'].'</td>';
			if (strpos ($row['source'], '/')) {
				list ($_type, $_id) = explode('/', $row['source']);
				$html .= "<td><a title='Редактировать объект' href=\"javascript:EditContent('{$_type}', '{$_id}')\">{$row['source']}</a></td>";
			} else {
				$html .= '<td>'.$row['source'].'</td>';
			}
			$html .= '<td>'.$row['user'].'</td>';
			$html .= '<td>'.$row['ip'].'</td>';
			$html .= '<td>'.$row['description'].'</td>';
			$html .= '</tr>';
		}
		$html .='</table>';

		//постраничный вывод:
		$pageCount = ceil($count/$onPage)+1;
		$start = (ceil($page/10)-1)*10+1;
		$max = ($start+10 <= $pageCount) ? $start+10 : $pageCount;
		if ($page > 10) {
			$prev = (ceil($start/10)-1)*10 - 9;
		}
		$last = $pageCount - $start - 10;
		if ($last > 0) {
			$next = (ceil($start/10)-1)*10 + 11;
		}
		$html .='<div class="perpage">';
		if (isset($prev)) {
			$html .= '<a href="' .$this->_bh($layout->getData, 'page', $prev). '">';
			$html .= '<img src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
			$html .= '</a>';
		}
		for ($i=$start; $i<$max; $i++) {
			$html .= '<a';
			if ($i == $page)
				$html .= ' class="selected"';

			$html .= ' href="'.$this->_bh($layout->getData, 'page', $i).'">'.$i.'</a>';
		}
		if (isset($next)) {
			$html .= '<a href="' .$this->_bh($layout->getData, 'page', $next). '">';
			$html .= '<img src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
			$html .= '</a>';
		}
		$html .='</div>';
		$layout->append('INNER.0.0', $html);
	}

	//_buildHref
	function _bh(&$arr, $name, $val) {
		$html = '?'.$name.'='.$val;
		foreach ($arr as $k=>$v) {
			if (!empty($v))
				$html .= '&'.$k.'='.urlencode($v);
		}
		return $html;
	}

	/**
	 * callback ф-ция для array_filter
	 */
	function _nullFilter($var) {
		return ($var !== null) ? true : false;
	}


}//~
?>
