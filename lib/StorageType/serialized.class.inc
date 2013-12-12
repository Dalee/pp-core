<?php
/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType serialized
 */

class PXStorageTypeSerialized extends PXStorageType {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = json_encode_koi((is_array($object[$k])) ? $object[$k] : array());

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	//TODO: удалить в феврале 2014'го. От сих...
	private function strK2U(&$str) {
		$str = myconv('k', DEFAULT_CHARSET, $str);
	}

	private function objK2U($obj) {
		switch(true) {
			default:
			case empty($obj):
				break;
			case is_array($obj):
				array_walk_recursive($obj, array($this, 'strK2U'));
				break;
			case is_string($obj):
				$this->strK2U($obj);
				break;
		}
		return $obj;
	}
	//TODO: ...до сих
	
	function normalize($field, $object, &$param) {
		$k = $field->name;
		$w = $object[$k];
		$v = null;

		// skip empties and nulls
		if ($w && $w !== 'null' && $w !== 'N;') {

			// the first byte can be one of [b, i, d, s, a, O, C, N, r, R].
			// the second byte can be one of [:, ;].
			// see http://www.phpinternalsbook.com/classes_objects/serialization.html
			// but json more simpler to check so we will do it.
			$fb   = $w[0];
			$sb   = $w[1];
			$lb   = substr($w, -1); // last byte

			// json_encoded data. not hardened check
			$json = ctype_digit($fb) || (!ctype_alpha($fb) && !ctype_alpha($fb));
			// null equals 'N;', all other /^\w:.*;$/
			$srlz = ctype_alpha($fb) && $sb === ':' && ($lb === ';' || $lb === '}');

			if ($srlz) {
				$v = @unserialize($w);

				//TODO: удалить в феврале 2014'го. От сих...
				if($v === false) {
					$v = @unserialize(myconv(DEFAULT_CHARSET, 'k', $w));
					$v = $this->objK2U($v);
				}
				//TODO: ...до сих
			}
			elseif ($json) {
				$v = json_decode_koi($w, true);
			}

		}

		return $v;
	}

}
