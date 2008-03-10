<?
class PXModuleRSSEngine extends PXAbstractModule {
	function PXModuleRSSEngine($area, $settings) {
		$this->PXAbstractModule($area, $settings);
	}

	function _GetChannel() {
		return $channel;
	}

	function _GetItems() {
		return $this->items;
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, $currentCid, $currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {
		FatalError('It\'s interface method. ReWrite it.');
		exit;
	}

	function GetChannel($channel) {
		$xml  = "\n\t\t".'<title>'.$channel['title'].'</title>';
		$xml .= "\n\t\t".'<link>'.$channel['link'].'?from=rss</link>';
		$xml .= "\n\t\t".'<description>'.$channel['description'].'</description>';
		$xml .= "\n\t\t".'<language>'.$channel['language'].'</language>';
		$xml .= "\n\t\t".'<copyright>'.$channel['copyright'].'</copyright>';
		$xml .= "\n\t\t".'<managingEditor>'.$channel['managingEditor'].'</managingEditor>';
		$xml .= "\n\t\t".'<webMaster>'.$channel['webMaster'].'</webMaster>';

		$xml .= "\n\t\t".'<generator>'.$channel['generator'].'</generator>';
		$xml .= "\n\t\t".'<ttl>'.$channel['ttl'].'</ttl>';
		$xml .= "\n\t\t".'<image>'.
				"\n\t\t\t".'<url>'.$channel['image'].'?from=rss</url>'.
				"\n\t\t\t".'<title>'.$channel['title'].'</title>'.
				"\n\t\t\t".'<link>'.$channel['link'].'?from=rss</link>'.
			"\n\t\t".'</image>';

		return $xml;
	}

	function GetItem($i) {
		$xml  = "\n\t\t\t".'<title>'.$i['title'].'</title>';
		$xml .= "\n\t\t\t".'<link>'.$i['link'].'?from=rss</link>';
		$xml .= "\n\t\t\t".'<guid>'.$i['link'].'?from=rss</guid>';

		if (isset($i['category'])) {
			$xml .= "\n\t\t\t".'<category>'.$i['category'].'</category>';
		}

		if (isset($i['author'])) {
			$xml .= "\n\t\t\t".'<author>'.$i['author'].'</author>';
		}

		$xml .= "\n\t\t\t".'<description><![CDATA['.$i['description'].']]></description>';
		$xml .= "\n\t\t\t".'<pubDate>'.$this->rssDateFormat($i['pubDate']).'</pubDate>';

		return $xml;
	}

	function GetXML($channel, $items) {
		$lastBuildDate = reset($items);
		$lastBuildDate = $lastBuildDate['pubDate'];

		$xml  = '<?xml version="1.0" encoding="utf-8"?>';
		$xml .= "\n".'<rss version="2.0">';
		$xml .= "\n\t".'<channel>';

		$xml .= $this->GetChannel($channel);

		$xml .= "\n\t\t".'<lastBuildDate>'.$this->rssDateFormat($lastBuildDate).'</lastBuildDate>';

		foreach($items as $i) {
			$xml .= "\n\n\t\t".'<item>';
			$tmp = $this->GetItem($i);

			$tmp = preg_replace("/<a(.*?)href=\"\//i",  '<a$1href="'.$channel['link'].'/',  $tmp);
			$tmp = preg_replace("/<img(.*?)src=\"\//i", '<img$1src="'.$channel['link'].'/', $tmp);
			$xml .= $tmp;
			$xml .= "\n\t\t".'</item>';
		}

		$xml .= "\n\t".'</channel>';
		$xml .= "\n".'</rss>';

		header('Content-Type:text/xml;charset=utf-8');

		$xml = myconv('k', 'u', $xml);
		$xml = $this->unhtmlentities($xml);
		echo $xml;
		exit;
	}

	function unhtmlentities($string) {
		$trans_tbl = get_html_translation_table(HTML_ENTITIES);
		$trans_tbl = array_flip($trans_tbl);
		$trans_tbl['&mdash;']  = "&#8211;";
		$trans_tbl['&ndash;']  = "&#8212;";
		$trans_tbl['&hellip;'] = "&#8230;";

		foreach($trans_tbl as $k=>$v) {
			$trans_tbl[$k] = myconv('w', 'u', $v);
		}

		$string = strtr($string, $trans_tbl);
//		$string = preg_replace('/&#(\d+);/e', "chr(\\1)", $string);

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
}
?>
