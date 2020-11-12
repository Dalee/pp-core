<?php

namespace PP\Lib\Rss;

/**
 * Class RssItem.
 *
 * @package PP\Lib\Rss
 */
class RssItem extends AbstractRssNode
{

    public function __construct($item)
    {
        $this->_data = $item;
        $this->nodeNames = ['title', 'link', 'guid', 'category', 'author', 'description', 'pubDate'];
    }

    public function xml()
    {
        $_ = $this->nodeSet($this->nodeNames);

        return $this->_node('item', $_);
    }

    public function link()
    {
        return $this->_node('link', $this->_data['link'] . '?from=rss');
    }

    public function guid()
    {
        return $this->_node('guid', $this->_data['link'] . '?from=rss');
    }

    public function description()
    {
        return $this->_node('description', '<![CDATA[' . $this->_data['description'] . ']]>');
    }

}
