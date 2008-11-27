<?
/**
 * PXAdminTable - ����������� HTML ������ ��� ProximaPortal.
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
	var $childLink;

	/**
	 * ����������� ������ PXAdminTable.
	 *
	 * @param Array $data - ������
	 * @param PXTypeDescription $dtype - ��� ������
	 * @param Array $getData - ������ � GET ����������� ������� ���� ��������� � ������ �������
	 * @param String $titleKey - ��� ���� � datatype �� �������� ����� ������������ 'title' �������
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
		$this->childLink = NULL;
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
							FatalError('�� ������ ��� ������ '.$field->source);
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
	 * ��������� ������������� ������.
	 *
	 * @param Integer $currentPage - ����� �����. ���
	 * @param Integer $rowsPerPage - ����� �� 1 ��������
	 * @param Integer $rowsCount   - ��� - �� �����
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
	 * ������������� ��������� � ��������� �� ������ � ��������� id.
	 * @param Integer $id - id ������.
	 */
	function setSelected($id) {
		$this->selectedRow = $id;
	}
	/**
	 * ������������� �������� ���������� ��� ������ �� �������� �������
	 * @param String $link - ������: 'sid' ��� 'cid'
	 */
	function setChildLink($link){
		$this->childLink = $link;
	}

	/**
	 * ���������� �������.
	 * @return String - html table
	 */
	function getTable() {
		$html  = '';
		$html .= '<h2>'.$this->dtype->title;
		if ($this->rowsCount != 0) {
			$html .= '&nbsp;(' .$this->rowsCount .')';
		}

		if ($this->dtype->struct == 'tree') {
			$html .= '[<a href="'.$this->_buildHref($this->dtype->id.'_view', 'tree').'">������</a>]';
		}

		$html .= '</h2>';

		if (count($this->data)) {

			$html .= '<table class="objects">';

			//<!-- table header
			$html .= '<thead><tr>';
			$_icons = 2 + count($this->controls) + (($this->dtype->childs && $this->showChildObjects) ? 1 : 0);
			$html .= '<th class="c'.(($_icons > 2) ? $_icons : '').'"><a href="'.$this->_BuildHref($this->dtype->id.'_order', $this->dtype->order).'" title="������� ���������� �� ���������"><img src="i/icon/order.gif" width="7" height="8" alt=""></a></th>';

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

				$up    = ($rowPos == 0 && $this->currentPage == 1) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;
				$down  = ($rowPos == sizeof($this->data)-1 && $this->rowsCount <= $this->currentPage * $this->rowsPerPage) || !is_numeric(strpos($this->dtype->order, 'sys_order')) ? 0 : 1;

				if (isset($row[$this->titleKey])) {
					$menuTitle = (strlen($row[$this->titleKey]) > 10) ? trim(substr($row[$this->titleKey], 0, 10)).'&hellip;' : $row[$this->titleKey];
					$menuTitle = str_replace('"', '', str_replace("'", '', $menuTitle));
					$menuTitle = str_replace("\n", '', str_replace("\r", '', $menuTitle));
				} else {
					$menuTitle = '';
				}

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
						'id'     => $row['id'],                 // ID �������
						'status' => (int)$row['status'],        // ������ ������, disabled/enabled
						'format' => $this->dtype->id,           // format/data type
						'title'  => $menuTitle,                 // ��������� ��� context ����
						'alias'  => $alias,                     //
						'level'  => 1,                          // ������� ����������� ������ = 1
						'up'     => $up,                        // ����� �� ����� ?
						'down'   => $down);                     // � ���� ?

				$html .= '<tr oncontextmenu="'.$this->_buildContext('edit', $tVars).'" class="';

				if ($row['status'] == false) {
					$html .= ' hide';
				}

				if ($row['id'] == $this->selectedRow) {
					$html .= ' selected';
				}

				$html .= '">';

				//TD ==>

				$html .= '<td class="c">';
				$html .= $this->_buildControls($tVars);

				if ($this->dtype->childs && $this->showChildObjects && isset($this->childLink)) {
					 $html .= '<a href="'.$this->_BuildHref($this->childLink, $row['id']).'">';
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

			$html .= $this->_buildPager();
		} else {
			$html .= '<div class="empty">&#8212;</DIV>';
		}//if

		return $html;
	}

	/**
	 * ��������� ����� control ������� � ����� $this->control �������.
	 * @see _addControl
	 */
	function appendControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name]))
			$this->controls[$name]=array();
		$this->_addControl($name, $template, $context);
	}

	/**
	 * ��������� ����� control ������� � ������ $this->control �������.
	 * @see _addControl
	 */
	function insertControl($name, $template, $context = NULL ) {
		if (!isset($this->controls[$name]))
			$this->controls = array($name=>NULL) + $this->controls;
		$this->_addControl($name, $template, $context);
	}

	/**
	 * ��������� ������� ����������.
	 *
	 * @see _buildControls
	 *
	 * @param String $name     - �������� �������� ����������
	 * @param String $template - ������ ��-�� ����������
	 * @param String $context  - �������� ������������ ��-�� ����������
	 */
	function _addControl($name, $template, $context){
		$this->controls[$name]['template'] = $template;
		$this->controls[$name]['context']  = $context;
	}

	/**
	 * ��������� �������� ���������� �� ����������� $this->controls.
	 *
	 * @param Array $vars - ������ � ����������� ��� �����������
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
	 * ������ ����� ��� javascript:Context() �-���.
	 *
	 * add : parent, items(format=>title)                                                 <br>
	 * file: title, isDir, isRead, isWrite, isDelete, isBinary, dir, href, side, isCopy   <br>
	 * edit: id, status, format, title, alias, level, up, down                            <br>
	 *
	 * ������ � ������������ ���� ����� 'file' � 'add' � ������ ������ �� ��������.       <br>
	 *
	 * @param String $cType - ��� ������������ ����
	 * @param Array  $cVars - ���������� ��� ������������ ����
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
	 * ����� html ��������� �� ���������.
	 *
	 * @return String - �������������� html � ������������ ����������.
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
				$html .= '<img src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="�������� '.$prev.'">';
				$html .= '</a>';
			}

			for ($i=$start; $i<$max; $i++) {
				$html .= '<a';

				if ($i == $this->currentPage)
					$html .= ' class="selected"';

				$html .= ' href="'.$this->_BuildHref($this->dtype->id.'_page', $i).'" title="�������� '.$i.'">'.$i.'</a>';
			}

			if (isset($next) && $next > 0) {
				$html .= '<a href="'.$this->_BuildHref($this->dtype->id.'_page', $next).'">';
				$html .= '<img src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="�������� '.$next.'">';
				$html .= '</a>';
			}

			$html .= '</div>';
		}
		return $html;
	}

	/**
	 * ������ ������ ������� �� ��������� display type.
	 *
	 * @param Array $object - ������
	 * @param String $field - ���� �� �������� ����������� display type
	 */
	function _buildCell($object, $field) {
		return PXDisplayType::callMethod('buildCell', $field, $object);
	}

	/**
	 * ������ ������, �� �������� ��������� getData.
	 *
	 * @param  String|Array $key - ������ ('nameN'=>'valN') ��� ��� ����������
	 * @param  String|NULL  $val - �������� ��� String ��������� $key
	 * @return String - ������ � html ����� ������
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