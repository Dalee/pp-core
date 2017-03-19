<?php

namespace PP\Lib\Rss;

/**
 * Class RssItem.
 *
 * @package PP\Lib\Rss
 */
class RssItem extends AbstractRssNode {

	function __construct($item) {
		$this->_data = $item;
		$this->nodeNames = ['title', 'link', 'guid', 'category', 'author', 'description', 'pubDate'];
	}

	function xml() {
		$_ = $this->nodeSet($this->nodeNames);

		return $this->_node('item', $_);
	}

	function link() {
		return $this->_node('link', $this->_data['link'].'?from=rss');
	}

	function guid() {
		return $this->_node('guid', $this->_data['link'].'?from=rss');
	}

	function description() {
		return $this->_node('description', '<![CDATA['.$this->_data['description'].']]>');
	}

}
