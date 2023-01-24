<?php
/**
 * PXAdminTable - конструктор HTML таблиц для ProximaPortal.
 *
 * @package ProximaPortal
 * @subpackage HTML
 */

define("MASK_FIRST_PAGE", 1);
define("MASK_LAST_PAGE",  2);
define("MASK_NO_MOVE",    3);

use PP\Lib\Html\Layout\LayoutAbstract;

class PXAdminTable extends PXAdminComplexWidget
{

	public $getData;

	public $titleKey;

	public $parentPathnames;
	public $tree;

	public $controls;                  //custom controls array
	public $showDefaultControls;       //where to show (or not to show) default main and edit controls : ENUM { 'before' , 'after', false }

	public $position;
	public $caption;

	public $selectedRow;
	public $showChildObjects;
	public $childLink;

	protected $tableId;

	final public const BEFORE_CONTENT_ALWAYS = 0;
	final public const BEFORE_CONTENT = 1;
	final public const AFTER_CONTENT = 2;

	/**
	 * Конструктор класса PXAdminTable.
	 *
	 * @param Array $data - данные
	 * @param PXTypeDescription $dtype - тип данных
	 * @param Array $getData - массив с GET переменными которые надо добавлять в ссылки таблицы
	 * @param String $titleKey - тип поля в datatype по которому будет определяться 'title' таблицы
	 */
	public function __construct(public $data, public $dtype, $getData, $titleKey = NULL)
	{
		$this->getData = !empty($getData) ? $getData : [];

		$this->currentOrder = $getData[$this->dtype->id . '_order'] ?? $this->dtype->order;

		$this->titleKey = $titleKey ?: 'title';
		$this->controls = [];
		$this->showDefaultControls = 'before';

		$this->selectedRow = NULL;
		$this->selectedType = NULL;

		$this->blocks = [ //set up available subwidgets areas
			self::BEFORE_CONTENT_ALWAYS => [],
			self::BEFORE_CONTENT => [],
			self::AFTER_CONTENT => [],
		];

		$this->position = MASK_NO_MOVE; //перемещение не возможно
		$this->caption = '';
	}

	public function loadDropdownValues()
	{
		PXRegistry::getDb()->loadDropdownValuesForListedFields($this->dtype, $this->data);
	}

	public function showDefaultControls($show = 'before')
	{
		$this->showDefaultControls = $show;
	}

	public function setParentPathNames(array $pathnames)
	{
		$this->parentPathnames = $pathnames;
	}

	public function setShowChildObjects($show = false)
	{
		$this->showChildObjects = $show;
	}

	/**
	 * Устанавливает указатель и подсветку на строку с выбранным id.
	 * @param Integer $id - id строки.
	 */
	public function setSelected($id, $type = NULL)
	{
		$this->selectedRow = $id;
		$this->selectedType = $type;
	}

	/**
	 * Устанавливает значение переменной для ссылки на дочерние объекты
	 * @param String $link - строка: 'sid' или 'cid'
	 */
	public function setChildLink($link)
	{
		$this->childLink = $link;
	}

	/**
	 * Возвращает таблицу.
	 * @return String - html table
	 */
	public function html()
	{
		$this->decorate();

		$html = '<div class="table_wrapper">';

		$html .= $this->htmlCaption();

		$html .= join($this->blocks[self::BEFORE_CONTENT_ALWAYS]);

		$hasInnerBlocks = false;

		foreach ($this->dtype->fields as $field) {
			if (is_a($field->storageType, 'PXStorageTypeBlockcontent')) {
				$hasInnerBlocks = true;
				break;
			}
		}

		if (is_countable($this->data) ? count($this->data) : 0) {
			$html .= join($this->blocks[self::BEFORE_CONTENT]);

			$html .= '<table class="objects">';


			//<!-- table header
			$html .= '<thead><tr>';
			$_icons = 2 + (is_countable($this->controls) ? count($this->controls) : 0) + ((($hasInnerBlocks || $this->dtype->childs) && $this->showChildObjects) ? 1 : 0);

			$html .= '<th class="c' . (($_icons > 2) ? $_icons : '') . '">';
			$html .= '<a href="' . $this->_buildHref($this->dtype->id . '_order', $this->dtype->order) . '" title="Порядок сортировки по умолчанию"><img src="i/icon/order.gif" width="7" height="8" alt=""></a>';
			$html .= '</th>';

			foreach ($this->dtype->fields as $field) {
				if ($field->listed) {
					$cellWidth = $field->displayType->getCellWidth($field);
					$html .= '<th' . ($field->name === $this->titleKey ? ' class="maincol"' : ' class="c-' . strtolower((string) $field->displayType->name) . '"') . ($cellWidth ? 'style="width: ' . $cellWidth . 'px"' : '') . '>';

					$param = [
						'order' => $this->currentOrder,
						'dtype' => $this->dtype->id,
						'table' => $this,
					];

					$html .= $field->storageType->objectsSortOrder($field, NULL, $param);

					$html .= '</th>';
				}
			}
			$html .= '</tr></thead>';
			//-->

			$rowPos = 0;
			foreach ($this->data as $rowKey => $row) {
				$up = $this->isFirstOnPage($rowPos) || !is_numeric(mb_strpos((string) $this->dtype->order, 'sys_order')) ? 0 : 1;
				$down = $this->isLastOnPage($rowPos) || !is_numeric(mb_strpos((string) $this->dtype->order, 'sys_order')) ? 0 : 1;

				if (isset($row[$this->titleKey])) {
					$menuTitle = (mb_strlen((string) $row[$this->titleKey]) > 10) ? trim(mb_substr((string) $row[$this->titleKey], 0, 10)) . '&hellip;' : $row[$this->titleKey];
					$menuTitle = str_replace('"', '', str_replace("'", '', (string) $menuTitle));
					$menuTitle = str_replace("\n", '', str_replace("\r", '', $menuTitle));
				} else {
					$menuTitle = '';
				}

				$alias = $this->_buildShowOnSiteLink($row);

				if (in_array($this->showDefaultControls, ['before', 'after'])) {
					$meths = ['before' => 'insertControl', 'after' => 'appendControl'];

					$t1 = '<a href="#"><img src="i/icon/menu.gif" ';
					$t1 .= 'onclick="{{context}}" oncontextmenu="{{context}}" alt=""></a>';

					$t2 = '<a oncontextmenu="{{context}}" href="javascript:EditContent(\'{{format}}\',\'{{id}}\')">';
					$t2 .= '<img src="i/icon/edit.gif" alt=""></A>';

					$this->{$meths[$this->showDefaultControls]}('edit', $t2, 'edit');
					$this->{$meths[$this->showDefaultControls]}('main', $t1, 'edit');
				}

				//templateVars
				$tVars = [
					'id' => $row['id'],                 // ID объекта
					'status' => (int)$row['status'],        // Статус строки, disabled/enabled
					'format' => $this->dtype->id,           // format/data type
					'title' => $menuTitle,                 // заголовок для context меню
					'alias' => $alias,                     //
					'level' => 1,                          // уровень вложенности обычно = 1
					'up' => $up,                        // можно ли вверх ?
					'down' => $down];                     // а вниз ?

				$html .= '<tr oncontextmenu="' . $this->_buildContext('edit', $tVars) . '" ';

				if ($this->tableId) {
					$html .= ' id="' . $this->tableId . '-' . $row['id'] . '"';
				}

				$html .= ' class="';

				if ($row['status'] == false) {
					$html .= ' hide';
				}

				if ($row['id'] == $this->selectedRow && $this->dtype->id == $this->selectedType) {
					$html .= ' selected';
				}

				$html .= '">';

				//TD ==>

				$html .= '<td class="c">';
				$html .= $this->_buildControls($tVars);

				if (($hasInnerBlocks || $this->dtype->childs) && $this->showChildObjects && isset($this->childLink)) {
					$html .= '<a href="' . $this->_BuildHref($this->childLink, $row['id'], 'ctype', $this->dtype->id) . '">';
					$html .= '<img src="i/icon/childs.gif" alt=""></a>';
				}

				$html .= '</td>';

				foreach ($this->dtype->fields as $field) {
					if ($field->listed) {
						$html .= "\n" . '<td>' . $this->_buildCell($row, $field) . '</td>';
					}
				}

				//<== TD

				$html .= '</tr>';
				$rowPos++;
			}//foreach

			$html .= '</table>';

			$html .= join($this->blocks[self::AFTER_CONTENT]);
		} else {

			$html .= '<div class="empty">&#8212;</DIV>';
		}//if

		$html .= '</div>';

		return $html;
	}

	/**
	 * Добавляем новый control элемент в конец $this->control массива.
	 * @see _addControl
	 */
	public function appendControl($name, $template, $context = NULL)
	{
		if (!isset($this->controls[$name])) {
			$this->controls[$name] = [];
		}

		$this->_addControl($name, $template, $context);
	}

	/**
	 * Добавляем новый control элемент в начало $this->control массива.
	 * @see _addControl
	 */
	public function insertControl($name, $template, $context = NULL)
	{
		if (!isset($this->controls[$name])) {
			$this->controls = [$name => NULL] + $this->controls;
		}

		$this->_addControl($name, $template, $context);
	}

	/**
	 * Добавляет элемент управления.
	 *
	 * @param String $name - название элемента управления
	 * @param String $template - шаблон эл-та управления
	 * @param String $context - название контекстного эл-та управления
	 * @see _buildControls
	 *
	 */
	public function _addControl($name, $template, $context)
	{
		$this->controls[$name]['template'] = $template;
		$this->controls[$name]['context'] = $context;
	}

	/**
	 * Формирует кнопочки управления по содержимому $this->controls.
	 *
	 * @param Array $vars - массив с переменными для подстановки
	 * @return String - html
	 */
	public function _buildControls($vars)
	{

		$html = "";

		foreach ($this->controls as $cname => $control) {
			$tmp = $control['template'];

			if (isset($control['context']) && $control['context'] !== NULL) {
				$vars['context'] = $this->_buildContext($control['context'], $vars);
			}
			foreach ($vars as $k => $v) {
				$tmp = str_replace('{{' . $k . '}}', $v, (string) $tmp);
			}
			$html .= $tmp;
		}//foreach

		return $html;
	}

	/**
	 * Строим вызов для javascript:Context() ф-ции.
	 *
	 * add : parent, items(format=>title)                                                 <br>
	 * file: title, isDir, isRead, isWrite, isDelete, isBinary, dir, href, side, isCopy   <br>
	 * edit: id, status, format, title, alias, level, up, down                            <br>
	 *
	 * Работа с контекстными меню типов 'file' и 'add' в данный момент не дописаны.       <br>
	 *
	 * @param String $cType - тип контекстного меню
	 * @param Array $cVars - переменные для контекстного меню
	 */
	public function _buildContext($cType, $cVars)
	{

		$context = '';
		switch ($cType) {
			//javascript:Context(event, 'add', parent, (format, title) )
			case 'add':
				$context = 'Context(event, \'add\', ' . $cVars['parent'];
				foreach ($cVars['items'] as $format => $title) {
					$context .= ', \'' . $format . '\', \'' . $title . '\' ';
				}
				$context .= '); return false;';

				break;

			//javascript:Context(event, 'edit', id, status, format, title, alias, level, up, down)
			case 'edit':
				$context = 'Context(event, \'edit\', ' . $cVars['id'] . ', ' . $cVars['status'] . ', \'' . $cVars['format'] . '\', ' .
					'\'' . $cVars['title'] . '\', \'' . $cVars['alias'] . '\',' . $cVars['level'] . ', ' .
					$cVars['up'] . ', ' . $cVars['down'] . '); return false;';
				break;

			//javascript:Context(event, 'file', title, isDir, isRead, isWrite, isDelete, isBinary, dir, href, side, isCopy)
			case 'file':
				$context = 'Context(event, \'file\', \'' . $cVars['title'] . '\',' . $cVars['isDir'] . ',' .
					$cVars['isRead'] . ',' . $cVars['isWrite'] . ',' . $cVars['isDelete'] . ',' .
					$cVars['isBinary'] . ',\'' . $cVars['dir'] . '\',\'' . $cVars['href'] . '\',' .
					'\'' . $cVars['side'] . '\',' . $cVars['isCopy'] . '); return false;';
				break;

		}//switch
		return $context;
	}

	/**
	 * Строит ячейку таблицы по заданному display type.
	 *
	 * @param Array $object - данные
	 * @param String $field - поле по которому проверяется display type
	 */
	public function _buildCell($object, $field)
	{
		return $field->displayType->buildCell($field, $object);
	}

	/**
	 * Строим ссылку, не забываем добавлять getData.
	 *
	 * @param String|Array $key - массив ('nameN'=>'valN') или имя переменной
	 * @param String|NULL $val - значение для String параметра $key
	 * @return String - строка с html кодом ссылки
	 */
	public function _buildHref(string|array $key, $val = NULL)
	{
		$args = func_get_args();
		$argsCount = func_num_args();

		$href = "?";

		for ($i = 0; $i < $argsCount; $i++) {
			if (is_array($args[$i])) {
				foreach ($args[$i] as $k => $v) {
					$this->_addToUrl($href, $k, $v);
				}
			} else {
				if (isset($args[$i + 1])) {
					$this->_addToUrl($href, $args[$i], $args[++$i]);
				}
			}
		}

		return $href;
	}

	public function _addToUrl(&$href, $k, $v)
	{
		$href = LayoutAbstract::buildHref($k, $v, $this->getData, $href);
	}

	public function setSiteTree($tree)
	{
		$this->tree = $tree;
	}

	public function _buildShowOnSiteLink($row)
	{
		if (!isset($row['parent']) || !isset($row['pathname']) || !mb_strlen((string) $row['pathname'])) {
			return '';
		}

		switch (true) {
			case isset($this->parentPathnames[$row['parent']]):
				$pathname = $this->parentPathnames[$row['parent']];

				if ($this->dtype->parent == 'struct') {
					return $pathname . $row['pathname'];
				} else {
					return $pathname . '?part=' . $row['pathname'];
				}

			case isset($this->tree) && isset($row['parent']) && $row['parent'] > 0:
				return $this->tree->getFullPathString($row['parent']) . $row['pathname'];

			default:
				return '';
		}
	}

	public function setPosition($position)
	{
		$this->position = $position;
	}

	public function isFirstOnPage($row)
	{
		return $row == 0 && ($this->position & MASK_FIRST_PAGE);
	}

	public function isLastOnPage($row)
	{
		return $row == sizeof($this->data) - 1 && ($this->position & MASK_LAST_PAGE);
	}

	public function setCaption($text)
	{
		$this->caption = $text;
	}

	public function htmlCaption()
	{
		if (!mb_strlen((string) $this->caption)) {
			return '';
		}

		$html = '';
		$html .= '<h2>' . $this->caption;

		if ($this->dtype->struct == 'tree') {
			$html .= '[<a href="' . $this->_buildHref($this->dtype->id . '_view', 'tree') . '">дерево</a>]';
		}

		$html .= '</h2>';
		return $html;
	}

	public function setTableId($id = null)
	{
		$this->tableId = $id ?: md5(microtime(1));
	}

}//class
