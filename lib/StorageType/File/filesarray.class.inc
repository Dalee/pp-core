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
class PXStorageTypeFilesarray extends PXStorageTypeAbstractFile {
	function cloneFile($field, $object, &$param, $symlink = false) {
		$donor = $param['donor'];

		$fileArray       = $donor[$field->name];
		$ai              = $this->ai($field, $object, $param['format']);
		$basedir         = HTDOCS_PATH . $ai;

		if (is_array($fileArray)) {
			MakeDirIfNotExists($basedir);

			foreach($fileArray as $k=>$f) {
				$object[$field->name][$k]['path'] = $ai.$f['entry'];

				if($symlink) {
					symlink(
						HTDOCS_PATH . $donor[$field->name][$k]['path'],
						HTDOCS_PATH . $object[$field->name][$k]['path']
					);

				} else {
					copy(
						HTDOCS_PATH . $donor[$field->name][$k]['path'],
						HTDOCS_PATH . $object[$field->name][$k]['path']
					);
				}
			}
		}
	}

	//FIXME: Remove static usage of this method from projects !
	function proceedFile($field, &$object, &$param) {
		$is_static_call  = !isset($this) || !(is_a($this, 'PXStorageTypeAbstractFile')); //see FIXME above
		$varName         = $field->name;
		$ai              = $is_static_call ?
		                      self::ai($field, $object, $param['format']) :
		                      $this->ai($field, $object, $param['format']);
		$basedir         = HTDOCS_PATH . $ai;
		$fileArray       = &$object[$varName];
		$allowed         = isset($param['allowed']) ? $param['allowed'] : NULL;
		$invalidateCache = false;

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
				$_ = $is_static_call ?
					self::copyFile($field, $fileArray['tmp_name'][0], $filename) :
					$this->copyFile($field, $fileArray['tmp_name'][0], $filename);
				
				$invalidateCache = true;
			}
		}

		if (is_array($fileArray) && isset($fileArray['remove']) && is_array($fileArray['remove'])) {
			foreach ($fileArray['remove'] as $fileName) {
				if (file_exists($basedir.'/'.$fileName)) {
					unlink($basedir.'/'.$fileName);
					$invalidateCache = true;
				}
			}
		}

		if (is_array($_FILES) && isset($_FILES[$varName.'_edit']) && is_array($_FILES[$varName.'_edit'])) {
			foreach ($_FILES[$varName.'_edit']['name'] as $k=>$v) {
				if (file_exists($basedir.'/'.$k) && isset($v) && strlen($v)) {
					unlink($basedir.'/'.$k);
					$_ = $is_static_call ?
						self::copyFile($field, $_FILES[$varName.'_edit']['tmp_name'][$k], $basedir.'/'._TranslitFilename($v)) :
						$this->copyFile($field, $_FILES[$varName.'_edit']['tmp_name'][$k], $basedir.'/'._TranslitFilename($v));
					$invalidateCache = true;
				}
			}
		}

		if($invalidateCache){
			$_ = $is_static_call ? self::removeFromCache($ai) : $this->removeFromCache($ai);
		}

		if (!$is_static_call) {
			$fileArray = $this->normalize($field, $object, $param);
		}
	}

	function __normalize($field, $object, &$param) {
		$filesArray = array();
		$basedir    = $this->ai($field, $object, $param['format']);
		$d          = new NLDir(HTDOCS_PATH . $basedir);

		if ($d->Exist()) {
			foreach ($d->getFileNames() as $entry) {
				$diskPath = HTDOCS_PATH . $basedir . $entry;
				$httpPath = $basedir.$entry;

				$filesArray[$entry] = array(
					'path' => isset($httpPath) ? $httpPath : NULL,
					'filename' => $entry,
					'size'     => filesize($diskPath),
					'type'     => mime_content_type($diskPath),
					'entry'    => $entry
				);
				$filesArray[$entry]['fullpath'] = &$filesArray[$entry]['path'];

				unset($httpPath);
				unset($diskPath);
			}
		}

		return $filesArray;
	}
}
?>
