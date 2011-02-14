<?php

class PXZoomFilter {
	public static function run($html, $layout) {
		$vars = $layout->smarty->get_template_vars();
		$keys = array_keys($vars);

		extract($vars, EXTR_PREFIX_ALL, 'lt');

		if (!(!empty($lt_currentCtype) && !empty($lt_currentCid) && $lt_currentCid != -1 &&  $lt_currentCtype != -1)) {
			return $html;
		}

		return self::wrapImages(self::makePhotoGallery($html));
	}

	private static function wrapImages($text) {
		preg_match_all("/<[^>]*img[^>]*class=\"imgwrap\"[^>]*>/is", $text, $res);

		$res = array_unique($res[0]);

		foreach ($res as $img) {
			if (strstr($img, 'rel="nowrap"')) continue;
			$text = str_replace($img, '<div class="descr-box"><div class="shadow-box"><div class="sh-t"></div><div class="sh-tr"></div><div class="l"></div><div class="r"></div><span class="sh-in">'.$img.'</span><div class="sh-b"></div><div class="sh-br"></div></div></div>', $text);
		}

		return $text;
	}

	private static function makePhotoGallery($html){
		return preg_replace("/(?:<|&lt;)!--#zoom\|([^#|]+)\|([^#|]+)(?:\|(\d+)x(\d+))?(?:\|(nowrap))?#--(?:>|&gt;)/i", <<<HTML
			<a href="$2" rel="zoombox">
				<img src="$1" class="imgwrap" style="width:$3px;height:$4px" rel="$5" />
			</a>
HTML
		, $html);
	}
}

?>
