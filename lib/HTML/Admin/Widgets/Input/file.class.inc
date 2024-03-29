<?php

require_once PPLIBPATH . 'Common/functions.html.inc';

class PXInputFile extends PXAdminWidget
{
    public string $distension = '';

    public string $type = 'file';

    public function __construct(public $name, public $value, public $entry = NULL)
    {
    }

    public function setparam()
    {
        $this->distension = is_null($this->entry) ? '' : '[' . $this->entry . ']';
    }

    public function preview()
    {
    }

    public function param()
    {
        $this->value['path'] = $this->value['fullpath'];
        $this->value['width'] = 0;
        $this->value['height'] = 0;

        return <<<HTML
			<input type="text" readonly value="{$this->value['filename']}" >
			<span>имя</span>
			<input type="text" readonly value="{$this->value['size']}" >
			<span>размер</span>
			<input type="text" readonly value="{$this->value['type']}" >
			<span>тип</span>
HTML;
    }

    public function view_copy_del()
    {
        if (!is_null($this->value['path'])) {
            $html = '<br>';

            $html .= '<a href="' . $this->value['path'] . '" target="_blank" class="img-preview">просмотр</a> ';
            $html .= '<button onclick="ToClipboardMulti(\'' . $this->value['path'] . '\', ' . (int)$this->value['width'] . ', ' . (int)$this->value['height'] . ', \'' . $this->type . '\'); return false;"' . $this->disable . '>HTML в буфер</button>';

            $html .= '<br>';

            $picName = mb_substr((string) $this->value['path'], mb_strrpos((string) $this->value['path'], '/') + 1);
            $html .= '<input class="checkbox" type="checkbox" id="' . $this->name . '_remove' . $this->distension . '" name="' . $this->name . '_remove[' . $picName . ']"' . $this->disable . '>';
            $html .= '<label for="' . $this->name . '_remove' . $this->distension . '">удалить файл</label>';
        } else {
            $html = '';
        }
        return $html;
    }

    public function html()
    {
        $this->setparam();

        $name = $this->name . (!is_null($this->entry) && mb_strlen((string) $this->entry) ? '_edit' : '') . $this->distension;

        return html_block('imagepreview', [], [
            $this->preview(),
            html_el('input', [
                'class' => 'file',
                'type' => 'file',
                'id' => $this->name . $this->distension,
                'name' => $name,
                'disabled' => $this->disable ? 'disabled' : null,
                'onpropertychange' => "Preview(this.value, '{$this->type}', '{$this->name}{$this->distension}');",
            ]),
            html_br(),
            $this->param(),
            $this->view_copy_del(),
        ]);
    }
}
