<?php

class PXInputTable extends PXAdminWidget
{
    public $colsWidth;

    public function __construct(public $name, public $value, $cols, public $height, public $header = [])
    {
        $this->value[] = null;

        if (is_array($cols)) {
            $this->colsWidth = $cols;
        } else {
            $this->colsWidth = [];
            for ($i = 0; $i < $cols; $i++) {
                $this->colsWidth[] = 1;
            }
        }
    }

    public function header()
    {
        return sizeof($this->header) ? sprintf(
            "<thead><tr><th>%s</th></tr></thead>",
            join('</th><th>', $this->header)
        ) : '';
    }

    public function colgroup()
    {
        $html = '<colgroup>';
        for ($i = 0; $i < (is_countable($this->colsWidth) ? count($this->colsWidth) : 0); $i++) {
            $width = ceil(100 * $this->colsWidth[$i] / array_sum($this->colsWidth));
            $html .= '<col width="' . $width . '%">';
        }
        $html .= '</colgroup>';
        return $html;
    }

    public function maketd()
    {

    }

    public function html()
    {
        $idx = 0;

        $html = '<table id="table' . quot($this->name) . '" class="datagrid">';
        $html .= $this->header();
        $html .= $this->colgroup();

        foreach ($this->value as $row) {
            $html .= '<tr>';

            $row = (array)$row;

            for ($i = 0, $cols = is_countable($this->colsWidth) ? count($this->colsWidth) : 0; $i < $cols; $i++) {
                $html .= '<td>';

                $currentValue = $row[$i] ?? null;

                if ($this->height == 0) {
                    $text = new PXInputInput($this->name . '[' . $idx . '][]', $currentValue);
                } else {
                    $text = new PXInputTextarea($this->name . '[' . $idx . '][]', $currentValue, $this->height);
                }

                $html .= $text->html();
                $html .= '</td>';
            }

            $html .= '</tr>';

            $idx++;
        }

        $html .= '</table>';
        $html .= '<button class="addrow" onclick="AddRowNew(\'' . $this->name . '\'); return false;"' . $this->disable . '>Добавить</button>';

        return $html;
    }
}

