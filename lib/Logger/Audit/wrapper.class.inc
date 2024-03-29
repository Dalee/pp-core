<?php

require_once PPLIBPATH . 'Logger/Audit/table.class.inc';
require_once PPLIBPATH . 'Logger/Audit/viewer.class.inc';

class PXAdminAuditWrapper
{
    public $app, $request, $db, $layout;
    public $minChildrenOnPage = 20;
    public $maxChildrenOnPage = 100;

    public $cols2title = [
        "ts" => "Дата/Время",
        "level" => "Уровень",
        "type" => "Тип сообщения",
        "source" => "Тип данных/объект",
        "user" => "Логин",
        "ip" => "IP адрес",
        "description" => "Сообщение",
        "diff" => "Что изменилось",
    ];

    public $frame = "INNER.0.0";
    public $caption = 'none';
    public $filters = [];

    public $hasFilterFrom = true;

    public function __construct()
    {
        PXRegistry::assignToObject($this);

        $this->page = $this->request->getVar('_page', 1);

        $onPage = $this->request->getVar('_per_page', $this->minChildrenOnPage);
        $this->onPage = $onPage > $this->maxChildrenOnPage ? $this->maxChildrenOnPage : $onPage;
    }

    public function init()
    {
        $this->viewer = new PXAuditViewer($this->filter(), $this->get_cols());

        $this->count = $this->viewer->getCount();
        $this->data = $this->viewer->getData($this->page, $this->onPage, $this->count);

        $this->caption = 'Журнал аудита, ' . $this->count . ' ' . NumericEndingsRussian($this->count, 'запись', 'записи', 'записей');
    }

    public function init_and_render()
    {
        $this->init();
        $this->render();
    }

    public function render()
    {
        $this->table();
        $this->pager();

        if ($this->hasFilterFrom) {
            $this->filterForm();
        }
    }

    public function table()
    {
        $table = new PXAdminAuditTable($this->cols2title);
        $table->setDict('level', $this->viewer->getErrorLevels());
        $table->setData($this->data);
        $table->setCaption($this->caption);
        $this->html($table->html());
    }

    public function pager()
    {
        $param = $this->request->getAllGetData();

        if (isset($param['date']) && isset($param['date']['year'])) {
            $param['ts'] = getFromArray($param['date'], 'day') . '-' . getFromArray($param['date'], 'month') . '-' . getFromArray($param['date'], 'year') . ' 00:00:00';
        }
        unset($param['date']);

        $pager = new PXAdminPager($this->page, $this->onPage, $this->count, '', $param);
        $this->html($pager->html());
    }

    public function filterForm()
    {
        $color = TABLECOLOR1;
        $borderColor = TABLECOLOR2;

        $types = getColFromtable($this->db->query('SELECT DISTINCT type FROM (SELECT * FROM log_audit ORDER BY id DESC LIMIT 10000) as t'), 'type');
        $levels = $this->viewer->getErrorLevels();

        $levelHTML = '';
        $typeHTML = '';
        $formatHTML = '';

        foreach ($levels as $k => $v) {
            $selected = ($k == $this->request->getVar('level')) ? ' selected' : '';
            $levelHTML .= '<option value="' . $k . '"' . $selected . '>' . ($v ?: '- Any -') . '</option>';
        }

        foreach ($types as $v) {
            $selected = ($v == $this->request->getVar('type')) ? ' selected' : '';
            $typeHTML .= '<option value="' . $v . '"' . $selected . '>' . $v . '</option>';
        }

        foreach ($this->app->types as $k => $v) {
            $selected = ($k == $this->request->getVar('format')) ? ' selected' : '';
            $formatHTML .= '<option value="' . $k . '"' . $selected . '>' . $v->title . '</option>';
        }


        $login = quot($this->request->getVar("user"));
        $description = quot($this->request->getVar("description"));
        $ip = quot($this->request->getVar("ip"));
        $id = quot($this->request->getVar("id"));

        $ts = $this->request->getVar('ts') ?: null;
        $ds = $this->request->getVar('date');
        if (isset($ds) && !in_array(null, $ds)) {
            $ts = getFromArray($ds, 'day') . '-' . getFromArray($ds, 'month') . '-' . getFromArray($ds, 'year') . ' 00:00:00';
        }

        $dateHTML = NLAbstractHTMLForm::BuildDateTime("date", $ts, false, true, true);

        $html =
            <<<HTML
	<H2>Filter</H2>

	<style type="text/css">
		.content form {
			padding: 0;
		}

		.content label, .content select, .content form input {
			display: block;
			width: 12%;
			float: left;
			margin: 1em;
		}

		.content label {
			text-align: right;
		}

		.content select, .content form input, .content form span.datetime {
			width: 30%;
			display: block;
		}

		.content form input[type='hidden'] {
			display: none;
		}

		#label_format, #label_description, #label_user {
			clear: left;
		}

		.content table {
			clear: left;
		}

		.content form .datetime {
			float: left;
			margin: 1em;
		}

		.content form .datetime input {
			margin: 0;
		}

		.content form input[type='submit'] {
			clear: left;
			float: right;
		}

		.content legend {
			color: #000000;
			font-weight: bold;
			font-size: 1.2em;
			padding: 0 0.5em;
		}

		.content fieldset {
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
			<select name="format" id="format">
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

    ##UTILS##

    public function get_cols()
    {
        return [...['id'], ...array_keys($this->cols2title)];
    }

    public function html($html)
    {
        $this->layout->append($this->frame, $html);
    }

    public function filter()
    {
        $filters = [];

        $cols = array_combine($cols = $this->get_cols(), array_pad([], is_countable($cols) ? count($cols) : 0, NULL));
        $cols = array_merge($cols, ["date" => null, "format" => null]);
        $in = array_merge($cols, $this->filters);

        foreach ($in as $field => $value) {
            $var = $this->request->getVar($field, $value);

            if (is_null($var)) {
                continue;
            }

            $this->layout->setGetVarToSave($field, $var);
            $filters[$field] = $var;
        }

        return $filters;
    }

    public function source($source)
    {
        if (!strpos((string) $source, '/')) {
            return $source;
        }

        [$_type, $_id] = explode('/', (string) $source);
        return '<a title="Редактировать объект" href="javascript:EditContent(\'' . $_type . '\', \'' . $_id . '\')">' . $source . '</a>';
    }
}

?>
