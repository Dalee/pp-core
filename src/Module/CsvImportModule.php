<?php

namespace PP\Module;

use PP\Lib\UrlGenerator\ContextUrlGenerator;
use PP\Lib\UrlGenerator\UrlGenerator;

/**
 * Class CsvImportModule.
 *
 * @package PP\Module
 */
class CsvImportModule extends AbstractModule
{

	public $htmlEsc;
	public $settings;

	public function __construct($area, $settings)
	{
		parent::__construct($area, $settings);

		$this->settings = $settings;
		$this->area = $area;

		$this->htmlEsc = [
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
		];
	}

	public function adminIndex()
	{
		$layout = $this->layout;
		$request = $this->request;

		$layout->setOneColumn();
		$html = '';

		switch ($request->GetVar('status')) {
			case 'error':
				$html .= $this->_UploadTable();
				$html .= '<h1 class="error">Ошибка</h1>';
				$html .= '<p class="error">Импорт данных не состоялся.<BR>';

				switch ($request->GetVar('error')) {
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
				$html .= $this->_Success();
				break;

			default:
				$html .= $this->_UploadTable();
				break;
		}

		$layout->Assign('INNER.0.0', $html);
	}

	public function _Success()
	{
		$request = $this->request;
		$app = $this->app;

		$html = '<h2>Импорт данных &laquo;' . $app->types[$this->settings['datatype']]->title . '&raquo; успешно произведен</h2>';
		$html .= '<p>В базу внесено <strong>' . htmlspecialchars($request->GetVar('quantity'), ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET) . '</strong> объектов.</p>';
		return $html;
	}

	public function adminAction()
	{
		$request = $this->request;
		$app = $this->app;
		$db = $this->db;

		$context = new ContextUrlGenerator();
		$context->setCurrentModule($this->area);
		$generator = new UrlGenerator($context);

		$redirTo = $generator->getAdminGenerator()->indexUrl();

		if (($filename = $this->UploadFile($request->GetUploadFile('file')))) {
			$quantity = $this->ImportToDb($this->_GetCsvSource($filename), $app, $db, $request);

			if (is_numeric($quantity)) {
				$redirTo .= '&status=success&quantity=' . $quantity;
			} else {
				$redirTo .= '&status=error&error=' . $quantity;
			}

		} else {
			$redirTo .= '&status=error&error=notupload';
		}

		return $redirTo;
	}

	public function ImportToDB($csv)
	{
		$app = $this->app;
		$db = $this->db;

		$objects = [];
		$fields = [];
		$parent = null;

		$format = $app->types[$this->settings['datatype']];

		foreach ($this->settings['field'] as $f) {
			[$k, $v] = explode('|', $f);

			if (trim($k) == 'parent') {
				$v = $app->GetProperty(trim($v));
				$parent = $v;
			}

			$fields[trim($k)] = trim($v);
		}

		foreach ($csv as $ln => $line) {
			if ($ln === 1 && isset($this->settings['skipfirst']) && $this->settings['skipfirst'] === 'true') {
				continue;
			}

			$valid = 0;
			foreach ($line as $d) {
				if (mb_strlen($d)) {
					$valid++;
				}
			}

			if (!$valid) {
				continue;
			}

			$object = [];

			$this->constructObject($object, $fields, $line);

			$objects[] = $object;
		}

		$db->transactionBegin();

		$this->deleteOldObjects($db, $format, $parent);

		$db->addContentObjects($format, $objects);
		$db->transactionCommit();

		return count($objects);
	}

	public function constructObject(&$object, $fields, $row)
	{
		foreach ($fields as $fk => $fv) {
			if ($fk === 'parent') {
				$object[$fk] = $fv;
			} elseif (is_numeric($fv)) {
				$object[$fk] = $row[$fv];
			} elseif (($fv === 'true' && ($fv = true)) || ($fv === 'false' && (($fv = false) || true))) {
				$object[$fk] = $fv;
			} else {
				$object[$fk] = $fv;
			}
		}
	}

	public function deleteOldObjects(&$db, &$format, $parent = null)
	{
		if (!is_null($parent)) {
			$sql = 'DELETE FROM ' . $format->id . ' WHERE parent = ' . $parent . ';';
		} else {
			$sql = 'DELETE FROM ' . $format->id;
		}

		if ($db->ModifyingQuery($sql) == ERROR_DB_BADQUERY) {
			FatalError("Ошибка в запросе" . $sql);
		}
	}

	public function UploadFile($file, $uploadDir = NULL)
	{
		$uploadDir = !is_null($uploadDir) ? $uploadDir : BASEPATH . '/tmp/csvimport';

		if (!isset($file['name'])) {
			return false;
		}

		if (!preg_match('/\.(csv|txt)$/i', $file['name'])) {
			return false;
		}

		return $this->UploadFileFromUser($file, $uploadDir);
	}

	public function UploadFileFromUser($file, $dir)
	{
		$dirO = new \NLDir($dir);
		$dir = $dirO->name;

		MakeDirIfNotExists($dir);

		if ($file && $file != 'none' && $file['name'] && is_writable($dir)) {
			$newFileName = $dir . '/' . _TranslitFilename(_stripBadFileChars($file['name']));

			if (!@copy($file['tmp_name'], $newFileName)) {
				FatalError('ERROR_IO_BADPERMISSIONS: ' . $dir);
			}

			if (!@chmod($newFileName, 0664)) {
				FatalError('ERROR_IO_BADPERMISSIONS: ' . $dir);
			}

			return $newFileName;
		} else {
			return false;
		}
	}

	public function _UploadTable()
	{
		return <<<HTML
			<h2>Импорт данных из CSV файла</h2>

			<form action="action.phtml" method="POST" enctype="multipart/form-data" class="autoheight">
				<input type="hidden" name="area" value="{$this->area}">
				<input type="file" name="file">
				<input type="submit" class="button" value="Импортировать данные">
			</form>
HTML;
	}

	public function _GetCsvSource($filename, $length = NULL)
	{
		if (!is_file($filename) || !is_readable($filename)) {
			return FALSE;
		}

		$row = 1;
		$maxLen = 0;
		$handle = fopen($filename, "r");
		$result = [];

		while (($data = fgetcsv($handle, 4096, ";")) !== FALSE && (!$length || $row <= $length)) {
			$num = count($data);
			for ($c = 0; $c < $num; $c++) {
				$result[$row][$c] = iconv(CHARSET_WINDOWS, CHARSET_UTF8, $data[$c]);
			}
			$maxLen = max($maxLen, $num);
			$row++;
		}

		fclose($handle);

		foreach ($result as $k => $v) {
			if (count($v) < $maxLen) {
				$result[$k] = array_pad($result[$k], $maxLen, NULL);
			}
		}

		return $result;
	}

	// method for safe access
	public function getParsedCsv($filename, $length = null)
	{
		return $this->_GetCsvSource($filename, $length);
	}

}
