<?php
class PXAdminAuditTable extends PXAdminTableSimple {
	function PXAdminAuditTable($cols) {
		parent::PXAdminTableSimple($cols);
		$this->app = PXRegistry::getApp();
		$this->setDict('datatypes', $this->app->types);
	}

	function _getDatatypeTitle($format) {
		return isset($this->dict['datatypes'][$format]) ? $this->dict['datatypes'][$format]->title : $format;
	}

	function cell($row, $col) {
		if($col == 'source') {
			if (strpos($row[$col], '/')) {
				preg_match('|(\w+)/?(\d+)?|', $row[$col], $matches);

				if(sizeof($matches) == 3) {
					$row[$col] = $this->__link('popup.phtml?area=objects&format='.$matches[1].'&id='.$matches[2].'&action=main', $this->_getDatatypeTitle($matches[1]).'/'.$matches[2]);

				} elseif(sizeof($matches) == 2) {
					$row[$col] = $this->__link('?area=dicts&sid='.$matches[1], $this->_getDatatypeTitle($matches[1]));
				}

			} elseif(strlen($row[$col])) {
				$row[$col] = $this->__link('?area=dicts&sid='.$row[$col], $this->_getDatatypeTitle($row[$col]));
			}
		}

		if($col == 'user' && $row[$col] !== 'UNKNOWN') {
			$row[$col] = $this->__link('./?area=auditlog&user='.$row[$col], $row[$col]);
		}

		return parent::cell($row, $col);
	}

	function __link($url, $text) {
		return '<a href="'.$url.'" target="_blank">'.$text.'</a>';
	}
}
?>