<?php

use PP\Lib\Html\Layout\LayoutAbstract;

class PXWidgetTabs extends PXAdminComplexWidget
{
    final public const PREPEND = 0;
    final public const APPEND = 1;

    public $blocks = [
        self::PREPEND => [],
        self::APPEND => []];

    public $items;
    public $selected;
    public $varName = 'area';
    public $buildHref = true;

    public function setSelected($selected)
    {
        $this->selected = $selected;
    }

    public function setVarName($varName)
    {
        $this->varName = $varName;
    }

    public function html()
    {
        $html = [];
        $this->decorate();

        $current = PXRegistry::getRequest()->getVar($this->varName, 'main');

        $html[] = "<ul class='topmenu'>";
        $html[] = join($this->blocks[self::PREPEND]);

        foreach ($this->items as $k => $v) {
            $href = $this->buildHref ? LayoutAbstract::buildHref($this->varName, $k) : '?' . $this->varName . '=' . $k;
            $html[] = sprintf("<li %s><a href='%s'>%s</a><b></b></li>", ($k === $current ? "class='current'" : ""), $href, $v);
        }

        $html[] = join($this->blocks[self::APPEND]);
        $html[] = "</ul>";

        return join($html);
    }
}
