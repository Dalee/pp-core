<?php

interface PXAdminWidgetIF
{
    public function html();
}

abstract class PXAdminWidget implements PXAdminWidgetIF, \Stringable
{
    public $disable;
    public $parentWidget = null;

    public function __construct()
    {
    }

    public function html()
    {
    }

    public function __toString(): string
    {
        return (string) $this->html();
    }

    public function setParent($widget)
    {
        $this->parentWidget = $widget;
    }

    public function getParentWidget()
    {
        if (is_null($this->parentWidget)) {
            $layout = PXRegistry::getLayout();
            $this->setParent($layout);
        }

        return $this->parentWidget;
    }

    public function addToParent($label)
    {
        $parent = $this->getParentWidget();
        $this->addTo($label, $parent);
    }

    public function setToParent($label)
    {
        $parent = $this->getParentWidget();
        $this->setTo($label, $parent);
    }

    public function addTo($label, $widget)
    {
        $widget->add($label, $this);
    }

    public function setTo($label, $widget)
    {
        $widget->set($label, $this);
    }

    public function setDisabled($disable)
    {
        $this->disable = $disable ? ' disabled="disabled"' : '';
    }
}

abstract class PXAdminComplexWidget extends PXAdminWidget
{
    public $blocks;

    public function __construct()
    {
        parent::__construct();
    }

    public function append($block, $label)
    { // $block commonly used for instances of PXAdminWidget or PXAdminComplexWidget, but can apply string
        $this->blocks[$label][] = $block;
    }

    public function decorate($type = null)
    {
        if ($type) {
            PXDecorativeWidgetsCollection::fillByType($this, $type);
        } else {
            PXDecorativeWidgetsCollection::fill($this);
        }
    }
}
