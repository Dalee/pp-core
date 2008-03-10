<?
class PXAdminTableSimple {
	function PXAdminTableSimple($cols) {
		$this->_cols = $cols;

		$this->_dict = array();
		$this->_data = array();

		$this->_controls     = array();
		$this->_nullText = '';
	}

	function setData($data) {
		if(!is_array($data)) {
			$data = array();
		}

		$this->_data = $data;
	}

	function setDict($col, $dictArray) {
		$this->_dict[$col] = $dictArray;
	}

	function setControls($url, $title, $icon, $confirm=false, $popup=false) {
		$this->_controls[] = array(
			'title'   => $title,
			'url'     => $url,
		        'icon'    => $icon,
		        'confirm' => $confirm,
		        'popup'   => $popup,
		);
	}

	function setNullText($text) {
		$this->_nullText = $text;
	}

	function _body() {
		$_  = '';

		foreach($this->_data as $row) {
			$_ .= '<tr>';

			if(isset($row['id'])) {
				$_ .= $this->_cellCTRL($row['id']);
			}

			foreach($this->_cols as $col=>$title) {
				$_ .= $this->_cell($row, $col);
			}

			$_ .= '</tr>';
		}

		return $_;
	}

	function _cellCTRL($id) {
		if(!sizeof($this->_controls)) {
			return '';
		}

		$_  = '<td class="c">';

		foreach($this->_controls as $c) {
			$url = $c['url'].$id;

			$_ .= '<a href="'.$url.'"';

			if($c['confirm']) {
				$_ .= ' onclick="return window.confirm(\'Вы действительно решили '.$c['title'].'?\');"';

			} elseif ($c['popup']) {
				$_ .= ' onclick="Popup(this.href, 760, 550); return false;"';
			}

			$_ .= '>';
			$_ .= '<img src="/admin/i/icon/'.$c['icon'].'.gif" alt="'.$c['title'].'"></a>';
		}

		$_ .= '</td>';

		return $_;
	}

	function _cell($row, $col) {
		$val = isset($row[$col]) ? $row[$col] : null;

		$_class = array();

		if(is_null($val)) {
			$_class[] = 'null';
		}

		if(isset($this->_dict[$col]) && !isset($this->_dict[$col][$val])) {
			$_class[] = 'not-in-dict';
		}

		$_ = '<td'.(sizeof($_class) ? ' class="'.implode(' ', $_class).'"' : '').'>';

		if(is_null($val)) {
			$val = $this->_nullText;
		}

		if(isset($this->_dict[$col][$val])) {
			$val = $this->_dict[$col][$val];
		}

		$_ .= $val;

		$_ .= '</td>';

		return $_;
	}

	function html() {
		$_  = '';

		$_ .= $this->_header();
		$_ .= $this->_body();
		$_ .= $this->_footer();

		return $_;
	}

	function _header() {
		$_  = '<table class="objects">';
		$_ .= '<thead><tr>';

		if(sizeof($this->_controls)) {
			$_ .= '<th class="c c'.sizeof($this->_controls).'"><img src="/admin/i/icon/order.gif" /></th>';   // !!! FIXME !!!
		}

		foreach($this->_cols as $colTitle)  {
			$_ .= '<th>'.$colTitle.'</th>'."\n";
		}

		$_ .= '</tr></thead>';

		return $_;
	}

	function _footer() {
		return '</table>';
	}
}

?>