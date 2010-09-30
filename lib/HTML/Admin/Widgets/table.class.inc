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

class PXAdminTable extends PXAdminComplexWidget {
	var $data;
	var $dtype;
	var $getData;

	var $titleKey;

	var $parentPathnames;
	var $tree;

	var $controls;                  //custom controls array
	var $showDefaultControls;       //where to show (or not to show) default main and edit controls : ENUM { 'before' , 'after', false }

	var $position;
	var $caption;

	var $selectedRow;
	var $showChildObjects;
	var $childLink;
	
	const BEFORE_CONTENT = 0;
	const AFTER_CONTENT  = 1;

	/**
	 * Конструктор класса PXAdminTable.
	 *
	 * @param Array $data - данные
	 * @param PXTypeDescription $dtype - тип данных
	 * @param Array $getData - массив с GET переменными которые надо добавлять в ссылки таблицы
	 * @param String $titleKey - тип поля в datatype по которому будет определяться 'title' таблицы
	 */
	function PXAdminTable($data, $dtype, $getData, $titleKey = NULL) {
		$this->data                = $data;
		$this->dtype               = $dtype;
		$this->getData             = !empty($getData) ? $getData : array();

		$this->currentOrder        = isset($getData[$this->dtype->id.'_order']) ? $getData[$this->dtype->id.'_order'] : $this->dtype->order;

		$this->titleKey            = ($titleKey) ? $titleKey : 'title';
		$this->controls            = array();
		$this->showDefaultControls = 'before';

		$this->selectedRow         = NULL;
		$this->selectedType        = NULL;

		$this->blocks              = array( //set up available subwidgets areas
			self::BEFORE_CONTENT => array(),
			self::AFTER_CONTENT  => array()
		);

		$this->position = MASK_NO_MOVE; //перемещение не возможно
		$this->caption = '';
	}

	function loadDropdownValues() {
		PXRegistry::getDb()->loadDropdownValuesForListedFields($this->dtype, $this->data);
	}

	function showDefaultControls($show = 'before') {
		$this->showDefaultControls = $show;
	}

	function setParentPathNames(array $pathnames) {
		$this->parentPathnames = $pathnames;
	}

	function setShowChildObjects($show=false) {
		$this->showChildObjects = $show;
	}

	/**
	 * Устанавливает указатель и подсветку на строку с выбранным id.
	 * @param Integer $id - id строки.
	 */
	function setSelected($id, $type=NULL) {
		$this->selectedRow  = $id;
		$this->selectedType = $type;
	}
	/**
	 * Устанавливает значение переменной для ссылки на дочерние объекты
	 * @param String $link - строка: 'sid' или 'cid'
	 */
	function setChildLink($link){
		$this->childLink = $link;
	}

	/**
	 * Возвращает таблицу.
	 * @return String - html table
	 */
	function html() {
		$this->decorate();

		$html = '<div class="table_wrapper">';

		$html .= $this->htmlCaption();
		if (count($this->data)) {
			$html .= join($this->blocks[self::BEFORE_CONTENT]);

			$html .= '<table class="objects">';


			//<!-- table header
			$html .= '<thead><tr>';
			$_icons = 2 + count($this->controls) + (($this->dtype->childs && $this->showChildObjects) ? 1 : 0);
			$html .= '<th class="c'.(($_icons > 2) ? $_icons : '').'"><a href="'.$this->_buildHref($this->dtype->id.'_order', $this->dtype->order).'" title="Порядок сортировки по умолчанию"><img src="i/icon/order.gif" width="7" height="8" alt=""></a></th>';

			foreach ($this->dtype->fields as $field) {
				if ($field->listed) {
					$html .= '<th'.($field->name === $this->titleKey ? ' class="maincol"' : ' class="c-'.strtolower($field->displayType->name).'"').'>';

					$param = array(
						'order' => $this->currentOrder,
						'dtype' => $this->dtype->id,
						'table' => $this
					);

					$html .= $field->storageType->objectsSortOrder($field, NULL, $param);

					$html .= '</th>';
				}
			}
			$html .= '</tr></thead>';
			//-->

			$rowPos = 0;
			foreach ($this->data as $rowKey=>$row){
				$up    = $this->isFirstOnPage($rowPos) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;
				$down  = $this->isLastOnPage($rowPos) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;

				if (isset($row[$this->titleKey])) {
					$menuTitle = (strlen($row[$this->titleKey]) > 10) ? trim(substr($row[$this->titleKey], 0, 10)).'&hellip;' : $row[$this->titleKey];
					$menuTitle = str_replace('"', '', str_replace("'", '', $menuTitle));
					$menuTitle = str_replace("\n", '', str_replace("\r", '', $menuTitle));
				} else {
					$menuTitle = '';
				}

				$alias = $this->_buildShowOnSiteLink($row);

				if (in_array($this->showDefaultControls, array('before', 'after'))) {
					$meths = array('before' => 'insertControl', 'after' => 'appendControl');
					
					$t1  = '<a href="#"><img src="i/icon/menu.gif" ';
					$t1 .= 'onclick="{{context}}" oncontextmenu="{{context}}" alt=""></a>';

					$t2  = '<a oncontextmenu="{{context}}" href="javascript:EditContent(\'{{format}}\',\'{{id}}\')">';
					$t2 .= '<img src="i/icon/edit.gif" alt=""></A>';

					$this->{$meths[$this->showDefaultControls]}('edit', $t2, 'edit');
					$this->{$meths[$this->showDefaultControls]}('main', $t1, 'edit');
				}

				//templateVars
				$tVars = array (
						'id'     => $row['id'],                 // ID объекта
						'status' => (int)$row['status'],        // Статус строки, disabled/enabled
						'format' => $this->dtype->id,           // format/data type
						'title'  => $menuTitle,                 // заголовок для context меню
						'alias'  => $alias,                     //
						'level'  => 1,                          // уровень вложенности обычно = 1
						'up'     => $up,                        // можно ли вверх ?
						'down'   => $down);                     // а вниз ?

				$html .= '<tr oncontextmenu="'.$this->_buildContext('edit', $tVars).'" class="';

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

				if ($this->dtype->childs && $this->showChildObjects && isset($this->childLink)) {
					 $html .= '<a href="'.$this->_BuildHref($this->childLink, $row['id'], 'ctype', $this->dtype->id).'">';
					 $html .= '<img src="i/icon/childs.gif" alt=""></a>';
				}

				$html .= '</td>';

				foreach ($this->dtype->fields as $field) {
					if ($field->listed) {
						$html .= "\n".'<td>'. $this->_buildCell($row, $field) .'</td>';
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
	function appendControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name])) {
			$this->controls[$name]=array();
		}

		$this->_addControl($name, $template, $context);
	}

	/**
	 * Добавляем новый control элемент в начало $this->control массива.
	 * @see _addControl
	 */
	function insertControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name])) {
			$this->controls = array($name=>NULL) + $this->controls;
		}

		$this->_addControl($name, $template, $context);
	}

	/**
	 * Добавляет элемент управления.
	 *
	 * @see _buildControls
	 *
	 * @param String $name     - название элемента управления
	 * @param String $template - шаблон эл-та управления
	 * @param String $context  - название контекстного эл-та управления
	 */
	function _addControl($name, $template, $context){
		$this->controls[$name]['template'] = $template;
		$this->controls[$name]['context']  = $context;
	}

	/**
	 * Формирует кнопочки управления по содержимому $this->controls.
	 *
	 * @param Array $vars - массив с переменными для подстановки
	 * @return String - html
	 */
	function _buildControls($vars) {

		$html = "";

		foreach ($this->controls as $cname=>$control) {
			$tmp = $control['template'];

			if (isset($control['context']) && $control['context'] !== NULL) {
				$vars['context'] = $this->_buildContext($control['context'], $vars);
			}
			foreach ($vars as $k=>$v) {
				$tmp = str_replace('{{'.$k.'}}', $v, $tmp);
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
	 * @param Array  $cVars - переменные для контекстного меню
	 */
	function _buildContext($cType, $cVars) {

		$context = '';
		switch ($cType) {
			//javascript:Context(event, 'add', parent, (format, title) )
			case 'add':
				$context = 'Context(event, \'add\', '.$cVars['parent'];
				foreach ($cVars['items'] as $format=>$title) {
					$context .= ', \''.$format.'\', \''.$title.'\' ';
				}
				$context.= '); return false;';

				break;

			//javascript:Context(event, 'edit', id, status, format, title, alias, level, up, down)
			case 'edit':
				$context  =  'Context(event, \'edit\', '.$cVars['id'].', '.$cVars['status'].', \''. $cVars['format'].'\', '.
							 '\''.$cVars['title'].'\', \''.$cVars['alias'].'\','.$cVars['level'].', '.
							 $cVars['up'].', '.$cVars['down'].'); return false;';
				break;

			//javascript:Context(event, 'file', title, isDir, isRead, isWrite, isDelete, isBinary, dir, href, side, isCopy)
			case 'file':
				$context  =  'Context(event, \'file\', \''.$cVars['title'].'\','.$cVars['isDir']. ','.
							 $cVars['isRead'].','.$cVars['isWrite'].','.$cVars['isDelete'].','.
							 $cVars['isBinary'].',\''.$cVars['dir'].'\',\''.$cVars['href'].'\','.
							 '\''.$cVars['side'].'\','.$cvars['isCopy'].'); return false;';
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
	function _buildCell($object, $field) {
		return $field->displayType->buildCell($field, $object);
	}

	/**
	 * Строим ссылку, не забываем добавлять getData.
	 *
	 * @param  String|Array $key - массив ('nameN'=>'valN') или имя переменной
	 * @param  String|NULL  $val - значение для String параметра $key
	 * @return String - строка с html кодом ссылки
	 */
	function _buildHref($key, $val = NULL) {
		$args      = func_get_args();
		$argsCount = func_num_args();

		$href = "?";
		for ($i = 0; $i < $argsCount; $i++) {
			if (is_array($args[$i])) {
				foreach ($args[$i] as $k => $v) {
					$this->_addToUrl($href, $k, $v);
				}
			} else {
				if (isset($args[$i+1])) {
					$this->_addToUrl($href, $args[$i], $args[++$i]);
				} else {
					$href = " ";
				}
			}
		}
		$href = substr($href, 0, -1);
		return $href;
	}

	function _addToUrl(&$href, $k, $v) {
		foreach ($this->getData as $gdK => $gdV)
			if ($k != $gdK && isset($gdV))
				$href .= $gdK.'='.urlencode($gdV).'&';

		$href .= $k.'='.urlencode($v).'&';
	}

	function setSiteTree($tree){
		$this->tree = $tree;
	}

	function _buildShowOnSiteLink($row){
		if(!isset($row['pathname']) || !strlen($row['pathname'])) {
			return '';
		}

		switch(true){
			case isset($this->parentPathnames):
				$pathname = isset($row['parent']) ? $this->parentPathnames[$row['parent']] : '';

				if($this->dtype->parent == 'struct') {
					return $pathname.$row['pathname'];
				} else {
					return $pathname.'?part='.$row['pathname'];
				}

			case isset($this->tree) && isset($row['parent']) && $row['parent'] > 0:
				return $this->tree->getFullPathString($row['parent']) . $row['pathname'];

			default:
				return '';
		}
	}

	function setPosition ($position) {
		$this->position = $position;
	}

	function isFirstOnPage ($row) {
		return $row == 0 && ($this->position & MASK_FIRST_PAGE) ? true : false;
	}

	function isLastOnPage ($row) {
		return $row == sizeof($this->data)-1 && $this->position & MASK_LAST_PAGE ? true : false;
	}

	function setCaption($text) {
		$this->caption = $text;
	}

	function htmlCaption() {
		if (!strlen($this->caption )) {
			return '';
		}

		$html = '';
		$html .= '<h2>'.$this->caption;

		if ($this->dtype->struct == 'tree') {
			$html .= '[<a href="'.$this->_buildHref($this->dtype->id.'_view', 'tree').'">дерево</a>]';
		}

		$html .= '</h2>';
		return $html;
	}
}//class
?>
