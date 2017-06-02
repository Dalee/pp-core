<?php

class PXInputTable extends PXAdminWidget {
	var $name;
	var $value;
	var $colsWidth;
	var $height;
	var $header;

	public function __construct($name, $value, $cols, $height, $header = array()) {
		$this->name = $name;
		$this->value = $value;
		$this->value[] = null;
		$this->height = $height;
		$this->header = $header;

		if (is_array($cols)) {
			$this->colsWidth = $cols;
		} else {
			$this->colsWidth = array();
			for ($i = 0; $i < $cols; $i++) {
				$this->colsWidth[] = 1;
			}
		}
	}

	function header() {
		return sizeof($this->header) ? sprintf(
			"<thead><tr><th>%s</th></tr></thead>",
			join('</th><th>', $this->header)
		) : '';
	}

	function colgroup() {
		$html = '<colgroup>';
		for ($i = 0; $i < count($this->colsWidth); $i++) {
			$width = ceil(100 * $this->colsWidth[$i] / array_sum($this->colsWidth));
			$html .= '<col width="' . $width . '%">';
		}
		$html .= '</colgroup>';
		return $html;
	}

	function maketd() {

	}

	function html() {
		$idx = 0;

		$html = '<table id="table' . quot($this->name) . '" class="datagrid">';
		$html .= $this->header();
		$html .= $this->colgroup();

		foreach ($this->value as $row) {
			$html .= '<tr>';

			$row = (array)$row;

			for ($i = 0, $cols = count($this->colsWidth); $i < $cols; $i++) {
				$html .= '<td>';

				$currentValue = (isset($row[$i])) ? $row[$i] : null;

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

