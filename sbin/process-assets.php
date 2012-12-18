#!/usr/bin/env php5
<?php
	/**
	 * Proxima Portal CSS manager
	 *
	 *
	 */
	define('BASEPATH', rtrim(realpath(dirname(__FILE__).'/../../'), '/') . '/');
	define('WORKPATH', BASEPATH . 'site/htdocs');
	define('SOURCEPATH', BASEPATH . 'local/htdocs');

	set_time_limit(0);
	ini_set('memory_limit', '512M');
	umask(0);
	
	require_once (BASEPATH . 'libpp/lib/HTML/inlineimage.class.inc');
	require_once (BASEPATH . 'libpp/vendor/CSSMin/CssMin.php');
	require_once (BASEPATH . 'libpp/lib/Common/functions.file.inc');

	//
	function d2($var) {
		$s = print_r($var, true);
		echo $s."\n";
	}

	// 
	class DirectorySubtree {
		protected $rootPath = null;
		protected $data = null;

		public function __construct() {
		}

		public function build($rootPath) {
			$this->rootPath = $rootPath;
			$this->data = array();

			if (!file_exists($this->rootPath)) {
				return;
			}
			$this->data = $this->recursiveWalk($this->rootPath);
		}

		public function getFileList($wildCart) {
			$result = array();
			foreach($this->data as $_ => $fileData) {
				if (fnmatch($wildCart, $fileData['filename'])) {
					$result[] = $fileData;
				}
			}
			return $result;
		}

		protected function foundedFile($fileName, $fullPath) {
			$subdirPath = ltrim(substr($fullPath, strlen($this->rootPath)), '/');

			$diff = strlen($subdirPath);
			$subdirPath = trim(substr($subdirPath, 0, $diff - strlen($fileName)), '/');

			return array (
				'filename' => $fileName,
				'fullpath' => $fullPath,
				'relative' => $subdirPath,
				'mtime' => filemtime($fullPath),
			);
		}

		protected function recursiveWalk($path) {
			$dh = null;
			if(!($dh = opendir($path))) {
				return array();
			}

			
			$dirList = array();
			$fileList = array();

			while (($fileName = readdir($dh))) {
				if ($fileName === '.' || $fileName === '..') {
					continue;
				}
				if (substr($fileName, 0, 1) === '.') {
					continue;
				}

				$fullPath = $path . '/' . $fileName;
				if (is_link($fullPath)) {
					continue;
				}
				if (is_file($fullPath)) { // ok, this is file, process and append him
					$fileList[] = $this->foundedFile($fileName, $fullPath);
				}

				if (is_dir($fullPath)) { // ok, this is directory
					$dirList[] = $fullPath;
				}
			}

			closedir($dh);
			foreach($dirList as $_ => $fullPath) {
				$dirFiles = $this->recursiveWalk($fullPath);
				if (!empty($dirFiles)) {
					$fileList = array_merge($fileList, $dirFiles);	
				}
			}

			unset($dirList);
			return $fileList;
		}

	}


	function buildDestinationFilePath($prefix, $fileData) {
		$destinationFileFormat = array();
		$destinationFileFormat[] = WORKPATH;
		$destinationFileFormat[] = $prefix;
		if (!empty($fileData['relative'])) {
			$destinationFileFormat[] = $fileData['relative'];
		}
		$destinationFileFormat[] = $fileData['filename'];
		$destinationFile = implode('/', $destinationFileFormat);
		return $destinationFile;
	}

	function buildTargetPathIfNeeded($prefix, $fileData) {
		$destinationDirFormat = array();
		$destinationDirFormat[] = WORKPATH;
		$destinationDirFormat[] = $prefix;
		if (!empty($fileData['relative'])) {
			$destinationDirFormat[] = $fileData['relative'];
		}
		$destinationDir = implode('/', $destinationDirFormat);
		if (!file_exists($destinationDir)) {
			if (!(mkdir($destinationDir, 0777, true))) {
				print ("Unable to create: {$destinationDir}");
				exit(1);
			}
		}
	}


	function isNeedProcessing($fileData, $destinationFile) {
		$srcTime = $fileData['mtime'];
		$dstTime = (file_exists($destinationFile)) ? filemtime($destinationFile) : 0;
		return ($dstTime < $srcTime);
	}

	function moveFileToTargetDir($tempFile, $destinationFile) {
		if (file_exists($destinationFile)) {
			unlink($destinationFile);	
		}
		
		chmod($tempFile, 0644);
		rename($tempFile, $destinationFile);
	}


	//
	$treePart = new DirectorySubtree();
	
	/**
	 * CSS processing
	 *
	 */
	$treePart->build(SOURCEPATH . '/css');
	$fileList = $treePart->getFileList('*.css');

	foreach($fileList as $_ => $fileData) {
		$sourceFile = $fileData['fullpath'];
		$destinationFile = buildDestinationFilePath('css', $fileData);

		if (!isNeedProcessing($fileData, $destinationFile)) {
			continue;
		}

		buildTargetPathIfNeeded('css', $fileData);
		$tempFile = tempnam(BASEPATH . 'tmp', 'css');
		print ("Processing {$sourceFile} thru {$tempFile} to {$destinationFile}\n");
		$resultData = CssMin::minify(
			file_get_contents($sourceFile),
			array(),
			array('ConvertImageUrl' => true)
		);

		if(!empty($resultData)) {
			if (file_put_contents($tempFile, $resultData)) {
				moveFileToTargetDir($tempFile, $destinationFile);
			}
		}
	}


	/**
	 * Javascript processing
	 *
	 */
	$treePart->build(SOURCEPATH . '/js');
	$fileList = $treePart->getFileList('*.js');

	foreach($fileList as $_ => $fileData) {
		$sourceFile = $fileData['fullpath'];
		$destinationFile = buildDestinationFilePath('js', $fileData);

		if (!isNeedProcessing($fileData, $destinationFile)) {
			continue;
		}

		buildTargetPathIfNeeded('js', $fileData);
		$tempFile = tempnam(BASEPATH . 'tmp', 'js');
		print ("Processing {$sourceFile} thru {$tempFile} to {$destinationFile}\n");

		if (PXHtmlImageTag::getInstance()->getProperty('CONFIG.ASSETS_USE_YUI') && 
			($compressorBinary = FindSystemFile('yui-compressor'))) {
			
			$outputFile = tempnam($bundleTypeRoot, $bundleName);
			$cmd = array();
			$cmd[] = $compressorBinary; // /usr/bin/yui-compressor
			$cmd[] = $sourceFile;   // .../asset-....(js|css)
			$cmd[] = "--type js";
			$cmd[] = "-o";
			$cmd[] = $tempFile;

			$cmd_string = implode(' ', $cmd);
			$statusCode = -1;
			$compressionResult = exec($cmd_string, $dummy, $statusCode);

			if((intval($statusCode) === 0)) {
				moveFileToTargetDir($tempFile, $destinationFile);
			} else {
				unlink($tempFile);
			}
		}
	}	
?>