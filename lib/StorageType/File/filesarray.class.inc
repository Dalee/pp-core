<?php
/**
 * óÐÉÓÏË ÆÁÊÌÏ×
 * ÈÒÁÎÑÔÓÑ × htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/éíñ_æáêìá
 * ÎÁÐÒÉÍÅÒ: htdocs/ai/news/123034/attach/rules.doc
 *           htdocs/ai/news/123034/attach/cases.xls
 *           htdocs/ai/news/123034/attach/nude.jpg
 *
 * displayType FILESARRAY
 * storageType filesarray
 */
class PXStorageTypeFilesarray extends PXStorageType {
	static function proceedFile($field, $object, &$param) {
		$varName   = $field->name;
		$basedir   = BASEPATH.'/htdocs/ai/'.$param['format'].'/'.$param['id'].'/'.$varName;
		$fileArray = $object[$varName];
		$allowed   = isset($param['allowed']) ? $param['allowed'] : NULL;

		if (is_array($fileArray) && !empty($fileArray['name'][0])) {
			$filename = NULL;

			if(is_array($allowed)) {
				if(in_array($fileArray['type'][0], array_keys($allowed))) {
					MakeDirIfNotExists($basedir);

					$tmp = _TranslitFilename($fileArray['name'][0]);
					$tmp = preg_replace('/\.\w+$/', '', $tmp);

					if(in_array($fileArray['type'][0], array_keys($allowed))) {
						$filename = $basedir.'/'.$tmp.'.'.$allowed[$fileArray['type'][0]];
					}
				}

			} else {
				MakeDirIfNotExists($basedir);
				$filename = $basedir.'/'. _TranslitFilename($fileArray['name'][0]);
			}

			if(!is_null($filename)) {
				copy($fileArray['tmp_name'][0], $filename);
			}
		}

		if (is_array($fileArray) && isset($fileArray['remove']) && is_array($fileArray['remove'])) {
			foreach ($fileArray['remove'] as $fileName) {
				if (file_exists($basedir.'/'.$fileName)) {
					unlink($basedir.'/'.$fileName);
				}
			}
		}

		if (is_array($_FILES) && isset($_FILES[$varName.'_edit']) && is_array($_FILES[$varName.'_edit'])) {
			foreach ($_FILES[$varName.'_edit']['name'] as $k=>$v) {
				if (file_exists($basedir.'/'.$k) && isset($v) && strlen($v)) {
					unlink($basedir.'/'.$k);
					copy($_FILES[$varName.'_edit']['tmp_name'][$k], $basedir.'/'._TranslitFilename($v));
				}
			}
		}
	}


	static function normalize($field, $object, &$param) {
		$filesArray = array();
		$basedir = '/ai/'.$param['format'].'/'.$object['id'].'/'.$field->name.'/';
		$d = new NLDir(BASEPATH.'htdocs'.$basedir);

		if ($d->Exist()) {
			foreach ($d->getFileNames() as $entry) {
				$diskPath = BASEPATH.'/htdocs'.$basedir.$entry;
				$httpPath = $basedir.$entry;

				$filesArray[$entry] = array(
					'fullpath' => isset($httpPath) ? $httpPath : NULL,
					'filename' => $entry,
					'size'     => filesize($diskPath),
					'type'     => mime_content_type($diskPath),
					'entry'    => $entry
				);

				unset($httpPath);
				unset($diskPath);
			}
		}

		return $filesArray;
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
?>
