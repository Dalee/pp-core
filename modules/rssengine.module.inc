<?php
class PXModuleRSSEngine extends PXModule {
	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->encoding = CHARSET_UTF8;
	}

	function _GetChannel() {
		return $channel;
	}

	function _GetItems() {
		return $this->items;
	}

	function userIndex(){ //&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, $currentCid, $currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {
		FatalError('It\'s interface method. ReWrite it.');
		exit;
	}

	function unhtmlentities($string) {
		$trans_tbl = get_html_translation_table(HTML_ENTITIES);
		$trans_tbl = array_flip($trans_tbl);
		$trans_tbl['&mdash;']  = "&#8211;";
		$trans_tbl['&ndash;']  = "&#8212;";
		$trans_tbl['&rsquo;']  = "&#8217;";
		$trans_tbl['&ldquo;']  = "&#8220;";
		$trans_tbl['&rdquo;']  = "&#8221;";
		$trans_tbl['&hellip;'] = "&#8230;";

		foreach($trans_tbl as $k=>$v) {
			$trans_tbl[$k] = myconv(DEFAULT_CHARSET, $this->encoding, $v);
		}

		$string = strtr($string, $trans_tbl);

		return $string;
	}

	function date2time ($string) {
		if ($string == 'today') {
			$time = mktime(0,0,0);

		} elseif ($string == 'month') {
			$time = mktime(0,0,0,date('n'),1);

		} elseif ($string != '') {
			preg_match("/^(\d{2})\.(\d{2})\.(\d{4})\s+(\d{2}):(\d{2}):(\d{2})(?:\.\d+)?$/si", trim($string), $date);
			$time = mktime($date[4], $date[5], $date[6], $date[2], $date[1], $date[3]);

		} else {
			$time = time();
		}

		return $time;
	}

	function rssDateFormat($string) {
		return date('d M Y H:i:s O', $this->date2time($string));
	}

	function GetXML($channel, $items) {
		$lastBuildDate = reset($items);
		$channel['lastBuildDate'] = $this->rssDateFormat($lastBuildDate['pubDate']);

		$xml = $this->xml($channel, $items);

		$xml = preg_replace("/<a(.*?)href=\"\//i",  '<a$1href="'.$channel['link'].'/',  $xml);
		$xml = preg_replace("/<img(.*?)src=\"\//i", '<img$1src="'.$channel['link'].'/', $xml);

		$xml = myconv(DEFAULT_CHARSET, $this->encoding, $xml);
		$xml = $this->unhtmlentities($xml);

		$response = PXResponse::getInstance();
		$response->setOk();
		$response->setContentType('text/xml', $this->encoding);
		$response->send($xml);
		exit;
	}

	function xml($channel, $items) {
		$channelO = new PXRssChannel($channel, $this);

		$_ = array();

		$_[] = '<?xml version="1.0" encoding="'.$this->encoding.'"?>';
		$_[] = '<rss version="2.0">';
		$_[] = $channelO->xml($items);
		$_[] = '</rss>';

		return implode("\n", $_);
	}
}

class PXRssXML {
	function _node($nodeName, $value) {
		if(is_array($value)) {
			$value = implode("", $value);
		}

		return
<<<XML
	<$nodeName>$value</$nodeName>
XML;
	}

	function nodeSet($nodes) {
		$_ = array();

		foreach($nodes as $node) {
			if(method_exists($this, $node)) {
				$_[] = $this->$node();

			} elseif (isset($this->_data[$node])) {
				$_[] = $this->_node($node, $this->_data[$node]);
			}
		}

		return implode("\n", $_);
	}
}

class PXRssChannel extends PXRssXML {
	function PXRssChannel($channel, &$rssEngine) {
		$this->_data     =  $channel;
		$this->rss       =& $rssEngine;

		$this->nodeNames =  array('title', 'link', 'description', 'language', 'copyright', 'managingEditor', 'webMaster', 'generator', 'ttl', 'image', 'lastBuildDate');
	}

	function link() {
		return $this->_node('link', $this->_data['link'].'?from=rss');
	}

	function image() {
		$_ = array(
			$this->_node('url',   $this->_data['image']),
			$this->_node('title', $this->_data['title']),
			$this->link()
		);

		return $this->_node('image', $_);
	}

	function xml($items) {
		$_ = array(
			$this->nodeSet($this->nodeNames),
		);

		foreach($items as $i) {
			$i['pubDate'] = $this->rss->rssDateFormat($i['pubDate']);

			$itemO = new PXRssItem($i);
			$_[] = $itemO->xml();
		}


		return $this->_node('channel', $_);
	}
}

class PXRssItem extends PXRssXML {
	function PXRssItem($item) {
		$this->_data = $item;
		$this->nodeNames = array('title', 'link', 'guid', 'category', 'author', 'description', 'pubDate');
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
?>
