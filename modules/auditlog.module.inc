<?
//sql view required:
//CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts, 'DD.MM.YYYY') as date, "level", "type", "source", "user", "ip", "description" FROM log_audit;


/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends PXAbstractModule {
	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$layout->setSimpleInnerLayout(array('100%'), array('100%'));

		$this->app     =& $app;
		$this->db      =& $db;
		$this->request =& $request;
		$this->layout  =& $layout;

		$this->html('<H2>Журнал аудита</H2>');

		$this->page    = $this->request->GetVar('page', 1);
		$this->onPage  = $this->app->GetProperty('CHILDREN_ON_PAGE', 15);


		$this->table();
		$this->pager();

		//$this->filterForm();
	}

	function filterForm() {
		$color = TABLECOLOR1;
		$html = 
<<<HTML
	<H2>Filter</H2>

	<style type="text/css">
		form {
			padding: 1em;
			background: $color;
			overflow: hidden;
		}

		label, select, form input {
			display: block;
			width: 7%;
			float: left;
			margin: 1em;
		}

		label {
			text-align: right;
		}

		select, form input {
			width: 20%;
		}

		table {
			clear: left;
		}
	</style>

	<form method="get" class="autoheight">
		<input type="hidden" name="area" value="auditlog">

		<label for="level">Уровень</label>
		<select name="level" id="level">
			<option value="0">ALL</option>
			<option value="1">DEBUG</option>
			<option value="2">INFO</option>
			<option value="3">WARN</option>
			<option value="4">ERROR</option>
			<option value="5">FATAL</option>
			<option value="6">NONE</option>
		</select>

		<label for="type">Тип</label>
		<select name="type" id="type">
			<option value="">- Any -</option>
		</select>

		<select name="sourcetype">
			<option value="">- Any -</option>
		</select>
		<input type="text" name="sourceid">
		<input type="text" name="login">
		<input type="text" name="ip">

		<input type="submit">
	</form>
HTML;
		$this->html($html);
	}

	function filterSelect($name, $values) {
		$selected = $this->request->getVar($name);

		$_ = '<select name="'.$name.'">';
		
	}

	function filter() {
		$filter = array();

		foreach(array('date', 'level', 'type', 'source', 'login', 'ip') as $field) {
			$var = $this->request->getVar($field,   null);

			if(is_null($var)) {
				continue;
			}

			$this->layout->setGetVarToSave($field, $var);

			$filter[] = "\"{$field}\" = '{$var}'";
		}

		if (sizeof($filter)) {
			return 'WHERE '.implode(' AND ', $filter);
		}

		return '';
	}

	function getData() {
		$offset  = ($this->page-1) * $this->onPage;

		$sql  = 'SELECT "ts", "date", "level", "type", "source", "user", "ip", description FROM log_audit_view ';
		$sql .= $this->filter();
		$sql .= 'ORDER BY id DESC';

		return $this->db->query($sql, false, array($this->onPage, $offset));
	}

	function error($level) {
		if (isset($this->app->directory['error-type']->values[$level]['title'])) {
			return $this->app->directory['error-type']->values[$level]['title'];
		}

		return $level;
	}

	function source($source) {
		if (!strpos ($source, '/')) {
			return $source;
		}

		list ($_type, $_id) = explode('/', $source);
		return  '<a title="Редактировать объект" href="javascript:EditContent(\''.$_type.'\', \''.$_id.'\')">'.$source.'</a>';
	}

	function table() {
		$html = '';

		foreach ($this->getData() as $row) {
			$row['level']  = $this->error($row['level']); 
			$row['source'] = $this->source($row['source']); 
			extract($row);

			$html .= 
<<<HTML
	<tr class="filemanager">
		<td>$ts</td>
		<td>$level</td>
		<td>$type</td>
		<td>$source</td>
		<td>$user</td>
		<td>$ip</td>
		<td>$description</td>
	</tr>
HTML;
		}


		$html = 
<<<HTML
	<table class="objects">
		<colgroup>
			<col width="15%">
			<col width="10%">
			<col width="10%">
			<col width="15%">
			<col width="10%">
			<col width="10%">
			<col>
		</colgroup>

		<tr>
			<th>Дата/Время</th>
			<th>Уровень</th>
			<th>Тип</th>
			<th>Тип/Объект</th>
			<th>Логин</th>
			<th>IP адрес</th>
			<th>Сообщение</th>
		</tr>

		$html
	</table>
HTML;

		$this->html($html);
	}

	function pager() {
		$where = $this->filter();
		$count = $this->db->query('SELECT COUNT(*) FROM log_audit_view '.$where, false);
		$count = (int)$count[0]['count'];

		$page = $this->page;

		//постраничный вывод:
		$pageCount = ceil($count/$this->onPage)+1;
		$start = (ceil($page/10)-1)*10+1;
		$max = ($start+10 <= $pageCount) ? $start+10 : $pageCount;

		if ($page > 10) {
			$prev = (ceil($start/10)-1)*10 - 9;
		}

		$last = $pageCount - $start - 10;

		if ($last > 0) {
			$next = (ceil($start/10)-1)*10 + 11;
		}

		$html ='<div class="perpage">';

		if (isset($prev)) {
			$html .= '<a href="' .$this->_bh($prev). '">';
			$html .= '<img src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
			$html .= '</a>';
		}

		for ($i=$start; $i<$max; $i++) {
			$html .= '<a';
			if ($i == $page)
				$html .= ' class="selected"';

			$html .= ' href="'.$this->_bh($i).'">'.$i.'</a>';
		}

		if (isset($next)) {
			$html .= '<a href="' .$this->_bh($next). '">';
			$html .= '<img src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
			$html .= '</a>';
		}
		$html .='</div>';

		$this->html($html);
	}

	function html($html) {
		$this->layout->append('INNER.0.0', $html);
	}

	//_buildHref
	function _bh($val) {
		$html = '?page='.$val;

		foreach ($this->layout->getData as $k=>$v) {
			if (!empty($v)) {
				$html .= '&'.$k.'='.urlencode($v);
			}
		}

		return $html;
	}

}
?>
