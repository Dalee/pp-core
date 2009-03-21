<?
//sql view required:
//CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts, 'DD.MM.YYYY') as date, "level", "type", "source", "user", "ip", "description" FROM log_audit;


/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends PXModule {
	function adminIndex() {
		$this->layout->setSimpleInnerLayout(array('100%'), array('100%'));

		$this->page    = $this->request->GetVar('_page', 1);
		$this->onPage  = $this->app->GetProperty('CHILDREN_ON_PAGE', 15);
		$this->count = 0;


		$this->table();

		$this->filterForm();
	}

	function filterForm() {
		$color = TABLECOLOR1;
		$sql = 'SELECT type FROM log_audit GROUP BY type';
		$tmp = getColFromtable($this->db->query($sql), 'type');
		$option = '';

		$level = array(
			'ALL'   => null,
			'DEBUG' => 1,
			'INFO'  => 2, 
			'WARN'  => 3,
			'ERROR' => 4,
			'FATAL' => 5,
			'NONE'  => 6
		);

		$oLevel = '';

		foreach($level as $k => $v) {
			$selected = ($k == $this->request->getVar('level')) ? ' selected' : '';

			$oLevel .= '<option value="' . $v . '"'. $selected .'>'. $k . '</option>';
		}

		foreach($tmp as $v) {
			$selected = ($k == $this->request->getVar('type')) ? ' selected' : '';
			$option .= '<option value="' . $v . '"'. $selected .'>'. $k . '</option>';
		}

		$login = $this->request->getVar("user");
		$description = $this->request->getVar("description");
		$ip = $this->request->getVar("ip");

		$ts = $this->request->getVar('ts') ? $this->request->getVar('ts') : null;
		$ds = $this->request->getVar('date');
		if(isset($ds) && !in_array(null, $ds)) {
			$ts = getFromArray($ds, 'day'). '-' . getFromArray($ds, 'month') . '-' . getFromArray($ds, 'year') . ' 00:00:00';
		}

		$date =  '<label>Дата</label>' . NLAbstractHTMLForm::BuildDateTime("date", $ts, false, true, true);

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
			clear: left;
		}

		select, form input {
			width: 20%;
		}

		table {
			clear: left;
		}
		form .datetime {
			float: left;
			margin: 1em;
		}
		form .datetime input {
			margin: 0;
		}
	</style>

	<form method="get" class="autoheight">
		<input type="hidden" name="area" value="auditlog">

		<label for="level">Уровень</label>
		<select name="level" id="level">
			$oLevel;
		</select>

		<label for="type">Тип</label>
		<select name="type" id="type">
			<option value="">- Any -</option>
			$option
		</select>
$date
		<label>Логин</label><input type="text" name="user" value="$login">
		<label>Сообщение</label><input type="text" name="description" value="$description">
		<label>IP</label><input type="text" name="ip" value="$ip">

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

		foreach(array('ts', 'date', 'level', 'type', 'source', 'user', 'description', 'ip') as $field) {
			$var = $this->request->getVar($field,   null);

			if(is_null($var)) {
				continue;
			}
			$this->layout->setGetVarToSave($field, $var);

			switch($field) {
				case 'description':
				case 'ip':
					$filter[] = $this->db->mapFields($field) . " ILIKE '%" . $var . "%'";
					break;

				case 'date':
				case 'ts':
					if(is_array($var) && !in_array(null, $var)) {
						$var = getFromArray($var, 'day'). '-' . getFromArray($var, 'month') . '-' . getFromArray($var, 'year') . ' 00:00:00';
					}

					if(!is_array($var)) {
						$filter[] = "date_trunc('day', ts) = " . $this->db->mapData($var);
					}
					break;	

				default:
					$filter[] = $this->db->mapFields($field) . "=" .  $this->db->mapData($var);
					break;
			}
		}

		if (sizeof($filter)) {
			return ' WHERE '.implode(' AND ', $filter);
		}

		return '';
	}

	function getData() {
		$offset  = ($this->page-1) * $this->onPage;

		$selectSQL  = 'SELECT "ts", "date", "level", "type", "source", "user", "ip", "description" FROM log_audit_view'.$this->filter().' ORDER BY id DESC';
		$countSQL   = 'SELECT count(*) FROM log_audit_view'.$this->filter();

		$data = $this->db->query($selectSQL, false, array($this->onPage, $offset));

		$count = $this->db->query($countSQL);
		$this->count = $count[0]['count'];

		return $data;
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
		$table = new PXAdminTableSimple(array("ts" => "Дата/Время", "level" => "Уровень", "type" => "Тип", "source" => "Тип/объект", "user" => "Логин", "ip" =>"IP адрес", "description" => "Сообщение"));
		$data = $this->getData();
		$table->setData($data);
		$table->setCaption('Журнал аудита, '.$this->count.' '.NumericEndingsRussian($this->count, 'запись', 'записи', 'записей'));

		$param = $this->request->GetAllGetData();

		if(isset($param['date']) && isset($param['date']['year'])) {
			$param['ts'] = getFromArray($param['date'], 'day'). '-' . getFromArray($param['date'], 'month') . '-' . getFromArray($param['date'], 'year') . ' 00:00:00';
		}
	
		unset($param['date']);
		foreach($param as $k => $v) {
			$param[$k] = iconv("koi8-r", "utf-8", $param[$k]); 
		}

		$pager = new PxAdminPager($this->page, $this->onPage, $this->count, '',  $param);
		$this->html($table->html() . $pager->html());
	}

	function html($html) {
		$this->layout->append('INNER.0.0', $html);
	}
}
?>
