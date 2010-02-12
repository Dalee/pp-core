<?php
//sql view required:
//CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts, 'DD.MM.YYYY') as date, "level", "type", "source", "user", "ip", "description" FROM log_audit;


/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends PXModule {
	function adminIndex() {
		$this->layout->setOneColumn();

		$this->page    = $this->request->getVar('_page', 1);
		$this->onPage  = $this->app->getProperty('CHILDREN_ON_PAGE', 15);

		$this->viewer = new PXAuditViewer($this->filter());
		$count = $this->viewer->getCount();
		$data  = $this->viewer->getData($this->page, $this->onPage);

		$this->table($count, $data);
		$this->pager($count);

		$this->filterForm();
	}

	function filterForm() {
		$color = TABLECOLOR1;
		$borderColor = TABLECOLOR2;

		$types = getColFromtable($this->db->query('SELECT type FROM log_audit GROUP BY type'), 'type');
		$levels = $this->viewer->getErrorLevels();

		$levelHTML  = '';
		$typeHTML   = '';
		$formatHTML = '';

		foreach($levels as $k => $v) {
			$selected = ($k == $this->request->getVar('level')) ? ' selected' : '';
			$levelHTML .= '<option value="' . $k . '"'. $selected .'>'. ($v ? $v : '- Any -') . '</option>';
		}

		foreach($types as $v) {
			$selected = ($v == $this->request->getVar('type')) ? ' selected' : '';
			$typeHTML .= '<option value="' . $v . '"'. $selected .'>'. $v . '</option>';
		}

		foreach($this->app->types as $k=>$v) {
			$selected = ($k == $this->request->getVar('format')) ? ' selected' : '';
			$formatHTML .= '<option value="' . $k . '"'. $selected .'>'. $v->title . '</option>';
		}


		$login       = $this->request->getVar("user");
		$description = $this->request->getVar("description");
		$ip          = $this->request->getVar("ip");
		$id          = $this->request->getVar("id");

		$ts = $this->request->getVar('ts') ? $this->request->getVar('ts') : null;
		$ds = $this->request->getVar('date');
		if(isset($ds) && !in_array(null, $ds)) {
			$ts = getFromArray($ds, 'day'). '-' . getFromArray($ds, 'month') . '-' . getFromArray($ds, 'year') . ' 00:00:00';
		}

		$dateHTML =  NLAbstractHTMLForm::BuildDateTime("date", $ts, false, true, true);

		$html = 
<<<HTML
	<H2>Filter</H2>

	<style type="text/css">
		form {
			padding: 0;
		}

		label, select, form input {
			display: block;
			width: 12%;
			float: left;
			margin: 1em;
		}

		label {
			text-align: right;
		}

		select, form input, form span.datetime {
			width: 30%;
			display: block;
		}

		form input[type='hidden'] {
			display: none;
		}

		#label_format, #label_description, #label_user {
			clear: left;
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

		form input[type='submit'] {
			clear: left;
			float: right;
		}

		legend {
			color: #000000;
			font-weight: bold;
			font-size: 1.2em;
			padding: 0 0.5em;
		}

		fieldset {
			float: left;
			width: 48%;
			margin: 1em 0 1em 2%;
			border-color: $borderColor;
			background: $color;
			overflow: hidden;
		}

		fieldset#fieldset_object {
			margin-left: 0;
			margin-right: -1%;
		}

		fieldset#message_fieldset {
			float: none;
			width: auto;
			margin: 0;
		}
	</style>

	<form method="get" class="autoheight">
		<input type="hidden" name="area" value="auditlog">

		<fieldset id="message_fieldset">
			<legend>Сообщение</legend>

			<label for="level">Уровень</label>
			<select name="level" id="level">
				$levelHTML;
			</select>

			<label id="label_type" for="type">Тип</label>
			<select name="type" id="type">
				<option value="">- Any -</option>
				$typeHTML
			</select>

			<label id="label_description">Текст</label>
			<input type="text" name="description" value="$description">

			<label>Дата</label>
			$dateHTML
		</fieldset>

		<fieldset id="fieldset_object">
			<legend>Объект</legend>

			<label id="label_format" for="format">Тип данных</label>
			<select name="format" id="type">
				<option value="">- Any -</option>
				$formatHTML
			</select>

			<label id="label_id">ID</label>
			<input type="text" name="id" value="$id">
		</fieldset>

		<fieldset>
			<legend>Пользователь</legend>

			<label id="label_user">Логин</label>
			<input type="text" name="user" value="$login">

			<label id="label_ip">IP</label>
			<input type="text" name="ip" value="$ip">
		</fieldset>

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
		$filters = array();

		foreach(array('ts', 'date', 'level', 'type', 'user', 'description', 'ip', 'format', 'id') as $field) {
			$var = $this->request->getVar($field,   null);

			if(is_null($var)) {
				continue;
			}
			$this->layout->setGetVarToSave($field, $var);

			$filters[$field] = $var;
		}

		return $filters;
	}

	function source($source) {
		if (!strpos ($source, '/')) {
			return $source;
		}

		list ($_type, $_id) = explode('/', $source);
		return  '<a title="Редактировать объект" href="javascript:EditContent(\''.$_type.'\', \''.$_id.'\')">'.$source.'</a>';
	}

	function table($count, $data) {
		$table = new PXAdminAuditTable();
		$table->setDict('level', $this->viewer->getErrorLevels());
		$table->setData($data);
		$table->setCaption('Журнал аудита, '.$count.' '.NumericEndingsRussian($count, 'запись', 'записи', 'записей'));

		$this->html($table->html());
	}

	function pager($count) {
		$param = $this->request->getAllGetData();

		if(isset($param['date']) && isset($param['date']['year'])) {
			$param['ts'] = getFromArray($param['date'], 'day'). '-' . getFromArray($param['date'], 'month') . '-' . getFromArray($param['date'], 'year') . ' 00:00:00';
		}

		unset($param['date']);
		foreach($param as $k => $v) {
			$param[$k] = myconv('k', 'u', $param[$k]); 
		}

		$pager = new PxAdminPager($this->page, $this->onPage, $count, '',  $param);

		$this->html($pager->html());
	}

	function html($html) {
		$this->layout->append('INNER.0.0', $html);
	}
}


class PXAdminAuditTable extends PXAdminTableSimple {
	function PXAdminAuditTable() {
		$cols = array(
			"ts"          => "Дата/Время",
			"level"       => "Уровень",
			"type"        => "Тип сообщения",
			"source"      => "Тип данных/объект",
			"user"        => "Логин",
			"ip"          =>"IP адрес",
			"description" => "Сообщение"
		);

		parent::PXAdminTableSimple($cols);

		$this->app =& PXRegistry::getApp();
		$this->setDict('datatypes', $this->app->types);
	}

	function _getDatatypeTitle($format) {
		return isset($this->dict['datatypes'][$format]) ? $this->dict['datatypes'][$format]->title : $format;
	}

	function cell($row, $col) {
		if($col == 'source') {
			if (strpos($row[$col], '/')) {
				preg_match('|(\w+)/?(\d+)?|', $row[$col], $matches);

				if(sizeof($matches) == 3) {
					$row[$col] = $this->__link('popup.phtml?area=objects&format='.$matches[1].'&id='.$matches[2].'&action=main', $this->_getDatatypeTitle($matches[1]).'/'.$matches[2]);

				} elseif(sizeof($matches) == 2) {
					$row[$col] = $this->__link('?area=dicts&sid='.$matches[1], $this->_getDatatypeTitle($matches[1]));
				}

			} elseif(strlen($row[$col])) {
				$row[$col] = $this->__link('?area=dicts&sid='.$row[$col], $this->_getDatatypeTitle($row[$col]));
			}
		}

		if($col == 'user' && $row[$col] !== 'UNKNOWN') {
			$row[$col] = $this->__link('./?area=auditlog&user='.$row[$col], $row[$col]);
		}

		return parent::cell($row, $col);
	}

	function __link($url, $text) {
		return '<a href="'.$url.'" target="_blank">'.$text.'</a>';
	}
}

class PXAuditViewer {
	function PXAuditViewer($filters) {
		$this->filters = $filters;
		$this->db =& PXRegistry::getDb();
	}

	function filterToSQL($filters) {
		foreach($filters as $field=>$var) {
			switch($field) {
				case 'description':
				case 'ip':
					$filters[$field] = $this->db->mapFields($field) . $this->db->LIKE($var);
					break;

				case 'date':
				case 'ts':
					if(is_array($var) && !in_array(null, $var)) {
						$var = getFromArray($var, 'day'). '-' . getFromArray($var, 'month') . '-' . getFromArray($var, 'year') . ' 00:00:00';
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
					$filters[$field] = $this->db->mapFields('source') . $this->db->LIKE('/'.$var, P_LEFT);
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
		$count = $this->db->query('SELECT count(*) FROM log_audit_view'.$this->filterToSQL($this->filters));
		return $count[0]['count'];

	}

	function getData($page, $onPage) {
		$offset  = ($page-1) * $onPage;
		$selectSQL  = 'SELECT "ts", "date", "level", "type", "source", "user", "ip", "description" FROM log_audit_view'.$this->filterToSQL($this->filters).' ORDER BY id DESC';

		return $this->db->query($selectSQL, false, array($onPage, $offset));
	}

	function getErrorLevels() {
		$levels = array();

		$app =& PXRegistry::getApp();

		if(isset($app->directory['error-type']->values)) {
			$levels = GetColFromTableWithIndexs($app->directory['error-type']->values, 'title');
			$levels[null] = '- Any -';
		}

		return $levels;
	}

}
?>