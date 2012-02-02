<?php

class PXStorageTypeAggr extends PXStorageTypeVirtual {

	public function normalize($field, $object, &$param) {

		$args = $field->storageTypeArgs;
		if (empty($args)) return "";

		$cb = array_shift($args);
		$args[0] = isset($object[$args[0]]) ? $object[$args[0]] : "";
		$args[1] = isset($object[$args[1]]) ? $object[$args[1]] : "";

		$res = null;
		if (is_callable(array($this, sprintf("aggr_%s", $cb)), false)) {
			$cb = array($this, sprintf("aggr_%s", $cb));
			$res = call_user_func_array($cb, $args);
		} else if (is_callable($cb)) {
			$res = call_user_func_array($cb, $args);
		}

		return $res;
	}
	
	/**
	 * Div aggregator
	 * @param type $a first arg: sum
	 * @param type $b the second arg: qty
	 * @param type $c the third arg: precision
	 * @return float
	 */
	protected function aggr_div($a, $b, $c = 2) {
		return (empty($a) || empty($b)) ? 0.0 : round($a/$b, (int)$c);
	}
	
	/**
	 * Div percentage aggregator
	 * @param type $a first arg: dividend
	 * @param type $b the second arg: divider
	 * @param type $c the third arg: precision
	 * @return float
	 */
	protected function aggr_div_percent($a, $b, $c = 2) {
		return $this->aggr_div($a, $b, (int)$c + 2) * 100 . '%';
	}
}

?>