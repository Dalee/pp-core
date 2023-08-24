<?php

class PXInputParentDropDown extends PXAdminWidget
{
    public function __construct(public $name, public $value, public $values)
    {
    }

    public function html()
    {
        $selected = false;
        $html = '<select name="' . $this->name . '"' . $this->disable . '>';

        foreach ($this->values as $k => $v) {
            if (isset($v['disabled'])) {
                $html .= '<optgroup class="disabled" label="' . quot($v['title']) . '"></optgroup>';
            } else {
                $k == $this->value && $selected = true;
                $html .= '<option value="' . quot(
                        $k
                    ) . '"' . ($k == $this->value ? ' selected' : '') . '>' . htmlspecialchars(
                        (string) $v['title'],
                        ENT_COMPAT | ENT_HTML401,
                        DEFAULT_CHARSET
                    ) . '</option>';
            }
        }

        if (!empty($this->value) && !$selected) {
            $html .= sprintf(
                '<option value="%s" selected>[нет в списке - "%s"]</option>',
                quot($this->value),
                htmlspecialchars((string) $this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET)
            );
        }

        $html .= '</select>';

        return $html;
    }
}
