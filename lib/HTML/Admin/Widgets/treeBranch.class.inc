<?php

class PXAdminTreeBranch extends PXAdminWidget
{

    public function __construct($leaf, $htmlTree, $parentLeaf)
    {
        $this->leaf = $leaf;
        $this->htmlTree = $htmlTree;
        $this->parent = $parentLeaf;
    }

    public function html()
    {
        $title = strip_tags($this->leaf->title);

        $html = '';
        $html .= '<div' . $this->divAttrs() . '>';
        $html .= '<span class="tree__leaf-title">';
        $html .= $this->paddingImages();
        $html .= $this->contextMenuLink();
        $html .= $this->editLink() . $this->toChildsLink('<span' . $this->spanAttrs() . '>' . $title . '</span>');
        $html .= $this->hintHtml();
        $html .= '</span>';

        $html .= '<span class="tree__leaf-fields">' . $this->listedFields() . '</span>';

        $html .= PXAdminTreeBranch::childrenHTML($this->leaf->children, $this->htmlTree, $this);
        $html .= '</div>';

        return $html;
    }

    public function divAttrs()
    {
        $cssClass = [
            'tree-leaf tree__leaf',
            'tree__leaf_level_' . $this->leaf->level,
            $this->htmlTree->isSelected($this->leaf->id) ? 'selected ' : '',
            $this->htmlTree->isOpenLeaf($this->leaf->parent) || $this->leaf->level <= 2 ? 'open' : 'close',
        ];

        return ' oncontextmenu="' . $this->contextMenu() . '" id="leafId' . $this->leaf->id . '" class="' . implode(' ', $cssClass) . '" ';
    }

    public function hintHtml()
    {
        $hint = '';
        foreach ($this->htmlTree->datatype->fields as $name => $descr) {
            if ($name !== 'title' && $descr->listed && is_string($this->leaf->content[$name])) {
                $hint = '[' . $this->leaf->content[$name] . ']';
                break;
            }
        }
        $hint && ($hint = ' <span class="tree__leaf-hint">' . $hint . '</span>');
        return $hint;
    }

    public function listedFields($brief = false)
    {
        $fields = [];

        foreach ($this->htmlTree->datatype->fields as $name => $descr) {
            if ($name !== 'title' && $descr->listed) {
                $param = [];
                $fields[] = '<dt>' . $descr->description . '</dt><dd>' . $descr->displayType->buildTreeHover($descr, $this->leaf->content, $param) . '</dd>';
            }
        }

        if (sizeof($fields)) {
            return sprintf("<dl class='tree__leaf-fields-container tree-fullBrief'>%s</dl>", implode('', $fields));

        } else {
            return '';
        }
    }

    public function spanAttrs()
    {
        $_ = ' onClick="ContextHide();"';
        $_ .= $this->leaf->content['status'] ? '' : ' class="hide"';

        if (isset($this->htmlTree->datatype->fields['pathname'])) {
            $_ .= ' title="' . $this->leaf->content['pathname'] . '"';
        }

        return $_;
    }

    public function contextMenuLink()
    {
        $openable = sizeof($this->leaf->children);

        if ($this->leaf->level == 1) {
            $type = !$openable ? '-0' : '-1';

        } else {
            $type = !$openable ? '-l' : '';
        }

        $alt = 'Изменить раздел &laquo;' . $this->escapeTitle() . '&raquo;';

        return '<img onClick="' . $this->contextMenu() . '" onContextMenu="' . $this->contextMenu() . '" class ="cursor" src="i/tree/leaf' . $type . '.gif" alt="' . $alt . '">';
    }

    public function getPadding()
    {
        $imgs = [];

        $parent = $this->parent;
        while (!is_null($parent) && is_a($parent, 'PXAdminTreeBranch')) {
            $type = $parent->isLast ? 'v' : 'tree';

            if ($parent->leaf->level > 1) {
                array_unshift($imgs, '<img src="i/tree/' . $type . '.gif">');
            }

            $parent = $parent->parent;
        }

        return implode('', $imgs);
    }

    public function paddingImages()
    {
        if ($this->leaf->level <= 1) {
            return '';
        }

        $id = $this->leaf->id;

        $openable = sizeof($this->leaf->children);

        if ($openable) {
            $type = $this->htmlTree->isOpenLeaf($id) ? 'open' : 'close';

        } else {
            $type = 'void';
        }

        if ($this->isLast) {
            $type .= '-last';
        }

        $img = '<img src="i/tree/' . $type . '.gif" id="leafImg' . $id . '">';

        if ($openable) {
            $img = '<a href="javascript:hideShowLeaf(\'' . $id . '\');">' . $img . '</a>';
        }

        return $this->getPadding() . $img;
    }

    public static function childrenHTML($children, &$htmlTree, $parentLeaf = null)
    {
        $html = '';

        foreach ($children as $k => $chId) {
            $isFirst = $k == 0;
            $isLast = $k + 1 == sizeof($children);

            $leaf = new PXAdminTreeBranch($htmlTree->getLeaf($chId), $htmlTree, $parentLeaf);
            $leaf->setPosition($isFirst, $isLast);
            $html .= $leaf->html();
        }

        return $html;
    }

    public function setPosition($isFirst, $isLast)
    {
        $this->isFirst = $isFirst;
        $this->isLast = $isLast;
    }

    public function contextMenu()
    {
        $canUp = $this->isFirst || !$this->htmlTree->canChangeOrder() ? 0 : 1;
        $canDown = $this->isLast || !$this->htmlTree->canChangeOrder() ? 0 : 1;

        $id = $this->leaf->id;
        $public = (int)$this->leaf->content['status'];
        $format = $this->htmlTree->datatype->id;
        $title = $this->escapeTitle();

        $pathname = $this->htmlTree->getPathname($id);
        $level = $this->leaf->level;

        return "Context(event, 'edit', {$id}, {$public}, '{$format}',  '{$title}', '{$pathname}', {$level}, {$canUp}, {$canDown}); return false;";
    }

    public function toChildsLink($html)
    {
        $href = $this->htmlTree->childsLink($this->leaf->id);

        if (mb_strlen($href)) {
            $_ = $this->leaf->content['status'] ? '' : ' class="hide"';
            $html = '<a href="' . $href . '"' . $_ . '>' . $html . '</a>';
        }

        return $html;
    }

    public function escapeTitle()
    {
        return str_replace(['"', "'"], '', strip_tags($this->leaf->title));
    }

    public function editLink()
    {
        $alt = 'Изменить раздел &laquo;' . $this->escapeTitle() . '&raquo;';
        $format = $this->htmlTree->datatype->id;
        $id = $this->leaf->id;

        $html = <<<HTML
			<a href="popup.phtml?area=objects&format={$format}&id={$id}&action=main" onclick="EditContent('{$format}', '{$id}'); return false;"><img src="i/tree/edit.gif" alt="{$alt}"></a>
HTML;

        return trim($html);
    }
}

?>
