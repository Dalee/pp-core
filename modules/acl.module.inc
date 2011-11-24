<?php
class PXModuleAcl extends PXModule {
	var $what;
	var $access;
	var $objectRule     = 'user';
	var $aclObjectTitle = 'Тип объекта';

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->what     = $this->getAvailableActions();
		$this->access   = $this->getAvailableAccess();
		$this->sqlTable = 'acl_objects';
	}
	
	function getAvailableActions() {
		return $this->getAclDefinitionsFor('actions');
	}
	
	function getAvailableAccess(){
		return $this->getAclDefinitionsFor('access');
	}
	
	function getAclDefinitionsFor($what){
		if (empty($this->app->langTree['module_acl_rules'][$what])) {
			FatalError("Не описаны правила module_acl_rules[{$what}] в lang.yaml");
		}
		
		$_ = $this->app->langTree['module_acl_rules'][$what];
		return array_combine(array_keys($_), getColFromTable($_, 'rus')); //TODO: make lang choose optional
	}
	
	function getWhatDict() { 
		return $this->what;
	}

	function adminIndex() {
		$sid = $this->_getSid();
		$rules  = $this->_getRules($sid);
		$fields = $this->_getFields();
		$dicts  = $this->_getDicts();

		$this->indexSetMenu($sid);

		$table = new PXAdminTableSimple($fields);

		$table->setData($rules);

		$table->setNullText('Все');
		$table->setDict('sgroupid',   $dicts['sgroup']);
		$table->setDict('objecttype', $dicts['types']);
		$table->setDict('what',       $this->getWhatDict());
		$table->setDict('access',     $this->access);

		$queryParams = 'area='  . $this->area . '&sid=' . urlencode($sid);
		$table->setControls('/admin/popup.phtml?'  . $queryParams . '&id=',               'изменить это правило', 'edit', false, true);
		$table->setControls('/admin/action.phtml?' . $queryParams . '&action=delete&id=', 'удалить  это правило', 'del',  true,  false);

		$table->setControls('/admin/action.phtml?' . $queryParams . '&action=up&id=',     'поднять  это правило', 'up' ,  false, false);
		$table->setControls('/admin/action.phtml?' . $queryParams . '&action=down&id=',   'опустить это правило', 'down', false, false);
		$table->showEven();

		$table->addToParent('INNER.1.0');

		$this->layout->setTwoColumns(false);
		$this->layout->assign('INNER.1.1', $this->addNewRuleButton());
	}

	function indexSetMenu($rSid) {
		$types = array();

		$countsQueryResult = $this->db->query(sprintf('SELECT "objecttype" as "id", count(*) as "count" FROM "%s" WHERE "objectrule" = \'%s\' GROUP BY 1;', $this->sqlTable, $this->objectRule));
		$counts = array_flat($countsQueryResult, 'id', 'count');

		$types['*'] = '   <i class="all">Полный список</i> (' . array_sum($counts) . ')';
		if ($counts['']) {
			$types[''] = ' Все (' . $counts[''] . ')';
		}

		$objects = $this->objectRule === 'user'
			? $this->app->types
			: $this->app->getAvailableModules();
		foreach ($objects as $k => $v) {
			if (!isset($counts[$k])) {
				continue;
			}
			$title = (!empty($v->title) ? $v->title : (!empty($v->description) ? $v->description : (!empty($v->name) ? $v->name : $k)));
			$count = (int)$counts[$k];
			$types[$k] = "$title ($count)";
		}
		asort($types);

		if (!isset($types[$rSid])) {
			$rSid = key($types);
		}

		$this->layout->assignKeyValueList('INNER.0.0', $types, $rSid);
	}

	function addNewRuleButton() {
		$_  = '';
		$_ .= '<a href="/admin/popup.phtml?area=' . $this->area . '" onclick="Popup(this.href); return false;" class="button">';
		$_ .= '<img src="i/icon/add.gif" alt="Добавить правило" />';
		$_ .= 'Правило доступа';
		return $_ .= '</a>';
	}

	function adminPopup() {
		$layout  = $this->layout;
		$request = $this->request;
		$app     = $this->app;

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


		//set save buttons
		$form = new PXAdminForm(null, null);
		$form->LeftControls();
		$form->RightControls();

		$_ = '';
		$_ .= NLAbstractHTMLForm::buildHidden('id', $rId);
		$_ .= NLAbstractHTMLForm::buildHidden('area', $this->area);
		$_ .= NLAbstractHTMLForm::buildHidden('action', ($rId ? 'edit' : 'add'));

		$_ .= '<table class="mainform">';
		foreach($fields as $col=>$title) {
			$p = array();
			$fieldType = new PXFieldDescription(array(), $app, $p);
			$fieldType->name = $col;
			$fieldType->description = $title;
			$param = array(
				'parents'     => NULL,
				'selfParents' => NULL,
				'even'        => false
			);
			
			if(isset($dicts[$col])) {
				$dType = 'DROPDOWN';
				if($col == 'sgroupid' && $app->types['sgroup']->struct == 'tree') {
					$param['datatype']   = & $app->types['sgroup'];
					$param['root_title'] = '-- любая --';
					$dType = 'PARENTDROPDOWN';
				}
				
				$fieldType->setDisplayType($dType);

				$tmpVals = array();
				foreach($dicts[$col] as $id=>$val) {
					$tmpVals[] = array('id' => $id, 'title' => $val);
				}

				$directory = new PXDirectoryDescription($col);
				$directory->values = $tmpVals;
				$directory->displayField = 'title';

				$fieldType->values = $directory;

			} else {
				$fieldType->setDisplayType('TEXT');
			}
			
			$_ .= $fieldType->displayType->buildRow($fieldType, $object, $param);
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
			'objecttype'   => $this->aclObjectTitle,
			'what'         => 'Действие',
			'access'       => 'Доступ'
		);
	}

	function _getSid() {
		return isset($_GET['sid']) ? $_GET['sid'] : '*';
	}

	function _getRules($sid = null) {
		$andwhat = '';
		switch (true) {
			case $sid === null or $sid === '*':break;
			default:
				$sid = (string)$sid;
			case is_scalar($sid):
				$andwhat = sprintf(' AND COALESCE("objecttype", \'\') = \'%s\'', $sid);
		}
		$tmp = $this->db->query(sprintf('SELECT * FROM "%s" WHERE "objectrule" = \'%s\' %s ORDER BY "sys_order" ASC;', $this->sqlTable, $this->objectRule, $andwhat));

		$table = array();
		foreach ($tmp as $t) {
			$table[$t['id']] = $t;
		}

		return $table;
	}

	function _getTypes() {
		$types  = array();
		foreach ($this->db->types as $typeName=>$type) { 
			$types[$typeName] = $type->title;
		}

		$types[null] = '-- любой --';
		
		return $types;
	}

	function _getDicts() {
		$db =& $this->db;

		$dicts = array();

		// sgroup
		$db->LoadDirectoriesByType($db->types['sgroup']);
		$dicts['sgroup']     = GetColFromTableWithIndexs($db->app->directory['sgroup']->values, 'title');
		$dicts['types']      = $this->_getTypes();
		$dicts['what']       = $this->getWhatDict();
		
		$dicts['sgroupid']   =& $dicts['sgroup'];
		$dicts['access']     =& $this->access;
		$dicts['objecttype'] =& $dicts['types'];

		return $dicts;
	}

	function _moveRule($rId, $direction) {
		$db = $this->db;

		$objectInDB = $db->query(sprintf('SELECT sys_order, id FROM %s WHERE id = %d', $this->sqlTable, $rId));

		if(sizeof($objectInDB) !== 1) {
			FatalError('Правило с id = '.$rId.' не найдено');
		}
		$objectInDB = current($objectInDB);

		$limit  = array(1, 0);
		$where  = sprintf("sys_order %s %d AND objectrule = '%s'", GetFromArray(array('<','>'), $direction == 'down'), (int)$objectInDB['sys_order'], $this->objectRule);
		$order  = 'sys_order '.($direction == 'down' ? ''  : 'DESC');
		$query  = sprintf('SELECT sys_order, id FROM %s WHERE %s ORDER BY %s ', $this->sqlTable, $where, $order);

		$nearObject = $db->query($query, false, $limit);

		if (sizeof($nearObject)) {
			$nearObject = current($nearObject);
			$moveQuery  = "UPDATE {$this->sqlTable} SET sys_order = %d WHERE id = %d";

			$db->transactionBegin();
			$db->modifyingQuery(sprintf($moveQuery, $objectInDB['sys_order'], $nearObject['id']));
			$db->modifyingQuery(sprintf($moveQuery, $nearObject['sys_order'], $objectInDB['id']));
			$db->transactionCommit();
		}
	}

	function adminAction() {
		$rId = (int)$this->request->getVar('id');
		$sid = $this->_getSid();
		$redir = '/admin/?area='.$this->area.'&sid='.$sid;

		switch($this->request->getVar('action')) {
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
					$values[$field] = $this->request->getVar($field);
				}
				
				$ruleTypeField          = 'objectrule';
				$fields[]               = $ruleTypeField;
				$values[$ruleTypeField] = $this->objectRule;

				$rId = $this->db->InsertObject($this->sqlTable, $fields, $values);
				$this->db->ModifyingQuery('UPDATE '.$this->sqlTable.' SET sys_order = id WHERE id = '.$rId);

				$redir = '/admin/popup.phtml?area='.$this->area.'&id='.$rId;
				break;

			case 'edit':
				$fields = array_keys($this->_getFields());
				$values = array();

				foreach($fields as $field) {
					$values[$field] = $this->request->getVar($field);
				}

				$this->db->UpdateObjectById($this->sqlTable, $rId, $fields, $values);

				$redir = '/admin/popup.phtml?area='.$this->area.'&id='.$rId;
				break;

			case 'delete':
				$rule = $this->db->query('SELECT count(id) as count FROM '.$this->sqlTable.' WHERE id = '.$rId);

				if(!isset($rule[0]['count'])) {
					FatalError('Какой то косяк с таблицей '.$this->sqlTable);
				}

				if((int)$rule[0]['count'] !== 1) {
					FatalError('Правило с id = '.$rId.' в таблице '.$this->sqlTable.' не найдено');
				}

				$this->db->modifyingQuery('DELETE FROM '.$this->sqlTable.' WHERE id = '.$rId);
				break;

		}

		return $redir;
	}
}

?>
