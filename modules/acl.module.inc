<?
class PXModuleAcl extends PXAbstractModule {
	var $what;
	var $access;

	function PXModuleAcl($area, $settings) {
		$this->PXAbstractModule($area, $settings);

		$this->what   = array();
		$this->access = array();
		$this->sqlTable = 'acl_objects';

		if(!isset($settings['what'])) {
			FatalError('Не описаны правила what');
		}

		foreach($settings['what'] as $w) {
			$tmp = preg_split('/\s*=\s*/', $w);
			$this->what[$tmp[0]] = $tmp[1];
		}

		if(!isset($settings['access'])) {
			FatalError('Не описаны правила access');
		}

		foreach($settings['access'] as $w) {
			$tmp = preg_split('/\s*=\s*/', $w);
			$this->access[$tmp[0]] = $tmp[1];
		}
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$this->db      =& $db;
		$this->request =& $request;

		$rules  = $this->_getRules();
		$fields = $this->_getFields();
		$dicts  = $this->_getDicts();

		$table = new PXAdminTableAcl($fields);
		$table->setData($rules);

		$table->setNullText('Все');
		$table->setDict('sgroupid',   $dicts['sgroup']);
		$table->setDict('objecttype', $dicts['types']);

		$table->setDict('what',       $this->what);
		$table->setDict('access',     $this->access);

		$table->setControls('/admin/popup.phtml?area=acl&id=',                'изменить это правило', 'edit', false, true);
		$table->setControls('/admin/action.phtml?area=acl&action=delete&id=', 'удалить  это правило', 'del',  true,  false);

		$table->setControls('/admin/action.phtml?area=acl&action=up&id=',     'поднять  это правило', 'up' ,  false, false);
		$table->setControls('/admin/action.phtml?area=acl&action=down&id=',   'опустить это правило', 'down', false, false);

		$html = $table->html();

		$layout->SetInnerLayout(
			array(
				array(array('100%', '100%')),
				array('', '')
			)
		);

		$layout->Assign("INNER.0.0", $html);

		$_  = NULL;
		$_ .= '<a href="/admin/popup.phtml?area=acl" onclick="Popup(this.href); return false;" class="button">';
		$_ .= '<img src="i/icon/add.gif" alt="Добавить правило">';
		$_ .= 'Правило доступа';
		$_ .= '</a>';

		$layout->assign('INNER.0.1', $_);
	}

	function adminPopup(&$app, &$request, &$user, &$db, &$layout) {
		$this->db      =& $db;
		$this->request =& $request;

		$rules  = $this->_getRules();
		$fields = $this->_getFields();
		$dicts  = $this->_getDicts();

		$rId = $request->getId();

		$layout->SetGetVarToSave('id',     $rId);
		$layout->SetOuterForm('action.phtml', 'POST', 'multipart/form-data');

		$object = array();

		if ($rId && isset($rules[$rId])) {
			$object = $rules[$rId];
		} else {
			foreach($fields as $name=>$title) {
				$object[$name] = null;
			}
		}

		$form = new PXAdminObjectForm(null, $object); // =(

		$layout->Assign('OUTER.LEFTCONTROLS',  $form->LeftControls());
		$layout->Assign('OUTER.RIGHTCONTROLS', $form->RightControls());

		$_ = '';
		$_ .= $form->buildHidden('id', $rId);
		$_ .= $form->buildHidden('area', $this->area);
		$_ .= $form->buildHidden('action', ($rId ? 'edit' : 'add'));

		$param = array(
			'parents'     => NULL,
			'selfParents' => NULL,
			'even'        => false
		);

		$_ .= '<table class="mainform">';
		foreach($fields as $col=>$title) {
			$fieldType = new PXFieldDescription(array(), $app);
			$fieldType->name = $col;
			$fieldType->description = $title;

			if(isset($dicts[$col])) {
				$fieldType->displayType = 'DROPDOWN';

				$tmpVals = array();
				foreach($dicts[$col] as $id=>$val) {
					$tmpVals[] = array('id' => $id, 'title' => $val);
				}

				$directory = new PXDirectory($col);
				$directory->values = $tmpVals;
				$directory->displayField = 'title';

				$fieldType->values = $directory;

			} else {
				$fieldType->displayType = 'TEXT';
			}

			$_ .= PXDisplayType::callMethod('buildRow', $fieldType, $object, $param);
		}
		$_ .= '</table>';

		$layout->Append('OUTER.CONTENT', $_);

		$title = ($rId == 0) ? 'Добавление нового правила' : 'Редактирование правила &#8470;'.$rId;
		$layout->AssignTitle($title);
	}

	function _getFields() {
		return array(
			'sgroupid'     => 'Группа',
			'objectid'     => 'Объект',
			'objectparent' => 'Родитель объекта',
			'objecttype'   => 'Тип объекта',
			'what'         => 'Действие',
			'access'       => 'Доступ'
		);
	}

	function _getRules() {
		$tmp = $this->db->query('SELECT * FROM '.$this->sqlTable.' ORDER BY sys_order');

		$table = array();
		foreach($tmp as $t) {
			$table[$t['id']] = $t;
		}

		return $table;
	}

	function _getDicts() {
		$db =& $this->db;

		$dicts = array();

		// sgroup
		$db->LoadDirectoriesByType($db->types['sgroup']);
		$dicts['sgroup'] = GetColFromTableWithIndexs($db->app->directory['sgroup']->values, 'title');

		// types
		$dicts['types']  = array();
		foreach($db->types as $typeName=>$type) {
			$dicts['types'][$typeName] = $type->title;
		}
		$dicts['types'][null] = '-- любой --';

		$dicts['sgroupid']   =& $dicts['sgroup'];
		$dicts['what']       =& $this->what;
		$dicts['access']     =& $this->access;
		$dicts['objecttype'] =& $dicts['types'];

		return $dicts;
	}

	function _moveRule($rId, $direction) {
		$db =& $this->db;

		$objectInDB = $db->query('SELECT sys_order, id FROM '.$this->sqlTable.' WHERE id = '.$rId);

		if(sizeof($objectInDB) !== 1) {
			FatalError('Правило с id = '.$rId.' не найдено');
		}
		$objectInDB = current($objectInDB);

		$limit = array(1, 0);
		$where = 'sys_order '.($direction == 'down' ? '>' : '<'   ).' '.$objectInDB['sys_order'];
		$order = 'sys_order '.($direction == 'down' ? ''  : 'DESC');
		$query = 'SELECT sys_order, id FROM '.$this->sqlTable.' WHERE '.$where.' ORDER BY '.$order.' ';

		$nearObject = $db->query($query, false, $limit);

		if (sizeof($nearObject)) {
			$nearObject = current($nearObject);

			$db->modifyingQuery($db->transactionString(true));
			$db->modifyingQuery('UPDATE '.$this->sqlTable.' SET sys_order = '.$objectInDB['sys_order'].' WHERE id = '.$nearObject['id']);
			$db->modifyingQuery('UPDATE '.$this->sqlTable.' SET sys_order = '.$nearObject['sys_order'].' WHERE id = '.$objectInDB['id']);
			$db->modifyingQuery($db->transactionString(false));
		}
	}

	function adminAction(&$app, &$request, &$user, &$db) {
		$rId = (int)$request->getVar('id');
		$redir = '/admin/?area='.$this->area;

		$this->db =& $db;

		switch($request->getVar('action')) {
			case 'up':
				$this->_moveRule($rId, 'up');
				break;

			case 'down':
				$this->_moveRule($rId, 'down');
				break;

			case 'add':
				$fields = array_keys($this->_getFields());
				$values = array();

				foreach($fields as $field) {
					$values[$field] = $request->getVar($field);
				}

				$rId = $db->InsertObject($this->sqlTable, $fields, $values);
				$db->ModifyingQuery('UPDATE '.$this->sqlTable.' SET sys_order = id WHERE id = '.$rId);

				$redir = '/admin/popup.phtml?area='.$this->area.'&id='.$rId;
				break;

			case 'edit':
				$fields = array_keys($this->_getFields());
				$values = array();

				foreach($fields as $field) {
					$values[$field] = $request->getVar($field);
				}

				$db->UpdateObjectById($this->sqlTable, $rId, $fields, $values);

				$redir = '/admin/popup.phtml?area='.$this->area.'&id='.$rId;
				break;

			case 'delete':
				$rule = $db->query('SELECT count(id) as count FROM '.$this->sqlTable.' WHERE id = '.$rId);

				if(!isset($rule[0]['count'])) {
					FatalError('Какой то косяк с таблицей '.$this->sqlTable);
				}

				if((int)$rule[0]['count'] !== 1) {
					FatalError('Правило с id = '.$rId.' в таблице '.$this->sqlTable.' не найдено');
				}

				$db->modifyingQuery('DELETE FROM '.$this->sqlTable.' WHERE id = '.$rId);
				break;
		}

		return $redir;
	}
}

class PXAdminTableAcl extends PXAdminTableSimple {
	function _body() {
		$_  = '';

		foreach($this->_data as $row) {
			$_ .= '<tr'.($row['access'] === 'deny' ? ' class="deny"' : '').'>';

			if(isset($row['id'])) {
				$_ .= $this->_cellCTRL($row['id']);
			}

			foreach($this->_cols as $col=>$title) {
				$_ .= $this->_cell($row, $col);
			}

			$_ .= '</tr>';
		}

		return $_;
	}
}
?>
