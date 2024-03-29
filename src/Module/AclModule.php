<?php

namespace PP\Module;

use Ramsey\Uuid\Uuid;

/**
 * Class AclModule.
 *
 * @package PP\Module
 */
class AclModule extends AbstractModule
{
    public $what;
    public $access;
    public $objectRule = 'user';
    public $aclObjectTitle = 'Тип объекта';

    protected $ruleTypeField;
    protected $orderingField;

    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        $this->what = $this->getAvailableActions();
        $this->access = $this->getAvailableAccess();
        $this->sqlTable = 'acl_objects';
        $this->orderingField = 'sys_order';
        $this->ruleTypeField = 'objectrule';
    }

    public function getAvailableActions()
    {
        return $this->getAclDefinitionsFor('actions');
    }

    public function getAvailableAccess()
    {
        return $this->getAclDefinitionsFor('access');
    }

    public function getAclDefinitionsFor($what)
    {
        if (empty($this->app->langTree['module_acl_rules'][$what])) {
            FatalError("Не описаны правила module_acl_rules[{$what}] в lang.yaml");
        }

        $_ = $this->app->langTree['module_acl_rules'][$what];
        return array_combine(array_keys($_), getColFromTable($_, 'rus')); //TODO: make lang choose optional
    }

    public function getWhatDict()
    {
        return $this->what;
    }

    public function adminIndex()
    {
        $sid = $this->_getSid();
        $rules = $this->_getRules($sid);
        $fields = $this->_getFields();
        $dicts = $this->_getDicts();

        $this->indexSetMenu($sid);

        $table = new \PXAdminTableSimple($fields);

        $table->setData($rules);

        $table->setNullText('Все');
        $table->setDict('sgroupid', $dicts['sgroup']);
        $table->setDict('objecttype', $dicts['types']);
        $table->setDict('what', $this->getWhatDict());
        $table->setDict('access', $this->access);

        $queryParams = 'area=' . $this->area . '&sid=' . urlencode((string) $sid);
        $table->setControls('/admin/popup.phtml?' . $queryParams . '&id=', 'изменить это правило', 'edit', false, true);
        $table->setControls('/admin/action.phtml?' . $queryParams . '&action=delete&id=', 'удалить  это правило', 'del', true, false);

        $table->setControls('/admin/action.phtml?' . $queryParams . '&action=up&id=', 'поднять  это правило', 'up', false, false);
        $table->setControls('/admin/action.phtml?' . $queryParams . '&action=down&id=', 'опустить это правило', 'down', false, false);
        $table->showEven();

        $table->setTableId('acl');

        $table->addToParent('INNER.1.0');

        $this->layout->setTwoColumns(false);
        $this->layout->assign('INNER.1.1', $this->addNewRuleButton());
    }

    public function indexSetMenu($rSid)
    {
        $types = [];

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

            if ($v instanceof \PXModuleDescription) {
                $title = $v->getDescription() == '' || $v->getDescription() == \PXModuleDescription::EMPTY_DESCRIPTION
                    ? $v->getName()
                    : $v->getDescription();
            } else {
                $title = (!empty($v->title) ? $v->title : (!empty($v->description) ? $v->description : (!empty($v->name) ? $v->name : $k)));
            }

            $count = (int)$counts[$k];
            $types[$k] = "$title ($count)";
        }

        asort($types);

        if (!isset($types[$rSid])) {
            $rSid = key($types);
        }

        $this->layout->assignKeyValueList('INNER.0.0', $types, $rSid);
    }

    public function addNewRuleButton()
    {
        $button = new \PXControlButton('Правило доступа');
        $button->setClickCode('Popup(\'' . '/admin/popup.phtml?area=' . $this->area . '\')');
        $button->setClass('add');
        return $button->html();
    }

    public function adminPopup()
    {
        $layout = $this->layout;
        $request = $this->request;
        $app = $this->app;

        $rules = $this->_getRules();
        $fields = $this->_getFields();
        $dicts = $this->_getDicts();

        $rId = $request->getId();

        $layout->SetGetVarToSave('id', $rId);
        $layout->SetOuterForm('action.phtml', 'POST', 'multipart/form-data');

        $object = [];

        if ($rId && isset($rules[$rId])) {
            $object = $rules[$rId];
        } else {
            foreach ($fields as $name => $title) {
                $object[$name] = null;
            }
        }


        //set save buttons
        $form = new \PXAdminForm(null, null);
        $form->leftControls();
        $form->rightControls();

        $_ = '';
        $_ .= \NLAbstractHTMLForm::BuildHidden('id', $rId);
        $_ .= \NLAbstractHTMLForm::BuildHidden('area', $this->area);
        $_ .= \NLAbstractHTMLForm::BuildHidden('action', ($rId ? 'edit' : 'add'));

        $_ .= '<table class="mainform">';
        foreach ($fields as $col => $title) {
            $p = [];
            $fieldType = new \PXFieldDescription([], $app, $p);
            $fieldType->name = $col;
            $fieldType->description = $title;
            $param = [
                'parents' => null,
                'selfParents' => null,
                'even' => false,
            ];

            if (isset($dicts[$col])) {
                $dType = 'DROPDOWN';
                if ($col == 'sgroupid' && $app->types['sgroup']->struct == 'tree') {
                    $param['datatype'] = &$app->types['sgroup'];
                    $param['root_title'] = '-- любая --';
                    $dType = 'PARENTDROPDOWN';
                }

                $fieldType->setDisplayType($dType);

                $tmpVals = [];
                foreach ($dicts[$col] as $id => $val) {
                    $tmpVals[] = ['id' => $id, 'title' => $val];
                }

                $directory = new \PXDirectoryDescription($col);
                $directory->values = $tmpVals;
                $directory->displayField = 'title';

                $fieldType->values = $directory;

            } else {
                $fieldType->setDisplayType('TEXT');
            }

            $_ .= $fieldType->displayType->buildRow($fieldType, $object, $param);
        }
        $_ .= '</table>';

        $layout->append('OUTER.CONTENT', $_);

        $title = ($rId == 0) ? 'Добавление нового правила' : 'Редактирование правила &#8470;' . $rId;
        $layout->assignTitle($title);
    }

    public function _getFields()
    {
        return [
            'sgroupid' => 'Группа',
            'objectid' => 'Объект',
            'objectparent' => 'Родитель объекта',
            'objecttype' => $this->aclObjectTitle,
            'what' => 'Действие',
            'access' => 'Доступ',
        ];
    }

    public function _getSid()
    {
        return $_GET['sid'] ?? '*';
    }

    public function _getSidCriteria($sid)
    {
        $andwhat = ['blank' => '1 = 1'];

        if ($sid === null || $sid === '*') {
            return $andwhat;
        }

        $sid = $this->db->escapeString($sid);
        $andwhat['sid'] = sprintf(' COALESCE("objecttype", \'\') = \'%s\'', $sid);

        return $andwhat;
    }

    public function _getRules($sid = null, $limit = null)
    {
        $criterias = $this->_getSidCriteria($sid);
        $criterias[] = sprintf('"objectrule" = \'%s\'', $this->objectRule);
        $where = join(' AND ', $criterias);
        $limit = ($limit) ? [(int)$limit, 0] : null;

        $tmp = $this->db->query(sprintf('SELECT * FROM "%s" WHERE %s ORDER BY "%s" ASC;', $this->sqlTable, $where, $this->orderingField), false, $limit);

        return $limit == 1 ? reset($tmp) : array_flat($tmp, 'id');
    }

    public function _getTypes()
    {
        $types = [];

        foreach ($this->db->types as $typeName => $type) {
            $types[$typeName] = $type->title;
        }

        $types[null] = '-- любой --';

        return $types;
    }

    public function _getDicts()
    {
        $dicts = [];

        // sgroup
        $this->db->LoadDirectoriesByType($this->db->types['sgroup']);
        $dicts['sgroup'] = GetColFromTableWithIndexs($this->db->app->directory['sgroup']->values, 'title');
        $dicts['types'] = $this->_getTypes();
        $dicts['what'] = $this->getWhatDict();

        $dicts['sgroupid'] = & $dicts['sgroup'];
        $dicts['access'] = & $this->access;
        $dicts['objecttype'] = & $dicts['types'];

        return $dicts;
    }

    protected function getRuleById($rId, $sid = false)
    {
        return current($this->getRulesByIds($rId, $sid));
    }

    protected function getRulesByIds($ids, $sid = false)
    {
        $ids = array_filter(array_map('intval', (array)$ids));
        if (empty($ids)) {
            return [];
        }

        $criteria = $this->_getSidCriteria($sid);
        $criteria['id'] = sprintf('"id" IN (%s)', join(',', $ids));
        $where = join(' AND ', $criteria);

        $query = sprintf('SELECT %s, id FROM %s WHERE %s;', $this->orderingField, $this->sqlTable, $where);

        return $this->db->query($query);
    }


    protected function getRulesByRuleAndDirection($rule, $direction, $sid = false, $limit = 1)
    {
        if ($limit <= 0) {
            return null;
        }

        // combining query
        $criteria = $this->_getSidCriteria($sid);
        $criteria['rule'] = sprintf('"objectrule" = \'%s\'', $this->objectRule);

        // set where and order by $direction
        switch ($direction) {
            default:
            case 'up':
                $criteria['field'] = '"%1$s" < %2$d';
                $order = '"%1$s" DESC';
                break;
            case 'down':
                $criteria['field'] = '"%1$s" > %2$d';
                $order = '"%1$s" ASC';
        }
        $criteria['field'] = sprintf($criteria['field'], $this->orderingField, $rule[$this->orderingField]);

        $where = join(' AND ', $criteria);
        $order = sprintf($order, $this->orderingField);
        $query = sprintf('SELECT "%s", "id" FROM %s WHERE %s ORDER BY %s ', $this->orderingField, $this->sqlTable, $where, $order);

        $objectsInDb = $this->db->query($query, false, [$limit, 0]);
        if (!sizeof($objectsInDb)) {
            return null;
        }

        return $limit == 1 ? current($objectsInDb) : $objectsInDb;
    }

    protected function getFirstRule($sid = false)
    {
        return $this->_getRules($sid, 1);
    }

    protected function getRulesBetweenOrders($from, $to, $sid = false)
    {
        echo($from . ' ' . $to) . PHP_EOL;
        if ($from > $to) {
            $from ^= $to ^= $from ^= $to;
        }

        $criteria = $this->_getSidCriteria($sid);
        $criteria['rule'] = sprintf('"objectrule" = \'%s\'', $this->objectRule);
        $criteria['between'] = sprintf('"%s" BETWEEN %d AND %d', $this->orderingField, $from, $to);
        $where = join(' AND ', $criteria);

        $query = sprintf('SELECT "%1$s", "id" FROM "%2$s" WHERE %3$s ORDER BY "%1$s" ASC;', $this->orderingField, $this->sqlTable, $where);

        return $this->db->query($query);
    }

    protected function swapOrders($a, $b)
    {
        $this->db->transactionBegin();
        $this->setOrder($a[$this->orderingField], $b['id']);
        $this->setOrder($b[$this->orderingField], $a['id']);
        $this->db->transactionCommit();

        return true; // fixme.
    }

    protected function _moveRule($rId, $direction, $sid = false)
    {

        $moving = $this->getRuleById($rId, $sid);
        $passive = $this->getRulesByRuleAndDirection($moving, $direction, $sid);

        if ($passive) {
            return $this->swapOrders($moving, $passive);
        }

        return false;
    }

    protected function setOrder($order, $criteria)
    {
        if (is_string($order) && ($order[0] == '-' || $order[0] == '+') && ctype_digit(substr((string) $order, 1))) {
            // makes -1, +1 sql-readable
            $order = sprintf('COALESCE("%s",0) %s', $this->orderingField, $order);
        } else {
            $order = (int)$order;
        }

        $where = match (true) {
            is_int($criteria) || ctype_digit((string) $criteria) => '"id" = ' . $criteria,
            is_string($criteria) && ($criteria[0] == '<' || $criteria[0] == '>') && ctype_digit(substr($criteria, 1)), is_string($criteria) && ($criteria[0] == '<' || $criteria[0] == '>') && ctype_digit(substr($criteria, 2)) && $criteria[1] == '=' => $this->orderingField . $criteria,
            is_array($criteria) => join(' AND ', $criteria),
            default => $criteria,
        };

        $set = ["{$this->orderingField} = {$order}"/*, "sys_modified = now()"*/];
        $set = join(",", $set);

        $query = "UPDATE {$this->sqlTable} SET {$set} WHERE {$where};";

        $this->db->modifyingQuery($query);
    }

    protected function getMaxOrder($sid)
    {
        static $maxOrder;
        if ($maxOrder) {
            return $maxOrder;
        }

        $where = join(' AND ', $this->_getSidCriteria($sid));
        [[$maxOrder]] = $this->db->query("SELECT max({$this->orderingField}) as \"0\" FROM {$this->sqlTable} WHERE {$where};");
        return $maxOrder;
    }

    protected function putRuleAfterRule($movingId, $afterId, $sid = false)
    {
        $moving = null;
        $after = null;
        $changing = null;
        $where = null;

        $rules = array_flat($this->getRulesByIds([$movingId, $afterId], $sid), 'id', $this->orderingField);
        if (!array_key_exists($movingId, $rules)) {
            return false;
        }

        $movingOrder = $rules[$movingId];
        if (null === $movingOrder) {
            $movingOrder = $this->getMaxOrder($sid);
        }

        switch (true) {
            case $afterId == 'first' || $afterId == 0:
                $this->setOrder('+1', "<" . $movingOrder);
                $this->setOrder(0, $movingId);
                return true;
            case is_int($afterId) || ctype_digit((string) $afterId):
                $afterOrder = $rules[$afterId];
                break;
            default:
                return 'after?!';
        }

        if ($afterOrder === null) {
            // hm. cant be moved...
            return 'do something with nulls!';
        }

        // fyi: all nulled at the end. _not at start_
        $oField = $this->orderingField;

        switch (true) {
            case $movingOrder - 1 == $afterOrder:
                // already correct order
                break;
            default:
            case $movingOrder == $afterOrder:
                // dunno... what we can do here? hm.
                $this->setOrder('+1', $movingId);
                // or that?
                // $criteria['upper'] = sprintf('("%s" > %d OR "id" = %d)', $oField, $movingOrder, $moving['id']);
                // $this->setOrder('+1', $criteria)
                break;
            case $movingOrder < $afterOrder:
                // a 1      | a 1      | a 1  // set b after e
                // b 2      | b 2 -> 4 | c 2  // moving = b, after = e
                // c 3 -> 2 | c 2      | d 2
                // d 3 -> 2 | d 3      | e 3
                // e 4 -> 3 | e 4      | b 4
                $criteria = sprintf('%2$d < %1$s AND %1$s <= %3$d', $oField, $movingOrder, $afterOrder);

                $this->setOrder('-1', $criteria);
                $this->setOrder($afterOrder, $movingId);
                break;
            case $movingOrder > $afterOrder:
                // a 1      | a 1      | a 1  // set e after a (e.order > a.order)
                // b 2 -> 3 | b 3      | e 2  // moving = e, after = a
                // c 3 -> 4 | c 4      | b 3
                // d 3 -> 4 | d 4      | c 4
                // e 4      | e 4 -> 2 | d 4
                $criteria = sprintf('%2$d < %1$s AND %1$s <= %3$d', $oField, $afterOrder, $movingOrder);

                $this->setOrder('+1', $criteria);
                $this->setOrder($afterOrder + 1, $movingId);
                break;
        }

        return compact('moving', 'after', 'changing', 'where');
    }

    public function adminAction()
    {
        $rId = (int)$this->request->getVar('id');
        $afterId = (int)$this->request->getVar('after');
        $sid = $this->_getSid();
        $redir = '/admin/?area=' . $this->area . '&sid=' . urlencode((string) $sid);
        $ajax = $this->request->getVar('ajax');
        $result = null;

        $action = $this->request->getVar('action');
        switch ($action) {
            case 'up':
            case 'down':
                $result = $this->_moveRule($rId, $action, $sid);
                break;

                // ajax
            case 'putafter':
                $result = $this->putRuleAfterRule($rId, $afterId, $sid);
                break;

            case 'add':
                $fields = array_keys($this->_getFields());
                $values = [];

                foreach ($fields as $field) {
                    $values[$field] = $this->request->getVar($field);
                }

                $fields[] = $this->ruleTypeField;
                $fields[] = 'sys_uuid';

                $values[$this->ruleTypeField] = $this->objectRule;
                $values['sys_uuid'] = Uuid::uuid4();

                $rId = $this->db->InsertObject($this->sqlTable, $fields, $values);
                $this->setOrder($rId, $rId);

                $redir = '/admin/popup.phtml?area=' . $this->area . '&id=' . $rId;
                break;

            case 'edit':
                $fields = array_keys($this->_getFields());
                $values = [];

                foreach ($fields as $field) {
                    $values[$field] = $this->request->getVar($field);
                }

                $this->db->UpdateObjectById($this->sqlTable, $rId, $fields, $values);

                $redir = '/admin/popup.phtml?area=' . $this->area . '&sid=' . urlencode((string) $sid) . '&id=' . $rId;
                break;

            case 'delete':
                $rule = $this->db->query('SELECT count(id) as count FROM ' . $this->sqlTable . ' WHERE id = ' . $rId);

                if (!isset($rule[0]['count'])) {
                    FatalError('Ошибка в таблице: ' . $this->sqlTable);
                }

                if ((int)$rule[0]['count'] !== 1) {
                    FatalError('Правило с id = ' . $rId . ' в таблице ' . $this->sqlTable . ' не найдено');
                }

                $this->db->modifyingQuery('DELETE FROM ' . $this->sqlTable . ' WHERE id = ' . $rId);
                break;
        }

        if ($ajax) {
            die(json_encode($result));
        }

        return $redir;
    }
}
