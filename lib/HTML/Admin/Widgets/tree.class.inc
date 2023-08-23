<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/treeBranch.class.inc';

use \PP\Lib\Html\Layout\LayoutAbstract;

class PXAdminTree extends PXAdminComplexWidget
{

    public const BEFORE = 0;
    public const BEFORE_CONTENT = 1;
    public const AFTER_CONTENT = 4;
    public const AFTER = 5;

    /**
     * Конструктор класса PXAdminTree.
     * Инициализирует поля класса. необходимые для построения дерева.
     */
    public function __construct($datatype, $tree)
    {
        $this->datatype = $datatype;

        $this->tree = $tree;
        $this->getData = [];
        $this->selectedSid = 0;
        $this->leafsToOpen = [];
        $this->caption = '';
        $this->showChildLinks = false;
        $this->childLinkVarName = 'cid';

        // set up available subwidgets areas
        $this->blocks = [
            self::BEFORE => [],
            self::BEFORE_CONTENT => [],
            self::AFTER_CONTENT => [],
            self::AFTER => [],
        ];
    }

    public function &getLeaf($id)
    {
        return $this->tree->leafs[$id];
    }

    public function isSelected($id)
    {
        return $this->selectedSid == $id;
    }

    public function canChangeOrder()
    {
        return mb_strpos((string) $this->datatype->order, 'sys_order') !== false;
    }

    public function setChildLink($chVarName)
    {
        $this->childLinkVarName = $chVarName;
    }

    public function loadDropdownValues()
    {
        PXRegistry::getDb()->loadDropdownValuesForListedFields($this->datatype, $this->tree->toTable());
    }

    public function setShowChildObjects($isShow)
    {
        $this->showChildLinks = $isShow;
    }

    public function childsLink($id)
    {
        return $this->datatype->childs && $this->showChildLinks
            ? LayoutAbstract::_BuildHref($this->childLinkVarName, $id)
            : null;
    }

    public function isOpenLeaf($id)
    {
        return in_array($id, $this->leafsToOpen);
    }

    public function html()
    {
        $this->decorate();

        $caption = $this->htmlCaption();

        $html = '';
        $html .= join($this->blocks[self::BEFORE]);

        if (sizeof($this->tree->leafs[0]->children) > 0) {
            // Рекурсивно генерируем дерево
            $html .= '<div class="tree">';
            $html .= $caption;
            $html .= join($this->blocks[self::BEFORE_CONTENT]);
            $html .= '<div class="tree__container">';
            $html .= PXAdminTreeBranch::childrenHTML($this->tree->leafs[0]->children, $this);
            $html .= '</div>';
            $html .= join($this->blocks[self::AFTER_CONTENT]);
            $html .= '</div>';
        } else {
            $html .= '<div class="tree">' . $caption . '<div class="empty">&#8212;</div></div>';
        }

        $html .= join($this->blocks[self::AFTER]);

        return $html;
    }

    public function setSelected($selectedSid)
    {
        $this->selectedSid = $selectedSid;

        // Добавляем в массив "расхлопывающихся" листьев всех предков выбранного
        $tmpId = $selectedSid;
        while (isset($this->tree->leafs[$tmpId]->parent)) {
            $tmpId = $this->tree->leafs[$tmpId]->parent;
            $this->leafsToOpen[] = $tmpId;
        }

    }

    public function setOpenedLeafs($leafsToOpen)
    {
        $this->leafsToOpen = array_merge($this->leafsToOpen, $leafsToOpen);
    }

    public function getPathname($id)
    {
        return isset($this->datatype->fields['pathname']) ? $this->tree->getFullPathString($id) : '';
    }

    public function setCaption($text)
    {
        $this->caption = $text;
    }

    public function htmlCaption()
    {
        if (!mb_strlen((string) $this->caption)) {
            return '';
        }

        $html = '';
        $html .= '<h2>' . $this->caption;

        if ($this->datatype->struct == 'tree') {
            $html .= '[<a href="' . LayoutAbstract::_buildHref($this->datatype->id . '_view', 'plain') . '">список</a>]';
        }

        $html .= '</h2>';
        return $html;
    }
}
