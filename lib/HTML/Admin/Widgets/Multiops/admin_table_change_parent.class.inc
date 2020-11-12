<?php

class PXAdminTableChangeParent extends PXAdminWidget
{

    public function html()
    {
        return $this->parentWidget->dtype->parent == DT_STRUCT ? $this->_drawMoveIntoSectionLink() : '';
    }

    public function _getAllowedSections()
    {
        $param = ['datatype' => $this->parentWidget->dtype];
        $object = NULL;
        [$format, $parentFormat] = PXDisplayTypeParentdropdown::getFormats($param);
        $tree = PXDisplayTypeParentdropdown::getParentTree($object, $parentFormat);
        $parents = $tree->getPlainTree(null);

        foreach ($parents as $k => $v) {
            @$allowed = $tree->leafs[$k]->content['allowed'];

            if (isset($allowed[$format]) || $format == $parentFormat) {
                $parents[$k] = $v;
            } else {
                unset($parents[$k]);
            }
        }

        return [null => '--- Выберите раздел ---'] + $parents;
    }

    public function _drawMoveIntoSectionLink()
    {
        $opts = '';
        foreach ($this->_getAllowedSections() as $id => $name) {
            $opts .= "<option value=\"{$id}\"'>{$name}</option>";
        }
        return <<<HTML
				<div class="multiple-bottom-controls">
					<a href="#doChangeParent" helper="true" confirm="Вы уверены что хотите переместить отмеченные объекты в выбранный раздел?">Переместить в ...</a>
					<div class="helper">
						<div class="parent-section-changer">
							<form>
								<select assign='{"parent": \$(this).val()}'>
									{$opts}
								</select>
							</form>
						</div>
					</div>
				</div>
HTML;
    }
}
