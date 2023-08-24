<?php
function copyr($source, $dest) {
	/**
	 * Copy a file, or a folder and its contents
	 *
	 * @author      Aidan Lister <aidan@php.net>
	 * @version     1.0
	 * @param       string   $source    The source
	 * @param       string   $dest      The destination
	 * @return      bool     Returns true on success, false on failure
	*/

	// Simple copy for a file
	if (is_file($source)) {
		return copy($source, $dest);
	}

	// Make destination directory
	if (!is_dir($dest)) {
		mkdir($dest);
	}

	// Loop through the folder
	$dir = dir($source);
	while (false !== $entry = $dir->read()) {
		// Skip pointers
		if ($entry == '.' || $entry == '..') {
			continue;
		}

		// Deep copy directories
		if (is_dir("$source/$entry") && ($dest !== "$source/$entry")) {
			copyr("$source/$entry", "$dest/$entry");
		} else {
			copy("$source/$entry", "$dest/$entry");
		}
	}
	// Clean up
	$dir->close();

	return true;
}

/**
 * Calls a $callback function for every file in a directory and its
 * subdirectories.
 *
 * The callback function receives 2 parameters:
 * 1) file path (ex.: /path/to/file/my.txt);
 * 2) file name (ex.: my.txt).
 */
function mapDir($dir, $callback) {
	if (!is_dir($dir)) {
		return false;
	}
	$fileNames = scandir($dir);
	foreach ($fileNames as $fileName) {
		if ($fileName == "." || $fileName == "..") {
			continue;
		}
		$filePath = $dir . "/" . $fileName;
		if (is_dir($filePath)) {
			mapDir($filePath, $callback);
		} else {
			call_user_func($callback, $filePath, $fileName);
		}
	}
	return true;
}

function unlinkDir($source) {
	// Simple copy for a file
	if (is_file($source)) {
		return unlink($source);
	}

	// Loop through the folder
	$dir = dir($source);

	if ($dir == false) {
		return;
	}

	while (false !== $entry = $dir->read()) {
		// Skip pointers
		if ($entry == '.' || $entry == '..') {
			continue;
		}

		// Deep copy directories
		if (is_dir("$source/$entry")) {
			unlinkDir("$source/$entry");
		} else {
			unlink("$source/$entry");
		}
	}
	// Clean up
	$dir->close();
	rmdir($source);

	return true;
}

function unlinkDirTreeIfEmpty($source) {
	if (!file_exists($source)) {
		return;
	}

	// loop thru the folder
	$dir = dir($source);

	if (!$dir) {
		return;
	}

	$empty = true;

	while (false !== ($entry = $dir->read())) {
		// skip pointers
		if ($entry == '.' || $entry == '..') {
			continue;
		}
		$entryfullpath = "$source/$entry";

		// unlink empty children if possible
		$empty = is_dir($entryfullpath) ? unlinkDirTreeIfEmpty($entryfullpath) : false;
	}

	// cleanup
	$dir->close();
	($empty === true) && rmdir($source);

	return $empty;
}

function smarty_file_exists($filename) {
	foreach(['site', 'local', 'libpp'] as $level) {
		if(file_exists(BASEPATH.'/'.$level.'/templates/'.$filename)) {
			return true;
		}
	}

	return false;
}

function is_binary($link) {
	$tmpStr  = '';

	@$fp    = fopen($link, 'rb');
	@$tmpStr = fread($fp, 256);
	@fclose($fp);

	if($tmpStr != '') {
		$characters = count_chars($tmpStr, 1);

		foreach($characters as $k=>$v) {
			if($k!=9 && $k!=10 && $k!=13 && ($k<32 || $k>255)) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

function ReadFileToString($fileName) {

	$content = null;

	if (!file_exists($fileName)) FatalError("Попытка открытия несуществующего файла <EM>".$fileName."</EM>");
	$fd = fopen($fileName, "r");
	if ($fd) {
		$fsize = filesize($fileName);
		if ($fsize > 0 && flock($fd, LOCK_SH) === true) {
			$content = fread($fd, $fsize);
			flock($fd, LOCK_UN);
		}
		fclose($fd);
	} else {
		FatalError("Ошибка при открытии файла <EM>".$fileName."</EM>");
	}
	return $content;
}

function ReadFileToUString($fileName) {
	if (!file_exists($fileName)) FatalError();
	return mb_convert_encoding((string) ReadFileToString($fileName), 'UTF-8', 'ISO-8859-1');
}

function WriteStringToFile($fileName, $string) {
	$fd = fopen($fileName, "w");
	if ($fd) {
		if (flock($fd, LOCK_EX) === true) {
			fwrite($fd, (string) $string);
			flock($fd, LOCK_UN);
		} else {
			FatalError("Ошибка при записи в файл <EM>".$fileName."</EM> - невозможно заблокировать файл.");
		}
		fclose($fd);
	} else {
		FatalError("Ошибка при записи в файл <EM>".$fileName."</EM>");
	}
}

/**
 * @deprecated use Monolog instead
 * @param $fileName
 * @param $string
 */
function AppendStringToFile($fileName, $string) {
	$fd = fopen($fileName, "a");
	if ($fd) {
		if (flock($fd, LOCK_EX) === true) {
			fwrite($fd, (string) $string);
			flock($fd, LOCK_UN);
		} else {
			FatalError("Ошибка при записи в файл <EM>".$fileName."</EM> - невозможно заблокировать файл.");
		}
		fclose($fd);
	} else {
		FatalError("Ошибка при записи в файл <EM>".$fileName."</EM>");
	}
}

if (!function_exists("mime_content_type")) {
	function mime_content_type($file, $type = 'mime') { //without Fileinfo extension constants FILEINFO_* are undefined
		if(function_exists("finfo_file") && file_exists('/etc/magic')) {
			$tmp = finfo_file(finfo_open(constant('FILEINFO_' . mb_strtoupper((string) $type)), '/etc/magic'), $file);
		} else {
			$tmp = exec(sprintf("file -bi '%s'", escapeshellcmd($file)));
		}
		return $tmp;
	}
}

function MakeDirIfNotExists($dirName, $perm = NULL) {
	$dirName = preg_replace('|[/\\\\]+|'.REGEX_MOD, DIRECTORY_SEPARATOR, (string) $dirName);
	$fullDirName = '';

	$dirPath = explode(DIRECTORY_SEPARATOR, $dirName);

	foreach($dirPath as $subDirName) {
		$fullDirName .= DIRECTORY_SEPARATOR.$subDirName;

		if (!file_exists($fullDirName)) {
			if($perm)  {
				if(!mkdir($fullDirName, $perm)) {
					FatalError('Не могу создать каталог '.$fullDirName.' с правами '.$perm);
				}

			} else {
				if(!mkdir($fullDirName)) {
					FatalError('Не могу создать каталог '.$fullDirName);
				}
			}
		}
	}
}

function parse_csv_string($source, $delim=';', $withColumnNames=false) {
	$strings = explode("\n", trim((string) $source));

	$res = [];

	foreach($strings as $line=>$string) {
		$string = trim($string);

		if(!mb_strlen($string)) {
			continue;
		}

		$elements = explode($delim, $string);

		for ($i = 0; $i < count($elements); $i++) {
			$nquotes = substr_count((string) $elements[$i], '"');

			if ($nquotes %2 == 1) {
				for ($j = $i+1; $j < count($elements); $j++) {
					if (substr_count((string) $elements[$j], '"') > 0) {
						// Put the quoted string's pieces back together again
						array_splice($elements, $i, $j-$i+1,
						implode($delim, array_slice($elements, $i, $j-$i+1)));
						break;
					}
				}
			}

			if ($nquotes > 0) {
				// Remove first and last quotes, then merge pairs of quotes
				$qstr =& $elements[$i];
				$qstr = substr_replace((string) $qstr, '', mb_strpos((string) $qstr, '"'), 1);
				$qstr = substr_replace($qstr, '', mb_strrpos($qstr, '"'), 1);
				$qstr = str_replace('""', '"', $qstr);
			}
		}

		if($withColumnNames) {
			if(!$line) {
				$columns = $elements;
				continue;
			}

			if (!empty($columns)) {
				$tmp = [];
				foreach ($columns as $pos => $col) {
					$tmp[$col] = $elements[$pos];
				}

				$elements = $tmp;
			}
		}

		$res[] = $elements;
	}

	return $res;
}

function parse_csv_file($filename, $delim=';') {
	if(!file_exists($filename)) {
		FatalError('Can\'t find '.$filename);
	}

	$result = [];

	$handle = fopen($filename, 'r');

	while (($tmp = fgetcsv($handle, 4096, $delim)) !== FALSE) {
		$result[] = $tmp;
	}

	fclose($handle);

	return $result;
}

function FindSystemFile($fileName){
	$pathArray = explode(':', (string) $_SERVER['PATH']);
	if (!in_array('/usr/X11R6/bin/', $pathArray) && !in_array('/usr/X11R6/bin', $pathArray)) {
		$pathArray[] = '/usr/X11R6/bin/';
	}
	foreach ($pathArray as $path) {
		if (is_executable($path.'/'.$fileName)) {
			return $path.'/'.$fileName;
		}
	}
}

/**
 * Determines mime type of file
 * @param string $filename
 * @return string
 * @throws Exception
 */
function fileMimeType ($filename) {
	if (is_callable('mime_content_type')) {
		[$mime] = explode(";", mime_content_type($filename));
	}
	elseif (is_callable('finfo_file')) {
		$finfo = finfo_open(FILEINFO_MIME_TYPE);
		$mime = finfo_file($finfo, $filename);
		finfo_close($finfo);
	}
	else {
		// @todo probably need to change to $mime = false
		throw new Exception ('Can\'t determine mime type at '.__FILE__.':'.__LINE__);
	}
	return $mime;
}

/**
 * Parsing file size
 * @params string $filesize
 * @params integer [$decimals=0]
 * @params array [$measures]
 * @test [1555] >>> {"size":1.52,"step":1,"measure":"KB","pretty":"2 KB"}
 * @test [1555, 2] >>> {"size":1.52,"step":1,"measure":"KB","pretty":"1.52 KB"}
 */
function parseFilesize ($filesize, $decimals = 0, $measures = null) {
	$measures = (array)($measures ?: null) + ['B', 'KB', 'MB', 'GB', 'TB', 'PB'];

	$size = $filesize;
	$step = 0;

	while ($size > 1024) {
		$size /= 1024;
		$step += 1;
	}

	$measure = $measures[$step];
	$pretty = number_format($size, $decimals, '.', '') . ' ' . $measure;
	$size = round($size, 2);

	return compact('size', 'step', 'measure', 'pretty');
}

/**
 * Prettify file size
 * @params string $filesize
 * @params integer [$decimals=0]
 * @params array [$measures]
 * @test [1555] >>> "2 KB"
 * @test [1555, 2] >>> "1.52 KB"
 */
function prettifyFilesize ($filesize, $decimals = 0, $measures = null) {
	$parsed = parseFilesize ($filesize, $decimals, $measures);
	return $parsed['pretty'];
}
