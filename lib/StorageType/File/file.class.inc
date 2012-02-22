<?php
/**
 * æÁÊÌ
 * ÈÒÁÎÉÔÓÑ × site/htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/éíñ_æáêìá
 * ÎÁÐÒÉÍÅÒ: site/htdocs/ai/news/123034/attach/rules.doc
 *
 * displayType FILE
 * storageType file
 */
class PXStorageTypeFile extends PXStorageTypeAbstractFile {
	//FIXME: Remove static usage of this method from projects !
	function proceedFile($field, &$object, &$param) {
		$is_static_call  = !isset($this) || !(is_a($this, 'PXStorageTypeAbstractFile')); //see FIXME above
		$fileArray       = &$object[$field->name];
		$ai              = $is_static_call ?
		                      self::ai($field, $object, $param['format']) :
		                      $this->ai($field, $object, $param['format']);
		$basedir         = HTDOCS_PATH . $ai;
		$filename        = null;
		$invalidateCache = false;

		if (is_array($fileArray)) {
			if (!empty($fileArray['name'])) {
				if(isset($param['allowed']) && is_array($param['allowed'])) {
					$allowed = $param['allowed'];

					$tmp = _TranslitFilename($fileArray['name']);
					$tmp = preg_replace('/\.\w+$/', '', $tmp);

					if(in_array($fileArray['type'], array_keys($allowed))) {
						$filename = $tmp.'.'.$allowed[$fileArray['type']];
					}

				} else {
					$filename = _TranslitFilename($fileArray['name']);
				}

				if(!is_null($filename)) {
					MakeDirIfNotExists($basedir);
					$d = dir($basedir);
					while ($entry = $d->read()) {
						if (is_file($basedir.$entry)) {
							unlink($basedir.$entry);
						}
					}
					$_ = $is_static_call ?
						self::copyFile($field, $fileArray['tmp_name'], $basedir.$filename) :
						$this->copyFile($field, $fileArray['tmp_name'], $basedir.$filename);

					$invalidateCache = true;
				}
			}

			if (isset($fileArray['remove']) && is_array($fileArray['remove'])) {
				foreach ($fileArray['remove'] as $fileName) {
					if (file_exists($basedir.$fileName)) {
						unlink($basedir.$fileName);
						$invalidateCache = true;
					}
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
		$fileName = NULL;
		$basedir  = $this->ai($field, $object, $param['format']);
		$d        = new NLDir(HTDOCS_PATH . $basedir);

		if ($d->Exist()) {
			while ($entry = $d->Read()) {
				$fileName = $entry;
			}
		}

		$file =  array(
			'filename' => isset($fileName) ? $fileName              : NULL,
			'path'     => isset($fileName) ? $basedir.$fileName     : NULL,
			'type'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? mime_content_type($d->name.'/'.$fileName) : NULL,
			'size'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? filesize($d->name.'/'.$fileName)          : 0
		);
		$file['fullpath'] = &$file['path'];

		return $file;
	}
}
?>
