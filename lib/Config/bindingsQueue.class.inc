<?php
require_once PPLIBPATH . 'Config/Description/Bindings/classes.inc';

class PXBindingsQueue implements Iterator
{
    private $pOrder, $rules;
    private bool $finalize;

    public function __construct()
    {
        $this->pOrder = 0;
        $this->rules = [];
        $this->finalize = false;
    }

    public function getRequestBinding()
    {
        $rArea = PXRegistry::getRequest()->getArea();

        if (strlen((string) $rArea) && sizeof(PXRegistry::getRequest()->getHostAndDir()) <= 1) {
            // FIXME
            // array_unshift($this->rules, new PXBindingDescription($this->app, array('type' => 'request', 'module' => $rArea), $this->pOrder++));
            $this->__addRule('PXBindingDescription', ['type' => 'request', 'module' => $rArea]);
        }
    }

    public function __addRule($cls, $rule)
    {
        $this->rules[] = new $cls($rule, $this->pOrder++);
    }

    public function add($binding, $type)
    {
        $rule = [
            'type' => $type,
            'var' => getFromArray($binding, 'var'),
            'value' => getFromArray($binding, 'value'),
            'order' => getFromArray($binding, 'order'),
            'module' => getFromArray($binding, 'module'),
        ];

        $cls = 'PXBindingDescription';
        $type = preg_replace('/[^0-9a-z]/' . REGEX_MOD, '', (string) $type);

        if (class_exists($cls . $type)) {
            $cls .= $type;
        }

        $this->__addRule($cls, $rule);
    }

    public function sort()
    {
        uasort($this->rules, $this->sortBindings(...));
    }

    /* Iterator methods */
    public function rewind()
    {
        reset($this->rules);
    }

    public function current()
    {
        return current($this->rules);
    }

    public function key()
    {
        return key($this->rules);
    }

    public function next()
    {
        return next($this->rules);
    }

    public function valid()
    {
        return !$this->finalize && ($this->current() !== false);
    }

    /* /Iterator methods */

    public function sortBindings($a, $b)
    {
        if ($a->order > $b->order) {
            return 1;
        }

        if ($a->order < $b->order) {
            return -1;
        }

        if ($a->pOrder > $b->pOrder) {
            return 1;
        }

        if ($a->pOrder < $b->pOrder) {
            return -1;
        }

        if ($a->type == 'main') {
            return -1;
        }

        if ($b->type == 'main') {
            return 1;
        }

        return 0;
    }

    public function stopLoading()
    {
        $this->finalize = true;
    }
}
