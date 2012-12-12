#!/usr/bin/env php5
<?php
	/**
	 * Proxima Portal CSS manager
	 *
	 *
	 */
	define('BASEPATH', rtrim(realpath(dirname(__FILE__).'/../../'), '/') . '/');
	define('WORKPATH', BASEPATH . 'site/htdocs/css');
	set_time_limit(0);
	ini_set('memory_limit', '512M');
	
	require_once (BASEPATH . 'libpp/lib/HTML/inlineimage.class.inc');
	require_once (BASEPATH . 'libpp/vendor/CSSMin/CssMin.php');

	//
	function d2($var) {
		$s = print_r($var, true);
		echo $s."\n";
	}

	//
	$fileList = glob(BASEPATH . 'local/htdocs/css/*.css');
	foreach($fileList as $_ => $sourceFile) {
		$destinationFile = WORKPATH . '/' . basename($sourceFile);

		$srcTime = filemtime($sourceFile);
		$dstTime = 0;
		if (file_exists($destinationFile)) {
			$dstTime = filemtime($destinationFile);	
		}

		if($dstTime >= $srcTime) {
			continue;
		}

		$tempFile = tempnam(BASEPATH . 'tmp', 'css');
		print ("Processing {$sourceFile} thru {$tempFile} to {$destinationFile}\n");
		$resultData = CssMin::minify(
			file_get_contents($sourceFile),
			array(),
			array('ConvertImageUrl' => true)
		);

		if(!empty($resultData)) {
			if (file_put_contents($tempFile, $resultData)) {
				if (file_exists($destinationFile)) {
					unlink($destinationFile);	
				}
				chmod($tempFile, 0644);
				rename($tempFile, $destinationFile);
				print ("Successfully processed: {$sourceFile} => {$destinationFile}\n");
			}
		}
	}
?>