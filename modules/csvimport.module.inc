<?php
class PXModuleCsvImport extends PXModule {
	var $htmlEsc;
	var $settings;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->settings = $settings;
		$this->area = $area;
		
		$this->htmlEsc = array(
			chr(145) => '&lsquo;',
			chr(146) => '&rsquo;',
			chr(147) => '&ldquo;',
			chr(148) => '&rdquo;',
			chr(171) => '&laquo;',
			chr(187) => '&raquo;',
			chr(150) => '&#8211;', // &ndash;
			chr(151) => '&#8212;', // &mdash;
			chr(185) => '&#8470;', // russian number
			chr(133) => '&#8230;', // &hellip;
			chr(153) => '&#8482;'  // &trade;
		);
	}

	function adminIndex() {
		$layout  = $this->layout;
		$request = $this->request;
		$app     = $this->app;

		$layout->setOneColumn();
		$html = '';

		switch ($request->GetVar('status')) {
			case 'error':
				$html .= $this->_UploadTable();
				$html .= '<h1 class="error">Ошибка</h1>';
				$html .= '<p class="error">Импорт данных не состоялся.<BR>';

				switch($request->GetVar('error')) {
					case 'notupload':
						$html .= 'Файл не закачан';
						break;

					case 'baddata':
					case 'notdata':
						$html .= 'Файл не содержит корректных данных';
						break;
				}

				$html .= '</p>';
				break;

			case 'success':
				$html .= $this->_Success($request, $app, $db);
				break;

			default:
				$html .= $this->_UploadTable();
				break;
		}
		$layout->Assign("INNER.0.0", $html);
	}

	function _Success(&$request, &$app, &$db) {
		$html  = '<h2>Импорт данных &laquo;'.$app->types[$this->settings['datatype']]->title.'&raquo; успешно произведен</h2>';
		$html .= '<p>В базу внесено <strong>'.htmlspecialchars($request->GetVar('quantity')).'</strong> объектов.</p>';
		return $html;
	}

	function adminAction() {
		$request = $this->request;
		$app     = $this->app;
		$db      = $this->db;

		$redir_to = $this->buildAdminIndexUrl();

		if (($filename = $this->UploadFile($request->GetUploadFile('file')))) {
			$quantity = $this->ImportToDb($this->_GetCsvSource($filename), $app, $db, $request);

			if(is_numeric($quantity)) {
				$redir_to .= '&status=success&quantity='.$quantity;
			} else {
				$redir_to .= '&status=error&error='.$quantity;
			}

		} else {
			$redir_to .= '&status=error&error=notupload';
		}

		return $redir_to;
	}

	function ImportToDB($csv, &$app, &$db, &$request) {
		$objects = array();
		$fields  = array();
		$parent  = NULL;

		$format = $app->types[$this->settings['datatype']];

		foreach ($this->settings['field'] as $f) {
			list($k, $v) = explode('|', $f);

			if(trim($k) == 'parent') {
				$v = $app->GetProperty(trim($v));
				$parent = $v;
			}

			$fields[trim($k)] = trim($v);
		}

		foreach($csv as $ln=>$line) {
			if($ln === 1 && isset($this->settings['skipfirst']) && $this->settings['skipfirst'] === 'true') {
				continue;
			}

			$valid = 0;
			foreach($line as $d) {
				if(strlen($d)) $valid++;
			}

			if(!$valid) continue;

			$object = array();

			$this->constructObject($object, $fields, $line);

			$objects[] = $object;
		}

		$db->transactionBegin();

		$this->deleteOldObjects($db, $format, $parent);

		$db->addContentObjects($format, $objects);
		$db->transactionCommit();

		return count($objects);
	}
	
	function constructObject(&$object, $fields, $row){
		foreach($fields as $fk=>$fv) {
			if($fk === 'parent') {
				$object[$fk] = $fv;
			} elseif(is_numeric($fv)) {
				$object[$fk] = $row[$fv];
			} elseif(($fv === 'true' && ($fv = true)) || ($fv === 'false' && (($fv = false) || true))){
				$object[$fk] = $fv;
			} else {
				$object[$fk] = $fv;
			}
		}
	}
	
	function deleteOldObjects(&$db, &$format, $parent = null){
		if(!is_null($parent)) {
			$sql = 'DELETE FROM '.$format->id.' WHERE parent = '.$parent.';';
		} else {
			$sql = 'DELETE FROM '.$format->id;
		}
		
		if ($db->ModifyingQuery($sql) == ERROR_DB_BADQUERY) {
			FatalError("Ошибка в запросе". $sql);
		}
	}

	function UploadFile($file, $uploadDir = NULL) {
		$uploadDir = !is_null($uploadDir) ? $uploadDir : BASEPATH.'/tmp/csvimport';

		if(!isset($file['name'])) {
			return false;
		}

		if(!preg_match('/\.(csv|txt)$/i', $file['name'])) {
			return false;
		}

		return $this->UploadFileFromUser($file, $uploadDir);
	}

	function UploadFileFromUser($file, $dir) {
		PXProfiler::begin('Upload user file '.number_format($file['size']).' byte');

		$dirO = new NLDir($dir);
		$dir = $dirO->name;

		MakeDirIfNotExists($dir);
		if ($file && $file != 'none' && $file['name'] && is_writable($dir)) {
			$newFileName = $dir.'/'._TranslitFilename($file['name']);

			if (!@copy($file['tmp_name'], $newFileName)) {
				FatalError('ERROR_IO_BADPERMISSIONS', $dir);
			}

			if (!@chmod($newFileName, 0664)) {
				FatalError('ERROR_IO_BADPERMISSIONS', $dir);
			}
			PXProfiler::end();

			return $newFileName;

		} else {
			PXProfiler::end();
			return false;
		}
	}

	function _UploadTable() {
		return <<<HTML
			<h2>Импорт данных из CSV файла</h2>

			<form action="action.phtml" method="POST" enctype="multipart/form-data" class="autoheight">
				<input type="hidden" name="area" value="{$this->area}">
				<input type="file" name="file">
				<input type="submit" class="button" value="Импортировать данные">
			</form>
HTML;
	}

	function _GetCsvSource($filename, $length=NULL) {
		PXProfiler::begin('GetCsvSource()');

		if (!is_file($filename) || !is_readable($filename)) {
			PXProfiler::end();
			return FALSE;
		}

		$row = 1;
		$maxLen = 0;
		$handle = fopen($filename, "r");
		$result = array();

		while (($data = fgetcsv($handle, 4096, ";")) !== FALSE && (!$length || $row <= $length)) {
			$num = count($data);
			for ($c=0; $c<$num; $c++) {
				$result[$row][$c] = $this->_PrepareString($data[$c]);
			}
			$maxLen = max($maxLen, $num);
			$row++;
		}

		fclose($handle);

		foreach ($result as $k=>$v) {
			if (count($v) < $maxLen) {
				$result[$k] = array_pad($result[$k], $maxLen, NULL);
			}
		}

		PXProfiler::end();

		return $result;
	}

	#method for safe access
	function getParsedCsv($filename, $length=null) {
		return $this->_GetCsvSource($filename, $length);
	}

	function _PrepareString($str) {
		if (!($length = strlen($str))) return NULL;
		$str = strtr($str, $this->htmlEsc);
		$str = convert_cyr_string($str, 'w', 'k');
		$str = str_replace("\\", '&#92;', $str);
		$str = str_replace("\n", ' ', $str);
		$str = str_replace("\\n", "\n", $str);
		$str = str_replace("\\r", "\r", $str);
		$str = str_replace("--%--", ";", $str);
		$str = str_replace("'", "&#146;", $str);
		$str = ereg_replace("\"([^\"]|$)", "\\1", $str);
		// (VERY SLOW!!!) $str = str_replace('#', '', preg_replace("/(([^#]+)#){2}/", "\\1", $str));
		$str = addslashes($str);
		$str = trim($str);
		return $str;
	}
}

?>
