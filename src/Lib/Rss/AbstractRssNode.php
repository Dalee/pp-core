<?php

namespace PP\Lib\Rss;

/**
 * Class AbstractRssNode.
 *
 * @package PP\Lib\Rss
 */
abstract class AbstractRssNode {

	function _node($nodeName, $value) {
		if (is_array($value)) {
			$value = implode('', $value);
		}

		return
			<<<XML
				<$nodeName>$value</$nodeName>
XML;
	}

	function nodeSet($nodes) {
		$_ = [];

		foreach ($nodes as $node) {
			if (method_exists($this, $node)) {
				$_[] = $this->$node();

			} elseif (isset($this->_data[$node])) {
				$_[] = $this->_node($node, $this->_data[$node]);
			}
		}

		return implode("\n", $_);
	}

}
