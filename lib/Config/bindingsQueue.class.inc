<?php
require_once 'Description/Bindings/default.class.inc';

require_once 'Description/Bindings/struct.class.inc';
require_once 'Description/Bindings/content.class.inc';
require_once 'Description/Bindings/subcontent.class.inc';

require_once 'Description/Bindings/main.class.inc';
require_once 'Description/Bindings/types.class.inc';

class PXBindingsQueue implements Iterator {
	private $pOrder, $rules;

	function __construct() {
		$this->pOrder = 0;
		$this->rules  = array();
	}

	function _ruleToArray($node, $type) {
		return array(
			'type'   => $type,
			'rule'   => $node->nodeValue('rule'),
			'value'  => $node->nodeValue('value'),
			'order'  => $node->nodeValue('order'),
			'module' => $node->nodeValue('module')
		);
	}

	function add($xmlNode, $type) {
		$rule = $this->_ruleToArray($xmlNode, $type);

		$cls = 'PXBindingDescription';

		switch($rule['type']) {
			case 'main':
				$cls .= 'Main';
				break;

			case 'types':
				$cls .= 'Types';
				break;

			case 'struct':
				$cls .= 'Struct';
				break;

			case 'content':
				$cls .= 'Content';
				break;

			case 'subcontent':
				$cls .= 'SubContent';
				break;
		}

		$this->rules[] = new $cls($rule, $this->pOrder++);
	}

	function sort() {
		uasort($this->rules, array($this, 'sortBindings'));
	}

	/* Iterator methods */
	public function rewind() {
		reset($this->rules);
	}

	public function current() {
		return current($this->rules);
	}

	public function key() {
		return key($this->rules);
	}

	public function next() {
		return next($this->rules);
	}

	public function valid() {
		return $this->current() !== false;
	}
	/* /Iterator methods */

	function sortBindings($a, $b) {
		if($a->order > $b->order) {
			return  1;
		}

		if($a->order < $b->order) {
			return -1;
		}

		if($a->pOrder > $b->pOrder) {
			return  1;
		}

		if($a->pOrder < $b->pOrder) {
			return -1;
		}

		if($a->type == 'main') {
			return -1;
		}

		if($b->type == 'main') {
			return 1;
		}

		return 0;
	}
}
?>