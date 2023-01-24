<?php

use PP\Lib\Objects\ContentObjectsInterface;

/* TODO: move this code to NLLeaf */

class PXStructLeaf implements Iterator
{
    private $data, $struct;

    public function __construct($leaf, $struct)
    {
        $this->data = $leaf;
        $this->struct = $struct;
    }

    public function __call($name, $arguments)
    {
        if (method_exists($this->data, $name)) {
            return call_user_func_array([$this->data, $name], $arguments);

        } else {
            $trace = PXTrace(); // TODO: refactor
            FatalError('Call to undefined method ' . self::class . '::' . $name . '() in ' . $trace[1]['file'] . ' on line ' . $trace[1]['line']);
        }
    }

    public function parent($level = 1)
    {
        $parentId = is_object($this->data) ? $this->data->parent($level) : null;
        return is_null($parentId) ? new PXStructLeaf404() : $this->struct->getLeaf($this->data->tree->leafs[$parentId]);
    }

    public function __get($key)
    {
        if (isset($this->data->{$key})) {
            return $this->data->{$key};
        }
        if (!isset($this->data->content[$key])) {
            return null;
        }
        return $this->data->content[$key];
    }

    public function path()
    {
        return $this->data->tree->getFullPathString($this->data->id);
    }

    public function isCurrent()
    {
        return $this->data->id == $this->struct->currentId;
    }

    public function isRoot()
    {
        return $this->data->id == $this->struct->rootId;
    }

    public function hasChilds()
    {
        return sizeof($this->data->children) > 0;
    }


    public function createpath()
    {
        return createPathByParentId($this->struct->tree, $this->id);
    }

    /* Iterator methods */
    public function valid()
    {
        return $this->current() !== false;
    }

    public function rewind()
    {
        reset($this->data->children);
    }

    public function current()
    {
        $key = current($this->data->children);

        if ($key === false) {
            return false;
        }

        return $this->struct->getLeaf($this->data->tree->leafs[$key]);
    }

    public function key()
    {
        return key($this->data->children);
    }

    public function next()
    {
		return next($this->data->children);
    }

    /* /Iterator methods */
}

class PXStructLeaf404 extends PXStructLeaf
{
    public function __construct()
    {
        $this->id = -1;
    }

    public function __get($key)
    {
        return null;
    }

    public function path()
    {
        return null;
    }

    public function isCurrent()
    {
        return false;
    }

    public function isRoot()
    {
        return false;
    }

    public function hasChilds()
    {
        return false;
    }

    public function valid()
    {
        return false;
    }

    public function rewind()
    {
    }

    public function current()
    {
        return false;
    }

    public function key()
    {
        return false;
    }

    public function next()
    {
        return false;
    }
}

class PXTreeObjects implements ContentObjectsInterface, ArrayAccess, Iterator
{
    public $format;
    public $tree; //FIXME: really public ? Has some direct calls from PXStructLeaf.

    public $pathId = [];
    public $rootId = -1;
    public $currentId;

    private array $leafsCache = [];
    private bool $treeIsLoaded = false;

    public function __construct()
    {
        $this->tree = PXRegistry::getDb()->buildTree([]);
    }

    public function __call($name, $arguments)
    {
        if (method_exists($this->tree, $name)) {
            return call_user_func_array([$this->tree, $name], $arguments);
        } else {
            $trace = pxtrace();
            FatalError('Call to undefined method ' . self::class . '::' . $name . '() in ' . $trace[1]['file'] . ' on line ' . $trace[1]['line']);
        }
    }

    public function __get($key)
    {
        $conds = [];
        switch ($key) {
            case 'root':
                $conds['root'] = [$this->rootId !== -1, $this->rootId];
            case 'current':
                $conds['current'] = [$this->hasCurrent(), $this->currentId];

                if ($conds[$key][0]) {
                    return $this->getLeaf($this->tree->leafs[$conds[$key][1]]);
                }

                return new PXStructLeaf404();
            default:
                if (isset($this->tree->$key)) {
                    return $this->tree->$key;
                }
        }

        return null;
    }

    public function __isset($key)
    {
        return isset($this->$key) || isset($this->tree->$key);
    }

    public function getLeaf($leafArray)
    {
        if (!isset($this->leafsCache[$leafArray->id])) {
            $this->leafsCache[$leafArray->id] = new PXStructLeaf($leafArray, $this);
        }

        return $this->leafsCache[$leafArray->id];
    }


    public function currentType()
    {
        if (!$this->hasCurrent()) {
            return;
        }

        $current = $this->getCurrent();
        return $current->content['type'];
    }

    public
    function setCurrent($id)
    {
        $this->currentId = $id;
    }

    public
    function setCurrentType($type)
    {
        if (!$this->hasCurrent()) {
            return false;
        }

        $this->tree->leafs[$this->currentId]->content['type'] = $type;

        return true;
    }

    public function hasCurrent()
    {
        return isset($this->tree->leafs[$this->currentId]);
    }

    public function getCurrent()
    {
        return $this->getLeaf($this->tree->leafs[$this->currentId]);
    }

    public function getCurrentType()
    {
        return $this->currentType();
    }

    // FIXME !!!
    public function getLinks()
    {
    }

    public function findCurrent($urlPath)
    {
        // Guessing context
        $this->_findRoot($urlPath[0]);

        $this->pathId = $this->tree->getIdArrayByPath('pathname', $urlPath);

        $this->currentId = end($this->pathId);
        $this->rootId = reset($this->pathId);
    }

    public function _findRoot($host)
    {
        // "пустой" проект
        if (!isset($this->tree->levels[1])) {
            return;
        }

        // находим соответствие между хостом, на который мы зашли
        // и корнем в дереве, который может быть нам нужен
        $hostAlias = 'default';
        if (isset($this->aliases[$host])) {
            $hostAlias = $this->aliases[$host];
        }

        // если алиас "не нашли", может быть нужный хост есть среди корней?
        if ($hostAlias == 'default') {
            foreach ($this->tree->levels[1] as $_rootId) {
                if ($this->tree->leafs[$_rootId]->content['pathname'] === $host) {
                    $hostAlias = $host;
                    break;
                }
            }
        }

        // находим нужный корень и правим его pathname
        foreach ($this->tree->levels[1] as $_rootId) {
            if ($this->tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
                $this->tree->leafs[$_rootId]->content['__pathname'] = $this->tree->leafs[$_rootId]->content['pathname'];
                $this->tree->leafs[$_rootId]->content['pathname'] = $host;
                break;
            }
        }
    }

    public function setAliases($domainAliases)
    {
        $this->aliases = $domainAliases;
    }

    public function setFormat($type)
    {
        $this->format = PXRegistry::getTypes($type);
    }

    public function load($urlPath)
    {
        $this->tree = PXRegistry::getDb()->getObjects($this->format, true, DB_SELECT_TREE);
        $this->findCurrent($urlPath);
        $this->treeIsLoaded = true;
    }

    public function isLoaded()
    {
        return $this->treeIsLoaded;
    }

    public function getAllowedChilds()
    {
        $cur = $this->getCurrent();
        return PXRegistry::getApp()->getAllowedChilds($this->format->id, $cur);
    }

    public function hasType($type)
    {
        return isset($this->format->id) && $type === $this->format->id && !empty($this->tree->leafs);
    }

    // ArrayAccess if
    public function offsetGet($id)
    {
        return $this->getLeaf($this->tree->leafs[$id]);
    }

    public function offsetSet($id, $object)
    {
        $this->tree->leafs[$id] = $object;
    }

    public function offsetExists($id)
    {
        return isset($this->tree->leafs[$id]);
    }

    public function offsetUnset($id)
    {
        unset($this->tree->leafs[$id]);
        unset($this->leafsCache[$id]);
    }

    /* Iterator */
    public function rewind()
    {
        reset($this->tree->leafs);
    }

    public function current()
    {
        return $this->getLeaf($this->tree->leafs[$this->key()]);
    }

    public function key()
    {
        return key($this->tree->leafs);
    }

    public function next()
    {
        next($this->tree->leafs);
    }

    public function valid()
    {
        $current = key($this->tree->leafs);
        return isset($this->tree->leafs[$current]);
    }
    /* /Iterator */
}
