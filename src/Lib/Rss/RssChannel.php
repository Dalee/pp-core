<?php

namespace PP\Lib\Rss;

/**
 * Class RssChannel.
 *
 * @package PP\Lib\Rss
 */
class RssChannel extends AbstractRssNode
{
    public function __construct($channel, &$rssEngine)
    {
        $this->_data = $channel;
        $this->rss = & $rssEngine;

        $this->nodeNames = [
            'title', 'link', 'description', 'language', 'copyright',
            'managingEditor', 'webMaster', 'generator', 'ttl', 'image', 'lastBuildDate',
        ];
    }

    public function link()
    {
        return $this->_node('link', $this->_data['link'] . '?from=rss');
    }

    public function image()
    {
        $_ = [
            $this->_node('url', $this->_data['image']),
            $this->_node('title', $this->_data['title']),
            $this->link(),
        ];

        return $this->_node('image', $_);
    }

    public function xml($items)
    {
        $_ = [
            $this->nodeSet($this->nodeNames),
        ];

        foreach ($items as $i) {
            $i['pubDate'] = $this->rss->rssDateFormat($i['pubDate']);

            $itemO = new RssItem($i);
            $_[] = $itemO->xml();
        }


        return $this->_node('channel', $_);
    }

}
