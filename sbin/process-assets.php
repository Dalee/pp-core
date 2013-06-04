#!/usr/bin/env php5
<?php
	/**
	 * Proxima Portal CSS manager
	 *
	 *
	 */
	define('BASEPATH',   rtrim(realpath(dirname(__FILE__).'/../../'), '/') . '/');
	define('PPPATH',     BASEPATH . 'libpp/');
	define('LOCALPATH',  BASEPATH . 'local/');
	define('SHAREDPATH', BASEPATH . 'site/');
	define('WORKPATH',   BASEPATH . 'local/htdocs');
	define('SOURCEPATH', BASEPATH . 'local/htdocs');
	define('DEBUGMODE',  false);

	set_time_limit(0);
	ini_set('memory_limit', '512M');
	umask(0);

	// require_once (BASEPATH . 'libpp/lib/common.defines.inc');
	require_once (BASEPATH . 'libpp/lib/HTML/inlineimage.class.inc');
	require_once (BASEPATH . 'libpp/vendor/CSSMin/CssMin.php');
	require_once (BASEPATH . 'libpp/lib/Common/functions.pp.inc');
	require_once (BASEPATH . 'libpp/lib/Common/functions.file.inc');
	require_once (BASEPATH . 'libpp/lib/Common/functions.etc.inc');

	//
	function d2($var) {
		$s = print_r($var, true);
		echo $s."\n";
	}
	function FatalError($a) {
		echo 'FatalError: ' . $a . PHP_EOL;
		die(1);
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
		$destinationFile = implode('/', array_filter($destinationFileFormat));
		return $destinationFile;
	}

	function buildTargetPathIfNeeded($prefix, $fileData) {
		$destinationDir = pathinfo(buildDestinationFilePath($prefix, $fileData), PATHINFO_DIRNAME);
		if (!file_exists($destinationDir)) {
			if (!(mkdir($destinationDir, 0777, true))) {
				print ("Unable to create: {$destinationDir}");
				exit(1);
			}
		}
	}


	function parseAssetsIgnoreFile ($ignoreString) {
		$lines = explode("\n", str_replace("\r", '', $ignoreString));
		$wildcards = array();
		foreach ($lines as $line) {
			// according to http://www.kernel.org/pub/software/scm/git/docs/gitignore.html
			// A line starting with # serves as a comment.
			if (strpos($line, '#') !== false) {
				$line = substr($line, 0, strpos($line, '#'));
			}
			$line = trim($line);
			// A blank line matches no files, so it can serve as a separator for readability.
			if (empty($line)) {
				continue;
			}

			$flag = 0;
			$wc = $line;

			// An optional prefix ! which negates the pattern; any matching file excluded by a previous pattern will become included again. If a negated pattern matches, this will override lower precedence patterns sources.
			$negate = ($wc[0] === '!');

			$wc = ltrim($wc, "\t !");

			// If the pattern ends with a slash, it is removed for the purpose of the following description, but it would only find a match with a directory. In other words, foo/ will match a directory foo and paths underneath it, but will not match a regular file or a symbolic link foo (this is consistent with the way how pathspec works in general in git).
			if (substr($wc, -1, 1) === '/') {
				$wc = $wc . '*';

			// If the pattern does not contain a slash /, git treats it as a shell glob pattern and checks for a match against the pathname relative to the location of the .gitignore file (relative to the toplevel of the work tree if not from a .gitignore file).
			} else if (strpos($wc, '/') === false) {
				// dummy

			// Otherwise, git treats the pattern as a shell glob suitable for consumption by fnmatch(3) with the FNM_PATHNAME flag: wildcards in the pattern will not match a / in the pathname. For example, "Documentation/*.html" matches "Documentation/git.html" but not "Documentation/ppc/ppc.html" or "tools/perf/Documentation/perf.html".
			} else {
				$flag = FNM_PATHNAME;
			}

			// A leading slash matches the beginning of the pathname. For example, "/*.c" matches "cat-file.c" but not "mozilla-sha1/sha1.c".
			$wc = (substr($wc, 0, 1) === '/') ? ($wc) : ('*' . $wc);

			$pattern = $wc;
			$wildcards[] = compact('line', 'pattern', 'flag', 'negate');
		}
		if (DEBUGMODE) {
			var_dump($wildcards);
		}
		return $wildcards;
	}

	function isNeedProcessing($fileData, $sourceFile, $destinationFile) {
		static $except = null;
		($except === null) && $except = file_exists(BASEPATH . '.assetsignore')
			? parseAssetsIgnoreFile(file_get_contents(BASEPATH . '.assetsignore'))
			: array();

		// todo: make it as close as gitignore works
		$relDestinationFile = preg_replace('@^'.BASEPATH.'/?[^/]+/htdocs/@', '/', $destinationFile);
		foreach ($except as $wildcard) {
			if (fnmatch($wildcard['pattern'], $relDestinationFile, $wildcard['flag'])) {
				if (DEBUGMODE) {
					echo 'skip '.$relDestinationFile." by wildcard ".json_encode($wildcard). PHP_EOL;
					return false; //isset($wildcard['negate']) && $wildcard['negate'];
				}
				return (isset($wildcard['negate']) && $wildcard['negate']);
			}
		}
		if (DEBUGMODE) {
			echo 'processing ' . $relDestinationFile . PHP_EOL;
			return false;
		}
		return true;
	}


	function moveFileToTargetDir($tempFile, $destinationFile) {
		if (!file_exists($tempFile) || !strlen(file_get_contents($tempFile))) {
			unlink($tempFile);
			echo "ALARM: Empty file in result of processing " . $destinationFile . PHP_EOL;
			return false;
		}

		if (file_exists($destinationFile)) {
			unlink($destinationFile);
		}

		chmod($tempFile, 0644);
		rename($tempFile, $destinationFile);

		return true;
	}

	// add protection code
	// 
	$isProtectionDisabled = (isset($argv[1]) && strcmp($argv[1], 'i_am_chosen_one') == 0);

	if (!$isProtectionDisabled) {
		print ("WARNING! ALARMA!\n");
		print ("This script modifying content of local/htdocs dir\n");
		print ("If you really know what are you doing, run this script with:\n");
		print ("i_am_chosen_one parameter.\n\n");
		exit(1);
	}

	// begin processing
	$treePart = new DirectorySubtree();

	/**
	 * CSS processing
	 *
	 */
	$treePart->build(SOURCEPATH);
	$fileList = (array)$treePart->getFileList('*.css');

	foreach ($fileList as $_ => $fileData) {
		$sourceFile = $fileData['fullpath'];
		$destinationFile = buildDestinationFilePath(null, $fileData);

		if (!isNeedProcessing($fileData, $sourceFile, $destinationFile)) { // filter by .assetsignore
			continue;
		}

		MakeDirIfNotExists(pathinfo($destinationFile, PATHINFO_DIRNAME));
		$tempFile = tempnam(BASEPATH . 'tmp', 'css');
		print ("Processing {$sourceFile} thru {$tempFile} to {$destinationFile}\n");
		$resultData = CssMin::minify(
			file_get_contents($sourceFile),
			array(),
			array('ConvertImageUrl' => compact('sourceFile'))
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
	 * @todo: add support of '/blocks/*?/*?.css files
	 */
	$treePart->build(SOURCEPATH);
	$fileList = (array)$treePart->getFileList('*.js');

	foreach ($fileList as $_ => $fileData) {
		$sourceFile = $fileData['fullpath'];
		$destinationFile = buildDestinationFilePath('', $fileData);

		if (!isNeedProcessing($fileData, $sourceFile, $destinationFile)) {
			continue;
		}

		MakeDirIfNotExists(pathinfo($destinationFile, PATHINFO_DIRNAME));
		$tempFile = tempnam(BASEPATH . 'tmp', 'js');
		print ("Processing {$sourceFile} thru {$tempFile} to {$destinationFile}\n");

		if (PXHtmlAssets::getInstance()->assets_yui && 
			($compressorBinary = FindSystemFile('yui-compressor'))) {

			// wtf? $outputFile = tempnam($bundleTypeRoot, $bundleName);
			$cmd = array();
			$cmd[] = $compressorBinary; // /usr/bin/yui-compressor
			$cmd[] = $sourceFile;   // .../asset-....(js|css)
			$cmd[] = "--type js";
			$cmd[] = "--charset=koi8-r";
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
