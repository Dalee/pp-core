<?
/**
 * PXAdminTable - конструктор HTML таблиц для ProximaPortal.
 *
 * @package ProximaPortal
 * @subpackage HTML
 */
class PXAdminTable {
	var $data;
	var $dtype;
	var $getData;

	var $titleKey;

	var $parentPathname;

	var $controls;                  //custom controls array
	var $showDefaultControls;       //show default main and edit controls

	var $selectedRow;

	var $currentPage;
	var $rowsPerPage;
	var $rowsCount;
	var $showChildObjects;

	/**
	 * Конструктор класса PXAdminTable.
	 *
	 * @param Array $data - данные
	 * @param PXTypeDescription $dtype - тип данных
	 * @param Array $getData - массив с GET переменными которые надо добавлять в ссылки таблицы
	 * @param String $titleKey - тип поля в datatype по которому будет определяться 'title' таблицы
	 */
	function PXAdminTable($data, &$dtype, &$getData, $titleKey = NULL) {
		$this->data     = $data;
		$this->dtype    = $dtype;
		$this->getData  = !empty($getData) ? $getData : null ;

		$this->currentOrder = isset($getData[$this->dtype->id.'_order']) ? $getData[$this->dtype->id.'_order'] : $this->dtype->order;

		$this->titleKey = ($titleKey) ? $titleKey : 'title';

		$this->controls    = array();
		$this->showDefaultControls = true;

		$this->selectedRow = NULL;

		$this->currentPage = 0;
		$this->rowsPerPage = 0;
		$this->rowsCount   = 0;
		$this->showChildObjects = false;
	}

	function loadDropdownValues(&$db) {
		foreach($this->dtype->fields as $field) {
			if($field->listed && strlen($field->source) && !sizeof($field->values->values)) {
				switch($field->values->schema) {
					case 'xml':
						$db->app->LoadXMLDirectory($field->values, true);
						break;

					case 'sql':
						if(!isset($db->types[$field->values->source])) {
							FatalError('Не описан тип данных '.$field->source);
						}
						$idsArray = GetColFromTable($this->data, $field->name);
						if(!sizeof($idsArray)) {
							break;
						}

						$field->values->location = $field->values->source.'.id IN ('.implode(',', $idsArray).')';
						$db->LoadSQLDirectory($field->values, true, array(), $this->dtype);
						break;
				}
			}
		}
	}


	/**
	 * Настройка постраничного вывода.
	 *
	 * @param Integer $currentPage - номер текущ. стр
	 * @param Integer $rowsPerPage - строк на 1 странице
	 * @param Integer $rowsCount   - кол - во строк
	 */
	function setPager($currentPage, $rowsPerPage, $rowsCount) {

		$this->currentPage = $currentPage;
		$this->rowsPerPage = $rowsPerPage;
		$this->rowsCount   = $rowsCount;

	}

	function showDefaultControls($show=true) {
		$this->showDefaultControls = $show;
	}

	function setParentPathName($pathname) {
		$this->parentPathname = $pathname;
	}

	function setShowChildObjects($show=false) {
		$this->showChildObjects = $show;
	}


	/**
	 * Устанавливает указатель и подсветку на строку с выбранным id.
	 * @param Integer $id - id строки.
	 */
	function setSelected($id) {
		$this->selectedRow = $id;
	}

	/**
	 * Возвращает таблицу.
	 * @return Strint - html table
	 */
	function getTable() {
		$html  = '';
		$html .= '<h2>'.$this->dtype->title;
		if ($this->rowsCount != 0) {
			$html .= '&nbsp;(' .$this->rowsCount .')';
		}

		if ($this->dtype->struct == 'tree') {
			$html .= '[<a href="'.$this->_buildHref($this->dtype->id.'_view', 'tree').'">дерево</a>]';
		}

		$html .= '</h2>';

		if (count($this->data)) {

			$html .= '<table class="objects">';

			//<!-- table header
			$html .= '<thead><tr>';
			$_icons = 2 + count($this->controls) + (($this->dtype->childs && $this->showChildObjects) ? 1 : 0);
			$html .= '<th class="c'.(($_icons > 2) ? $_icons : '').'"><a href="'.$this->_BuildHref($this->dtype->id.'_order', $this->dtype->order).'" title="Порядок сортировки по умолчанию"><img src="i/icon/order.gif" width="7" height="8" alt=""></a></th>';

			foreach ($this->dtype->fields as $field) {
				if ($field->listed) {
					$html .= '<th'.($field->name === $this->titleKey ? ' class="maincol"' : '').'>';

					$param = array(
						'order' => $this->currentOrder,
						'dtype' => $this->dtype->id,
						'table' => $this
					);

					$html .= PXStorageType::callMethod('objectsSortOrder', $field, NULL, $param);

					$html .= '</th>';
				}
			}
			$html .= '</tr></thead>';
			//-->

			$rowPos = 0;
			foreach ($this->data as $rowKey=>$row){
				$html .= '<tr  class="';

				if ($row['status'] == false) {
					$html .= ' hide';
				}

				if ($row['id'] == $this->selectedRow) {
					$html .= ' selected';
				}

				$html .= '">';

				//TD ==>

				$html .= '<td class="c">';

				$up    = ($rowPos == 0 && $this->currentPage == 1) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;
				$down  = ($rowPos == sizeof($this->data)-1 && $this->rowsCount <= $this->currentPage * $this->rowsPerPage) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;

				$menuTitle = (strlen($row[$this->titleKey]) > 10) ? trim(substr($row[$this->titleKey], 0, 10)).'&hellip;' : $row[$this->titleKey];
				$menuTitle = str_replace('"', '', str_replace("'", '', $menuTitle));
				$menuTitle = str_replace("\n", '', str_replace("\r", '', $menuTitle));

				$alias = isset($row['pathname']) && isset($this->parentPathname) ? $this->parentPathname.$row['pathname'] : '';

				if ($this->showDefaultControls) {

					$t1 = '<a href="#"><img src="i/icon/menu.gif" ';
					$t1.= 'onclick="{{context}}" oncontextmenu="{{context}}" alt=""></a>';

					$t2 = '<a oncontextmenu="{{context}}" href="javascript:EditContent(\'{{format}}\',\'{{id}}\')">';
					$t2.= '<img src="i/icon/edit.gif" alt=""></A>';

					$this->insertControl('edit', $t2, 'edit');
					$this->insertControl('main', $t1, 'edit');
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

				$html .= $this->_buildControls($tVars);

				if ($this->dtype->childs && $this->showChildObjects) {
					 $varName = ($this->dtype->base == 'struct') ? 'sid' : 'cid';
					 $html .= '<a href="'.$this->_BuildHref($varName, $row['id']).'">';
					 $html .= '<img src="i/icon/childs.gif" alt=""></a>';
				}

				//<SPAN>
				$context = $this->_buildContext('edit', $tVars);
/*
				$html .= '<span onContextMenu="'.$context.'" onClick="javascript: ContextHide();" class="';

				if ($row['status'] == false) {
					$html .= ' hide';
				}



				if ($row['id'] == $this->selectedRow) {
					$html .= ' selected';
				}
				$html .= '">';

				$curTitle = strip_tags($row[$this->titleKey]);
				$html .= strlen($curTitle) > 50 ? substr($curTitle, 0, 50).'&hellip;' : $curTitle;
				$html .= '</span>';
				//</SPAN>

				if ($this->dtype->childs)
					$html .= '</a>';
*/
				$html .= '</td>';

				foreach($this->dtype->fields as $field) {
					if($field->listed) {
						$html .= "\n".'<td>'. $this->_buildCell($row, $field) .'</td>';
					}
				}

				//<== TD

				$html .= '</tr>';
				$rowPos++;
			}//foreach

			$html .= '</table>';

			$html .= $this->_buildPager();
		} else {
			$html .= '<div class="empty">&#8212;</DIV>';
		}//if

		return $html;
	}

	/**
	 * Добавляем новый control элемент в конец $this->control массива.
	 * @see _addControl
	 */
	function appendControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name]))
			$this->controls[$name]=array();
		$this->_addControl($name, $template, $context);
	}

	/**
	 * Добавляем новый control элемент в начало $this->control массива.
	 * @see _addControl
	 */
	function insertControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name]))
			$this->controls = array($name=>NULL) + $this->controls;
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
			//javascript:Context('add', parent, (format, title) )
			case 'add':
				$context = 'Context(\'add\', '.$cVars['parent'];
				foreach ($cVars['items'] as $format=>$title) {
					$context .= ', \''.$format.'\', \''.$title.'\' ';
				}
				$context.= '); window.event.cancelBubble = true; return false;';

				break;

			//javascript:Context('edit', id, status, format, title, alias, level, up, down)
			case 'edit':
				$context  =  'Context(\'edit\', '.$cVars['id'].', '.$cVars['status'].', \''. $cVars['format'].'\', '.
							 '\''.$cVars['title'].'\', \''.$cVars['alias'].'\','.$cVars['level'].', '.
							 $cVars['up'].', '.$cVars['down'].'); window.event.cancelBubble = true; return false;';
				break;

			//javascript:Context('file', title, isDir, isRead, isWrite, isDelete, isBinary, dir, href, side, isCopy)
			case 'file':
				$context  =  'Context(\'file\', \''.$cVars['title'].'\','.$cVars['isDir']. ','.
							 $cVars['isRead'].','.$cVars['isWrite'].','.$cVars['isDelete'].','.
							 $cVars['isBinary'].',\''.$cVars['dir'].'\',\''.$cVars['href'].'\','.
							 '\''.$cVars['side'].'\','.$cvars['isCopy'].'); window.event.cancelBubble = true; return false;';
				break;

		}//switch
		return $context;
	}

	/**
	 * Вывод html навигации по страницам.
	 *
	 * @return String - сформированный html с постраничной навигацией.
	 */
	function _buildPager() {

		$html = '';

		if ($this->rowsCount > $this->rowsPerPage && $this->rowsPerPage > 0) {

			$html .= '<div class="perpage">';

			$allPages = ceil($this->rowsCount/$this->rowsPerPage)+1;
			$start	= (ceil($this->currentPage/10)-1)*10+1;
			$max	  = $start + 10;

			if ($max > $allPages) {
				$max = $allPages;
			}

			if ($this->currentPage > 10) {
				$prev = (ceil($start/10)-1)*10; //-9
			}

			$last = $allPages - $start - 10;

			if ($last > 0) {
				$next = (ceil($start/10)-1)*10 + 11;
			}

			if (isset($prev)) {
				$html .= '<a href="'.$this->_BuildHref($this->dtype->id.'_page', $prev).'">';
				$html .= '<img src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
				$html .= '</a>';
			}

			for ($i=$start; $i<$max; $i++) {
				$html .= '<a';

				if ($i == $this->currentPage)
					$html .= ' class="selected"';

				$html .= ' href="'.$this->_BuildHref($this->dtype->id.'_page', $i).'" title="Страница '.$i.'">'.$i.'</a>';
			}

			if (isset($next) && $next > 0) {
				$html .= '<a href="'.$this->_BuildHref($this->dtype->id.'_page', $next).'">';
				$html .= '<img src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
				$html .= '</a>';
			}

			$html .= '</div>';
		}
		return $html;
	}

	/**
	 * Строит ячейку таблицы по заданному display type.
	 *
	 * @param Array $object - данные
	 * @param String $field - поле по которому проверяется display type
	 */
	function _buildCell($object, $field) {
		return PXDisplayType::callMethod('buildCell', $field, $object);
	}

	/**
	 * Строим ссылку, не забываем добавлять getData.
	 *
	 * @param  String|Array $key - массив ('nameN'=>'valN') или имя переменной
	 * @param  String|NULL  $val - значение для String параметра $key
	 * @return String - строка с html кодом ссылки
	 */
	function _buildHref($key, $val = NULL) {
		$href = "?";
		if (is_array($key)) {
			foreach ($key as $k => $v) {
				foreach ($this->getData as $gdK => $gdV)
					if ($k != $gdK && isset($gdV))
						$href .= $gdK.'='.urlencode($gdV).'&';

				$href .= $k.'='.urlencode($v).'&';
			}//fe

		} elseif (!is_null($key) && !is_null($val)) {
			foreach ($this->getData as $gdK => $gdV)
				if ($key != $gdK && isset($gdV))
					$href .= $gdK.'='.urlencode($gdV).'&';

			$href .= $key.'='.urlencode($val).'&';
		} else {
			$href = " ";//h8t substr -1 ,-)
		}
		$href = substr($href, 0, -1);
		return $href;
	}
}//class
?>
