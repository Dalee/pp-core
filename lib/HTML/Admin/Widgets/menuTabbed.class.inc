<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/tabs.class.inc';

class PXWidgetMenuTabbed extends PXAdminComplexWidget
{
    public const BEFORE_TABS = 0;
    public const TABS = 1;
    public const AFTER_TABS = 2;

    public $items;
    public $varName = 'area';
    public $buildHref = true;
    public $selected;

    public $blocks = [
        self::BEFORE_TABS => [],
        self::TABS => [],
        self::AFTER_TABS => [],
    ];

    public function html()
    {
        $this->decorate();

        $tabs = new PXWidgetTabs();

        $tabs->items = $this->items;
        $tabs->buildHref = $this->buildHref;
        $tabs->setSelected($this->selected);
        $tabs->setVarName($this->varName);

        $this->append($tabs, self::TABS);

        $html = [];

        $html[] = join($this->blocks[self::BEFORE_TABS]);
        $html[] = join($this->blocks[self::TABS]);
        $html[] = join($this->blocks[self::AFTER_TABS]);

        return join($html);
    }
}
