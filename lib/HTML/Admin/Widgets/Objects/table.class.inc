<?php

class PXAdminTableObjects extends PXAdminObjects
{
    protected $objects = [];
    protected $additionalArgs = [];

    public function __construct($format, $where = NULL, $loadMethod = '')
    {
        parent::__construct($format);
        $this->where = $where;
        $this->loadMethod = $loadMethod;
        $this->getAdditionalParams(func_get_args());

        $this->createSubWidget();
    }

    public function getControlParent()
    {
        if (isset($this->controlParent)) {
            $parent = $this->controlParent;

        } else if (isset($this->format->parent)) {
            $parent = getFromArray(current($this->objects), 'parent');

        } else {
            $parent = null;
        }

        return $parent;
    }

    private
    function getAdditionalParams($arguments)
    {
        $this->additionalArgs = array_slice($arguments, 3);
    }

    public function getWhere()
    {
        return $this->where;
    }


    private
    function &loadObjects($where = NULL, $mode = DB_SELECT_TABLE, $limit = null, $offset = null, $order = NULL)
    {
        $args = func_get_args();
        $cacheDataKey = self::makeDataKey($this->format->id, $this->loadMethod, $this->additionalArgs, $args);
        if ($r = self::retrieveData($cacheDataKey)) {
            return $r;
        }

        $args = [
            $this->format,
            NULL,
        ];

        $method = 'getObjects';
        if (mb_strlen((string) $this->loadMethod)) {
            $method .= $this->loadMethod;
            $args[] = $where;

        } elseif (mb_strlen((string) $where)) {
            $method .= 'ByWhere';
            $args[] = $where;
        }

        if (sizeof($this->additionalArgs) > 0) {
            $args = array_merge($args, $this->additionalArgs);
        }

        if (!is_null($limit) && !is_null($offset)) {
            $method .= 'Limited';
            $args[] = $limit;
            $args[] = $offset;
        }

        $args[] = $mode;
        $args[] = $order;

        if (!method_exists($this->db, $method)) {
            FatalError('Undefined method ' . $method . ' in database');
        }

        $r = call_user_func_array([$this->db, $method], $args);

        self::storeData($cacheDataKey, $r);
        return $r;
    }

    protected
    function createSubWidget()
    {
        $this->count = $this->loadObjects($this->getWhere(), DB_SELECT_COUNT);

        $onPage = (int)$this->request->getVar(PXAdminPager::getPerPageParamByFormatId($this->format->id));
        $onPage = $onPage > 0 ? $onPage : $this->app->getProperty('CHILDREN_ON_PAGE', 20);

        $this->layout->setGetVarToSave(PXAdminPager::getPerPageParamByFormatId($this->format->id), $onPage);

        $currentPage = (int)$this->request->getVar($this->format->id . '_page', 1);
        $currentPage = max(($currentPage > ceil($this->count / $onPage) ? ceil($this->count / $onPage) : $currentPage), 1);

        $this->layout->setGetVarToSave($this->format->id . '_page', $currentPage);
        $this->layout->setGetVarToSave($this->format->id . '_view', 'plain');

        $currentPage = (int)$this->request->getVar($this->format->id . '_page', 1);
        $currentOrder = $this->request->getOrderVar($this->format->id, $this->format->order, $this->format->fields);

        $this->layout->setGetVarToSave($this->format->id . '_order', $currentOrder);

        $this->objects =& $this->loadObjects($this->getWhere(), DB_SELECT_TABLE, $onPage, $onPage * ($currentPage - 1), $currentOrder);
        $this->db->filterByAccess('admin', $this->format, $this->objects);

        $this->pager = new PxAdminPager($currentPage, $onPage, $this->count, $this->format, $this->layout->getData);

        $this->widget = new PXAdminTable($this->objects, $this->format, $this->layout->getData);
        $this->widget->setPosition($this->pager->getPosition());
        $this->widget->loadDropdownValues();

        $this->_setParentPathname();
    }

    private
    function _setParentPathname()
    {
        if (!sizeof($this->objects)) {
            return;
        }

        $pathnames = $this->_loadParentPathnames($this->format, $this->objects);

        if (is_array($pathnames)) {
            $this->widget->setParentPathnames($pathnames);
        }
    }

    public function appendControl($name, $template)
    {
        $this->widget->appendControl($name, $template);
    }

    private
    function _loadParentPathnames($format, $objects)
    {
        if (!isset($format->fields['pathname'])) {
            return;
        }

        if (!isset($format->parent)) {
            return;
        }

        $parentsIds = GetColFromTable($objects, 'parent');

        $pathnames = [];

        if ($format->parent == 'struct') {
            $treeObjects = new PXAdminTreeObjects('struct');
            $tree = $treeObjects->getTree();

            foreach ($parentsIds as $p) {
                $pathnames[$p] = isset($tree->leafs[$p]) ? $tree->leafs[$p]->createpath() : '';
            }

        } else {
            $parents = $this->db->getObjectsByIdArray($format->parentType(), null, $parentsIds);

            $parentPathnames = $this->_loadParentPathnames($format->parentType(), $parents);

            foreach ($parents as $p) {
                $pathnames[$p['id']] = $parentPathnames[$p['parent']] . $p['pathname'];
            }
        }

        return $pathnames;
    }


    public function has($id)
    {
        return isset($this->objects[$id]);
    }

    public function get($id)
    {
        if (!$this->has($id)) {
            FatalError('Can\'t find ' . $id);
        }

        return $this->objects[$id];
    }

    public function html()
    {
        return parent::html() . $this->pager->html();
    }
}
