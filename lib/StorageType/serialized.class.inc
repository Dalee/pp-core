<?php
/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType serialized
 */

class PXStorageTypeSerialized extends PXStorageType
{
	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : [];
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;
		$v = json_encode((is_array($object[$k])) ? $object[$k] : []);

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}

	public function normalize($field, $object, &$param)
	{
		$k = $field->name;
		$w = $object[$k];
		$v = null;

		// skip empties and nulls
		if ($w && $w !== 'null' && $w !== 'N;') {

			// the first byte can be one of [b, i, d, s, a, O, C, N, r, R].
			// the second byte can be one of [:, ;].
			// see http://www.phpinternalsbook.com/classes_objects/serialization.html
			// but json more simpler to check so we will do it.
			$fb = $w[0];
			$sb = $w[1];
			$lb = substr($w, -1); // last byte

			// json_encoded data. not hardened check
			$json = ctype_digit($fb) || (!ctype_alpha($fb) && !ctype_alpha($fb));
			// null equals 'N;', all other /^\w:.*;$/
			$srlz = ctype_alpha($fb) && $sb === ':' && ($lb === ';' || $lb === '}');

			if ($srlz) {
				$v = @unserialize($w);
			} elseif ($json) {
				$v = json_decode($w, true);
			}

		}

		return $v;
	}

}
