<?php
/**
 * Project:     Dalee PHP-classes library
 * File:        captcha.class.inc
 *
 * Генератор captcha изображений
 *
 * @copyright   Dalee, 2003-2010
 * @author      Stanislav Tugovikov <st@dalee.ru>
 * @author      Andrey Nikolaev <andrey.nikolaev@dalee.ru>
 * @author      Alexey Ten <aten@dalee.ru>
 * @package     ProximaPortal
 * @subpackage  Security
 */
class NLCaptcha {
	/**
	 * Конструктор
	 *
	 * @param String             $text
	 * @param Array              $fonts
	 * @param Array              $colors
	 *
	 * @return
	 */
	function NLCaptcha($text, $width, $height, $fonts, $colors, $withLines=true, $fontSize=26) {
		$this->colors = array();
		foreach($colors as $ci=>$c) {
			$tmp = explode('|', chunk_split($c,    2, '|'));
			foreach($tmp as $k=>$v) {
				$tmp[$k] = hexdec($v);
			}

			$this->colors[$ci] = $tmp;

		}

		$this->width  = $width;
		$this->height = $height;

		$this->init();                                // первые шаги
		//$this->addNoise();                            // добавить шум
		$this->addText($text, $fonts, $fontSize);     // вывести надпись

		if($withLines) {
			$this->addLines();                    // добавить случайные линии
		}

		$this->showImage();                           // вывести изображение в браузер
	}

	function init() {
		mt_srand((double)microtime()*1000000000);

		// создание "хоста"
		$im = @ImageCreate($this->width, $this->height) or FatalError('Cannot Initialize new GD image stream');

		$ica = array();

		// заполнение таблицы индексов цветов
		$rev = array();
		$ica[0]   = ImageColorAllocate($im, $this->colors['lite'][0], $this->colors['lite'][1], $this->colors['lite'][2]);
		$rev[$ica[0]] = 0;
		$ica[127] = ImageColorAllocate($im, $this->colors['dark'][0], $this->colors['dark'][1], $this->colors['dark'][2]);

		for ($i = 0; $i < 10; $i++) {
			$t = ($this->colors['dark'][1] + mt_rand(0, 32));

			$ica[$i+1]   = ImageColorAllocate($im, $this->colors['dark'][0], $t, $this->colors['dark'][2]);
			$rev[$ica[$i+1]] = $i + 1;
			$ica[$i+128] = ImageColorAllocate($im, 300 - $this->colors['dark'][0], 300 - $t, 300 - $this->colors['dark'][2]);

			$rev[$ica[$i+128]] = $i + 128;
		}

		// заливка фона
		ImageFilledRectangle($im, 0, 0, $this->width, $this->height, $ica[0]);

		// веса шума, линий и .т.п.
		$noiseRatio = mt_rand(80,150)/100;
		$linesRatio = 2 - $noiseRatio;
		$fontRatio  = $linesRatio > 1 ? ($linesRatio < 1.5 ? $linesRatio : 1) : 1;// max($linesRatio, 1);

		$this->ratio = array(
			'noise' => $noiseRatio,
			'lines' => $linesRatio,
			'font'  => $fontRatio
		);


		$this->im  = $im;
		$this->ica = $ica;
		$this->rev = $rev;
	}

	function addNoise() {
		for ($i=0; $i < 500 * $this->ratio['noise']; $i++) {
			$dx = mt_rand(0, $this->width  - 1);
			$dy = mt_rand(0, $this->height - 1);

			ImageSetPixel($this->im, $dx, $dy, $this->ica[$i%10 + 1]);
		}
	}

	function addText($text, $fonts, $fSize) {
		$fontRatio = $this->ratio['font'];

		for ($i=0; $i<strlen($text); $i++) {
			$size  =  mt_rand($fSize,$fSize*1.15) * $fontRatio;
			$dx    =  $i*$fSize + mt_rand(4, 6);
			$dy    = ($this->height > 3*$fSize) ? 20 + mt_rand(15, 45) / $fontRatio : mt_rand($fSize*1.17, $fSize*1.5);

			$angle = mt_rand(-10,10) / $fontRatio;

			ImageTTFText($this->im, $size, $angle,  $dx,  $dy, $this->ica[mt_rand(1, 10)], $fonts[mt_rand(0,sizeof($fonts) - 1 )], substr($text, $i, 1));
		}


/*
		for ($i=0; $i<strlen($text); $i++) {
			$size  = 23;
			$dx    = $i*23+5+mt_rand(0, 5);
			$dy    = mt_rand(25, 34);
			$angle = (mt_rand(0,1)*2-1) * mt_rand(5, 10);

			ImageTTFText($this->im, $size, $angle,  $dx,  $dy, $this->ica[mt_rand(1, 10)], $fonts[mt_rand(0,sizeof($fonts) - 1 )], substr($text, $i, 1));
		}
*/
	}

	function addLines() {
		$im = $this->im;
		$ica = $this->ica;
		$rev = $this->rev;
		$x   = $this->width;
		$y   = $this->height;

		// square boxes
		if ( mt_rand(0,10) > 5) {
			//vertical
			$p1 = -10 + mt_rand(10, 20);
			$p2 = $p1 + mt_rand(15, 20);

			for ($i = 0; $i < mt_rand(1,2); $i++) {
				$this->imageLineThick($im, 0 , mt_rand(0,$y), $x, mt_rand(0, $y), $ica[mt_rand(1,10)], mt_rand(2,3));
			}

			while ($p2 <= $x) {
				for ($i = $p1; $i < $p2; $i++) {
					for ($j = 0; $j < $y; $j++) {
						$k = ImageColorAt($im, $i, $j);
						if (isset($rev[$k])) $m = $rev[$k]; else $m = 0;
						ImageSetPixel($im, $i, $j, $ica[127 + $m]);
					}
				}
				$p1 = $p2 + mt_rand(15, 20);
				$p2 = $p1 + mt_rand(15, 20);
			}

		} else {

			//horisontal
			$p1 = -5 + mt_rand(5, 20);
			$p2 = $p1 + mt_rand(5, 20);

			for ($i = 0; $i < mt_rand(1,2); $i++) {
				$this->ImageLineThick($im, mt_rand(0,$x), 0, mt_rand(0, $x), $y, $ica[mt_rand(1,10)], mt_rand(2,3));
			}

			while ($p2 <= $y) {
				for ($i = $p1; $i < $p2; $i++) {
					for ($j = 0; $j < $x; $j++) {
						$k = ImageColorAt($im, $j, $i);
						if (isset($rev[$k])) $m = $rev[$k]; else $m = 0;
						ImageSetPixel($im, $j, $i, $ica[127 + $m]);
					}
				}
				$p1 = $p2 + mt_rand(5, 20);
				$p2 = $p1 + mt_rand(5, 20);
			}
		}
	}

	function showImage() {
		require_once 'libpp/lib/response.class.inc';
		$response = PXResponse::getInstance();
		$response->dontCache();
		$response->setContentType('image/jpeg');

		$response->send();
		ImageJpeg($this->im, '', mt_rand(60,90));
		ImageDestroy($this->im);
	}

	/*
	 * ripped from comment at http://ru.php.net/imageline
	 *
	 * this way it works well only for orthogonal lines
	 * imagesetthickness($image, $thick);
	 * return imageline($image, $x1, $y1, $x2, $y2, $color);
	 */
	function imageLineThick($image, $x1, $y1, $x2, $y2, $color, $thick = 1) {
		if ($thick == 1) {
			return ImageLine($image, $x1, $y1, $x2, $y2, $color);
		}

		$t = $thick / 2 - 0.5;

		if ($x1 == $x2 || $y1 == $y2) {
			return ImageFilledRectangle($image, round(min($x1, $x2) - $t), round(min($y1, $y2) - $t), round(max($x1, $x2) + $t), round(max($y1, $y2) + $t), $color);
		}

		$k = ($y2 - $y1) / ($x2 - $x1); //y = kx + q
		$a = $t / sqrt(1 + pow($k, 2));

		$points = array(
			round($x1 - (1+$k)*$a), round($y1 + (1-$k)*$a),
			round($x1 - (1-$k)*$a), round($y1 - (1+$k)*$a),
			round($x2 + (1+$k)*$a), round($y2 - (1-$k)*$a),
			round($x2 + (1-$k)*$a), round($y2 + (1+$k)*$a),
		);

		ImageFilledPolygon($image, $points, 4, $color);

		return ImagePolygon($image, $points, 4, $color);
	}
}
?>
