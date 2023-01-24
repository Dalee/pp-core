<?php

use PP\Lib\Http\Response;

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
class NLCaptcha
{
	/**
	 * Конструктор
	 *
	 * @param String $text
	 * @param array $fonts
	 * @param array $colors
	 *
	 * @return
	 */
	public function __construct($text, $width, $height, $fonts, $colors, $withLines = true, $fontSize = 26)
	{
		$this->colors = [];
		foreach ($colors as $ci => $c) {
			$tmp = explode('|', chunk_split((string) $c, 2, '|'));
			foreach ($tmp as $k => $v) {
				$tmp[$k] = hexdec($v);
			}

			$this->colors[$ci] = $tmp;

		}

		$this->width = $width;
		$this->height = $height;

		$this->init();                                // первые шаги
		//$this->addNoise();                            // добавить шум
		$this->addText($text, $fonts, $fontSize);     // вывести надпись

		if ($withLines) {
			$this->addLines();                    // добавить случайные линии
		}

		$this->showImage();                           // вывести изображение в браузер
	}

	public function init()
	{
		mt_srand((double)microtime() * 1_000_000_000);

		// создание "хоста"
		$im = @ImageCreate($this->width, $this->height) or FatalError('Cannot Initialize new GD image stream');

		$ica = [];

		// заполнение таблицы индексов цветов
		$rev = [];
		$ica[0] = ImageColorAllocate($im, $this->colors['lite'][0], $this->colors['lite'][1], $this->colors['lite'][2]);
		$rev[$ica[0]] = 0;
		$ica[127] = ImageColorAllocate($im, $this->colors['dark'][0], $this->colors['dark'][1], $this->colors['dark'][2]);

		for ($i = 0; $i < 10; $i++) {
			$t = ($this->colors['dark'][1] + random_int(0, 32));

			$ica[$i + 1] = ImageColorAllocate($im, $this->colors['dark'][0], $t, $this->colors['dark'][2]);
			$rev[$ica[$i + 1]] = $i + 1;
			$ica[$i + 128] = ImageColorAllocate($im, 300 - $this->colors['dark'][0], 300 - $t, 300 - $this->colors['dark'][2]);

			$rev[$ica[$i + 128]] = $i + 128;
		}

		// заливка фона
		ImageFilledRectangle($im, 0, 0, $this->width, $this->height, $ica[0]);

		// веса шума, линий и .т.п.
		$noiseRatio = random_int(80, 150) / 100;
		$linesRatio = 2 - $noiseRatio;
		$fontRatio = $linesRatio > 1 ? ($linesRatio < 1.5 ? $linesRatio : 1) : 1;// max($linesRatio, 1);

		$this->ratio = [
			'noise' => $noiseRatio,
			'lines' => $linesRatio,
			'font' => $fontRatio,
		];


		$this->im = $im;
		$this->ica = $ica;
		$this->rev = $rev;
	}

	public function addNoise()
	{
		for ($i = 0; $i < 500 * $this->ratio['noise']; $i++) {
			$dx = random_int(0, $this->width - 1);
			$dy = random_int(0, $this->height - 1);

			ImageSetPixel($this->im, $dx, $dy, $this->ica[$i % 10 + 1]);
		}
	}

	public function addText($text, $fonts, $fSize)
	{
		$fontRatio = $this->ratio['font'];

		$text_len = mb_strlen((string) $text);
		for ($i = 0; $i < $text_len; $i++) {
			$size = random_int($fSize, $fSize * 1.15) * $fontRatio;
			$dx = $i * $fSize + random_int(4, 6);
			$dy = ($this->height > 3 * $fSize) ? 20 + random_int(15, 45) / $fontRatio : random_int($fSize * 1.17, $fSize * 1.5);

			$angle = random_int(-10, 10) / $fontRatio;

			ImageTTFText($this->im, $size, $angle, $dx, $dy, $this->ica[random_int(1, 10)], $fonts[random_int(0, sizeof($fonts) - 1)], mb_substr((string) $text, $i, 1));
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

	public function addLines()
	{
		$im = $this->im;
		$ica = $this->ica;
		$rev = $this->rev;
		$x = $this->width;
		$y = $this->height;

		// square boxes
		if (random_int(0, 10) > 5) {
			//vertical
			$p1 = -10 + random_int(10, 20);
			$p2 = $p1 + random_int(15, 20);

			for ($i = 0; $i < random_int(1, 2); $i++) {
				$this->imageLineThick($im, 0, random_int(0, $y), $x, random_int(0, $y), $ica[random_int(1, 10)], random_int(2, 3));
			}

			while ($p2 <= $x) {
				for ($i = $p1; $i < $p2; $i++) {
					for ($j = 0; $j < $y; $j++) {
						$k = ImageColorAt($im, $i, $j);
						if (isset($rev[$k])) $m = $rev[$k]; else $m = 0;
						ImageSetPixel($im, $i, $j, $ica[127 + $m]);
					}
				}
				$p1 = $p2 + random_int(15, 20);
				$p2 = $p1 + random_int(15, 20);
			}

		} else {

			//horisontal
			$p1 = -5 + random_int(5, 20);
			$p2 = $p1 + random_int(5, 20);

			for ($i = 0; $i < random_int(1, 2); $i++) {
				$this->ImageLineThick($im, random_int(0, $x), 0, random_int(0, $x), $y, $ica[random_int(1, 10)], random_int(2, 3));
			}

			while ($p2 <= $y) {
				for ($i = $p1; $i < $p2; $i++) {
					for ($j = 0; $j < $x; $j++) {
						$k = ImageColorAt($im, $j, $i);
						if (isset($rev[$k])) $m = $rev[$k]; else $m = 0;
						ImageSetPixel($im, $j, $i, $ica[127 + $m]);
					}
				}
				$p1 = $p2 + random_int(5, 20);
				$p2 = $p1 + random_int(5, 20);
			}
		}
	}

	public function showImage()
	{
		$response = Response::getInstance();
		$response->dontCache();
		$response->setContentType('image/jpeg');

		$response->sendStream(function () {
			ImageJpeg($this->im, null, random_int(60, 90));
			ImageDestroy($this->im);
		});
	}

	/*
	 * ripped from comment at http://ru.php.net/imageline
	 *
	 * this way it works well only for orthogonal lines
	 * imagesetthickness($image, $thick);
	 * return imageline($image, $x1, $y1, $x2, $y2, $color);
	 */
	public function imageLineThick($image, $x1, $y1, $x2, $y2, $color, $thick = 1)
	{
		if ($thick == 1) {
			return ImageLine($image, $x1, $y1, $x2, $y2, $color);
		}

		$t = $thick / 2 - 0.5;

		if ($x1 == $x2 || $y1 == $y2) {
			return ImageFilledRectangle($image, round(min($x1, $x2) - $t), round(min($y1, $y2) - $t), round(max($x1, $x2) + $t), round(max($y1, $y2) + $t), $color);
		}

		$k = ($y2 - $y1) / ($x2 - $x1); //y = kx + q
		$a = $t / sqrt(1 + $k ** 2);

		$points = [
			round($x1 - (1 + $k) * $a), round($y1 + (1 - $k) * $a),
			round($x1 - (1 - $k) * $a), round($y1 - (1 + $k) * $a),
			round($x2 + (1 + $k) * $a), round($y2 - (1 - $k) * $a),
			round($x2 + (1 - $k) * $a), round($y2 + (1 + $k) * $a),
		];

		ImageFilledPolygon($image, $points, 4, $color);

		return ImagePolygon($image, $points, 4, $color);
	}
}
