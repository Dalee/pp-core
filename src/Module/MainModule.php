<?php

namespace PP\Module;

use PP\Lib\Html\Layout\AdminHtmlLayout;

/**
 * Class MainModule.
 *
 * @package PP\Module
 */
class MainModule extends AbstractModule
{
    private $rootFormatId;
    private readonly array $objectsLoadMode;
    private ?\PXTypeDescription $rootFormat = null;

    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        $this->rootFormatId = $settings['rootFormat'];

        $this->objectsLoadMode = [
            PP_CHILDREN_FETCH_NONE => 'none',
            PP_CHILDREN_FETCH_SELECTED => 'selected',
            PP_CHILDREN_FETCH_ALL => 'all',
            PP_CHILDREN_FETCH_PAGED => 'paged',
        ];
    }

    public function userIndex()
    {
        // Bad path
        if ($this->request->IsBadPath()) {
            $this->response->notFound();
            return;
        }

        // Get path
        $urlPath = $this->request->getHostAndDir();
        $urlFile = $this->request->getFile();
        $urlPart = $this->request->getPart();

        $this->tree->setFormat($this->rootFormatId);
        $this->tree->setAliases($this->_parseAliasesConfig());

        // Loading struct tree
        $this->tree->load($urlPath);
        $this->tree->getLinks();

        // Loading content objects
        $this->loadObjects($this->tree, $urlFile, $this->objects);
        $this->objects->getLinks();

        // Loading subcontent objects
        $this->loadObjects($this->objects, $urlPart, $this->subObjects);
        $this->subObjects->getLinks();

        $this->check404error($urlFile, $urlPart);
    }

    public function check404error($urlFile, $urlPart)
    {
        // Определение кода ошибки
        if (
            !$this->tree->hasCurrent() ||
            !($this->request->isIndexFile($urlFile) || $this->objects->hasCurrent()) ||
            !($this->request->isIndexFile($urlPart) || $this->subObjects->hasCurrent())
        ) {
            $this->response->notFound();
        } else {
            $this->response->setOk();
        }
    }

    protected function _parseAliasesConfig()
    {
        if (!isset($this->settings['domainAlias'])) {
            $this->settings['domainAlias'] = [];
            return [];
        }

        $config = $this->settings['domainAlias'];
        if (is_string($config)) {
            $config = explode("\n", $config);
        }

        $aliases = [];
        foreach ($config as $row) {
            [$host, $alias] = preg_split('/\s*=\s*/', trim((string) $row));
            $aliases[$host] = $alias;
        }

        return $aliases;
    }

    private function loadObjects($object, $pathName, $fillObj)
    {
        if (!$object->hasCurrent()) {
            return;
        }

        $allowed = $object->getAllowedChilds();

        if (!(is_countable($allowed) ? count($allowed) : 0)) {
            return;
        }

        foreach ($allowed as $type => $behaviour) {
            $format = \PXRegistry::getTypes($type);

            if (!is_object($format)) {
                continue;
            }

            $loadingMethod = 'load' . ucfirst((string) $this->objectsLoadMode[$behaviour]) . 'Objects';
            $objsArray = $this->$loadingMethod($object, $format, $pathName);

            $fillObj->add($type, $objsArray);
            $fillObj->findCurrent($type, $pathName);
        }
    }

    private function loadAllObjects($object, $format, $pathName)
    {
        $objsArray = [];

        $objsArray = $this->_loadContent($format, true, 'parent', $object->currentId);

        return $objsArray;
    }

    private function loadNoneObjects($object, $format, $pathName)
    {
        return [];
    }

    private function loadPagedObjects($object, $format, $pathName)
    {
        $objsArray = [];

        if (!$this->request->isIndexFile($pathName)) {
            $objsArray = $this->loadSelectedObjects($object, $format, $pathName);

        } else {
            $a_per_page = $this->app->getProperty(strtoupper((string) $format->id) . '_PER_PAGE', DEFAULT_CHILDREN_PER_PAGE);
            $count = $this->_loadContent($format, true, 'parent', $object->currentId, DB_SELECT_COUNT);
            $currentPage = $this->getCurrentPage($format, $count, $a_per_page);

            $this->layout->assign('FP_' . strtoupper((string) $format->id) . '_TOTAL', $count);
            $this->layout->assign('FP_' . strtoupper((string) $format->id) . '_PER_PAGE', $a_per_page);
            $this->layout->assign('FP_' . strtoupper((string) $format->id) . '_DEFAULT_PAGE', $currentPage);

            $objsArray = $this->_loadContentLimited($format, true, 'parent', $object->currentId, $a_per_page, $a_per_page * ($currentPage - 1));
        }

        return $objsArray;
    }

    private function loadSelectedObjects($object, $format, $pathName)
    {
        $objsArray = [];

        if (!isset($format->fields['pathname'])) {
            return;
        }

        $objsArray = $this->_loadContentLimited($format, true, ['pathname' => $pathName, 'parent' => $object->currentId], 'IGNORED', 1, 0);
        return $objsArray;
    }

    private function getCurrentPage($format, $count, $a_per_page)
    {
        // default page - from properties.ini (first or last)
        $defaultPage = $this->app->getProperty('FP_' . strtoupper((string) $format->id) . '_DEFAULT_LAST_PAGE') ? ceil($count / $a_per_page) : 1;

        $currentPage = $this->request->getVar('page', $defaultPage);
        $currentPage = max(1, ($currentPage > ceil($count / $a_per_page) ? ceil($count / $a_per_page) : $currentPage));

        return $currentPage;
    }

    public function adminJson()
    {
        $format = $this->request->getVar('f');
        $cl = $this->request->getVar('cl');
        $id = $this->request->getVar('id');
        $answer = '';
        if (!empty($id) && isset($this->app->types[$format]) && ($this->app->types[$format]->struct == 'tree' && $this->request->GetVar($format . '_view') != 'plain')) {

            // {Нам это необходимо, чтобы работал PXAdminAjaxTreeObjects
            $layout = new AdminHtmlLayout("index", $this->app->types);
            $layout->setGetVarToSave('area', $this->area);
            \PXRegistry::setLayout($layout);
            // нам это необходимо, чтобы работал PXAdminAjaxTreeObjects}

            $hTree = new \PXAdminAjaxTreeObjects($format, null, null, true, $id);
            isset($cl) && $hTree->showChildren($cl);

            $answer = $hTree->html();
        }
        return ['branch' => $answer];
    }

    public function adminIndex()
    {
        if (!isset($this->app->types[$this->rootFormatId])) {
            FatalError("Некорректный тип данных");
        }

        $app = $this->app;
        $request = $this->request;
        $layout = $this->layout;

        $this->rootFormat = $app->types[$this->rootFormatId];

        $rqSid = $request->getSid();
        $rqCid = $request->getCid();

        $hTree = new \PXAdminAjaxTreeObjects($this->rootFormat, null, null, true);

        if ($this->rootFormatId == $this->app->modules['main']->getSetting('rootFormat')) {
            $this->fillPathnamedAliases($hTree->getTree());
        }

        $hTree->hideCaption();
        $hTree->showChildren('sid');

        if ($rqSid != null && $hTree->has($rqSid)) {
            $hTree->setControlParent($rqSid);
            $hTree->setSelected($rqSid);

            $parentObject = $hTree->get($rqSid);

            $layout->assignTitle('раздел &laquo;' . $parentObject['title'] . '&raquo;');

            $cidType = null;
            $cidObject = null;

            $structHasInnerBlocks = false;

            foreach ($this->rootFormat->fields as $fieldName => $field) {
                if (!is_a($field->storageType, 'PXStorageTypeBlockcontent')) {
                    continue;
                }

                $blocks = new \PXAdminBlocksTree($this->rootFormat, $fieldName, $rqSid);
                $blocks->addToParent('INNER.1.0');
            }

            $allowedChilds = $app->getAllowedChildsKeys($this->rootFormat->id, $parentObject);
            if (count($allowedChilds)) {
                foreach ($allowedChilds as $childFormat) {
                    if (!isset($app->types[$childFormat])) {
                        $layout->append('INNER.1.0', '<h1 class="error">Формат ' . $childFormat . ' не описан</h1>');
                        continue;
                    }

                    if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat . '_view') != 'plain') {
                        $objects = new \PXAdminTreeObjects($childFormat, $rqSid, 'ByParent', true);

                    } else {
                        $objects = new \PXAdminTableObjects($childFormat, $rqSid, 'ByParent');
                    }

                    $objects->showChildren('cid');
                    $objects->setControlParent($rqSid);
                    $objects->addToParent('INNER.1.0');

                    if ($rqCid && $childFormat == $request->getCtype()) {
                        $layout->setGetVarToSave('ctype', $request->getCtype());

                        if ($objects->has($rqCid)) {
                            $cidType = $childFormat;
                            $cidObject = $objects->get($rqCid);
                        }
                    }
                }

                if ($rqCid && !empty($cidType)) {
                    $layout->setGetVarToSave('cid', $rqCid);

                    $childHasInnerBlocks = false;
                    foreach ($this->app->types[$cidType]->fields as $fieldName => $field) {
                        if (!is_a($field->storageType, 'PXStorageTypeBlockcontent')) {
                            continue;
                        }
                        $blocks = new \PXAdminBlocksTree($this->app->types[$cidType], $fieldName, $rqCid);
                        $blocks->addToParent('INNER.2.0');
                        $childHasInnerBlocks = true;
                    }

                    $allowedChilds = $app->getAllowedChildsKeys($cidType, $cidObject);
                    $layout->setThreeColumns();

                    if (count($allowedChilds)) {
                        foreach ($allowedChilds as $childFormat) {
                            if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat . '_view') != 'plain') {
                                $subObjects = new \PXAdminTreeObjects($childFormat, $rqCid, 'ByParent');

                            } else {
                                $subObjects = new \PXAdminTableObjects($childFormat, $rqCid, 'ByParent');
                            }

                            $subObjects->setControlParent($rqCid);
                            $subObjects->addToParent('INNER.2.0');
                        }

                    } elseif (!$childHasInnerBlocks) {
                        $this->layout->notSetAllowedChilds('INNER.2.0', $cidType, $rqCid);
                    }
                }

            } elseif (!$structHasInnerBlocks) {
                $this->layout->notSetAllowedChilds('INNER.1.0', $this->rootFormatId, $rqSid);
            }

        } elseif ($rqSid != null) {
            $layout->assign('INNER.1.0', '<H2 class="error">Раздел ' . htmlspecialchars((string) $rqSid, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET) . ' не найден</H2>');

        } else {
            $layout->assign('INNER.1.0', '<H2>Выберите раздел</H2>');
        }

        $hTree->addToParent('INNER.0.0');
    }

    public function _loadContent(&$format, $status, $param, $value, $flag = DB_SELECT_TABLE, $order = null)
    {
        return $this->db->GetObjectsByField($format, $status, $param, $value, $flag, $order);
    }

    public function _loadContentLimited(&$format, $status, $param, $value, $limit, $offset, $flag = DB_SELECT_TABLE, $order = null)
    {
        return $this->db->GetObjectsByFieldLimited($format, $status, $param, $value, $limit, $offset, $flag, $order);
    }

    // temporary struct root fixer. probably we must move it out to struct.class.inc (PXStructTree)
    protected function fillPathnamedAliases($tree, $host = null)
    {
        // "пустой" проект
        if (empty($tree) or empty($tree->levels[1])) {
            return;
        }

        // set current host
        is_null($host) && $host = \PXRegistry::getRequest()->getHTTPHost();
        $aliases = $this->_parseAliasesConfig();

        // находим соответствие между хостом, на который мы зашли - fixme: use case?
        // и корнем в дереве, который может быть нам нужен
        $hostAlias = 'default';
        if (isset($aliases[$host])) {
            $hostAlias = $aliases[$host];
        }

        // если алиас "не нашли", может быть нужный хост есть среди корней? - fixme: use case?
        if ($hostAlias == 'default') {
            foreach ($tree->levels[1] as $_rootId) {
                if ($tree->leafs[$_rootId]->content['pathname'] === $host) {
                    $hostAlias = $host;
                    break;
                }
            }
        }

        // находим нужный корень и правим его pathname по полному соответствию маске
        foreach ($tree->levels[1] as $_rootId) {
            if ($tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
                $tree->leafs[$_rootId]->content['__pathname'] = $tree->leafs[$_rootId]->content['pathname'];
                $tree->leafs[$_rootId]->content['pathname'] = $host;
                break;
            }
        }
    }

}
