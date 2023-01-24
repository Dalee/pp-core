<?php

namespace PP\Module;

use PP\Lib\Http\Response;
use PP\Lib\Rss\RssChannel;

/**
 * Class RssEngineModule.
 *
 * @package PP\Module
 */
class RssEngineModule extends AbstractModule
{

    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        $this->encoding = CHARSET_UTF8;
    }

    public function _GetItems()
    {
        return $this->items;
    }

    public function userIndex(): void
    {
        FatalError('It\'s interface method. ReWrite it.');
        exit;
    }

    public function unhtmlentities($string)
    {
        $trans_tbl = get_html_translation_table(HTML_ENTITIES);
        $trans_tbl = array_flip($trans_tbl);
        $trans_tbl['&mdash;'] = "&#8211;";
        $trans_tbl['&ndash;'] = "&#8212;";
        $trans_tbl['&rsquo;'] = "&#8217;";
        $trans_tbl['&ldquo;'] = "&#8220;";
        $trans_tbl['&rdquo;'] = "&#8221;";
        $trans_tbl['&hellip;'] = "&#8230;";

        $string = strtr($string, $trans_tbl);

        return $string;
    }

    public function date2time($string)
    {
        if ($string == 'today') {
            $time = mktime(0, 0, 0);

        } elseif ($string == 'month') {
            $time = mktime(0, 0, 0, date('n'), 1);

        } elseif ($string != '') {
            preg_match("/^(\d{2})\.(\d{2})\.(\d{4})\s+(\d{2}):(\d{2}):(\d{2})(?:\.\d+)?$/si", trim((string) $string), $date);
            $time = mktime($date[4], $date[5], $date[6], $date[2], $date[1], $date[3]);

        } else {
            $time = time();
        }

        return $time;
    }

    public function rssDateFormat($string)
    {
        return date('d M Y H:i:s O', $this->date2time($string));
    }

    public function GetXML($channel, $items): void
    {
        $lastBuildDate = reset($items);
        $channel['lastBuildDate'] = $this->rssDateFormat($lastBuildDate['pubDate']);

        $xml = $this->xml($channel, $items);

        $xml = preg_replace("/<a(.*?)href=\"\//i", '<a$1href="' . $channel['link'] . '/', (string) $xml);
        $xml = preg_replace("/<img(.*?)src=\"\//i", '<img$1src="' . $channel['link'] . '/', $xml);

        $xml = $this->unhtmlentities($xml);

        $response = Response::getInstance();
        $response->setOk();
        $response->setContentType('text/xml', $this->encoding);
        $response->send($xml);
        exit;
    }

    public function xml($channel, $items)
    {
        $channelO = new RssChannel($channel, $this);

        $_ = [];

        $_[] = '<?xml version="1.0" encoding="' . $this->encoding . '"?>';
        $_[] = '<rss version="2.0">';
        $_[] = $channelO->xml($items);
        $_[] = '</rss>';

        return implode("\n", $_);
    }
}
