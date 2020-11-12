<?php
/**
 * Разделитель
 *
 * storageType virtual
 * displayType DELIMITER
 */
class PXDisplayTypeDelimiter extends PXDisplayType
{
    public function buildRow($field, $object, $param = NULL)
    {
        $even = PXDisplayType::getRow();

        $html = "\n";
        $html .= '<tr' . (!$even ? ' class="even"' : '') . '>';
        $html .= '<td colspan="2" class="delim"><div><div class="lhr"><hr size="1" noshade="1"/></div><div>&nbsp;' . $field->description;
        $html .= '&nbsp;</div><div class="rhr"><hr size="1" noshade="1"/></div></div></td>';
        $html .= '</tr>';

        return $html;
    }
}
