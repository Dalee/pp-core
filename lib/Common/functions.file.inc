<?
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

function unlinkDir($source) {
	// Simple copy for a file
	if (is_file($source)) {
		return unlink($source);
	}

	// Loop through the folder
	$dir = dir($source);
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

function smarty_file_exists($filename) {
	return file_exists(BASEPATH.'/templates/'.$filename);
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
		}
		fclose($fd);
	} else {
		FatalError("Ошибка при открытии файла <EM>".$fileName."</EM>");
	}
	return $content;
}

function ReadFileToUString($fileName) {
	if (!file_exists($fileName)) FatalError();
	return utf8_encode(ReadFileToString($fileName));
}

function WriteStringToFile($fileName, $string) {
	$fd = fopen($fileName, "w");
	if ($fd) {
		if (flock($fd, LOCK_EX) === true) {
			fwrite($fd, $string);
			flock($fd, LOCK_UN);
		} else {
			FatalError("Ошибка при записи в файл <EM>".$fileName."</EM> - невозможно заблокировать файл.");
		}
		fclose($fd);
	} else {
		FatalError("Ошибка при записи в файл <EM>".$fileName."</EM>");
	}
}

function AppendStringToFile($fileName, $string) {
	$fd = fopen($fileName, "a");
	if ($fd) {
		if (flock($fd, LOCK_EX) === true) {
			fwrite($fd, $string);
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
	function mime_content_type($file) {
		return exec ("file -bi " . escapeshellcmd($file));
	}
}

function MakeDirIfNotExists($dirName, $perm = NULL) {
	$dirName = preg_replace('|[/\\\\]+|', DIRECTORY_SEPARATOR, $dirName);
	$fullDirName = '';

	$dirPath = explode(DIRECTORY_SEPARATOR, $dirName);

	if(IS_WIN) {
		$fullDirName = array_shift($dirPath);
	}

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
	$strings = explode("\n", trim($source));

	$res = array();

	foreach($strings as $line=>$string) {
		$string = trim($string);

		if(!strlen($string)) {
			continue;
		}

		$elements = explode($delim, $string);

		for ($i = 0; $i < count($elements); $i++) {
			$nquotes = substr_count($elements[$i], '"');

			if ($nquotes %2 == 1) {
				for ($j = $i+1; $j < count($elements); $j++) {
					if (substr_count($elements[$j], '"') > 0) {
						// Put the quoted string's pieces back together again
						array_splice($elements, $i, $j-$i+1,
						implode($separator, array_slice($elements, $i, $j-$i+1)));
						break;
					}
				}
			}

			if ($nquotes > 0) {
				// Remove first and last quotes, then merge pairs of quotes
				$qstr =& $elements[$i];
				$qstr = substr_replace($qstr, '', strpos($qstr, '"'), 1);
				$qstr = substr_replace($qstr, '', strrpos($qstr, '"'), 1);
				$qstr = str_replace('""', '"', $qstr);
			}
		}

		if($withColumnNames) {
			if(!$line) {
				$columns = $elements;
				continue;
			}

			$tmp = array();
			foreach($columns as $pos=>$col) {
				$tmp[$col] = $elements[$pos];
			}

			$elements = $tmp;
		}

		$res[] = $elements;
	}

	return $res;
}

function parse_csv_file($filename, $delim=';') {
	if(!file_exists($filename)) {
		FatalError('Can\'t find '.$filename);
	}

	$result = array();

	$handle = fopen($filename, 'r');

	while (($tmp = fgetcsv($handle, 4096, $delim)) !== FALSE) {
		$result[] = $tmp;
	}

	fclose($handle);

	return $result;
}
?>
