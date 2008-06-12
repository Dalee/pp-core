<?php
/**
 * æÁÊÌ
 * ÈÒÁÎÉÔÓÑ × site/htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/éíñ_æáêìá
 * ÎÁÐÒÉÍÅÒ: site/htdocs/ai/news/123034/attach/rules.doc
 *
 * displayType FILE
 * storageType file
 */
class PXStorageTypeFile extends PXStorageType {
	static function proceedFile($field, $object, &$param) {
		$fileArray = $object[$field->name];
		$basedir = BASEPATH.'/site/htdocs/ai/'.$param['format'].'/'.$param['id'].'/'.$field->name.'/';
		$filename = null;

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

					copy($fileArray['tmp_name'], $basedir.$filename);
				}
			}

			if (isset($fileArray['remove']) && is_array($fileArray['remove'])) {
				foreach ($fileArray['remove'] as $fileName) {
					if (file_exists($basedir.$fileName)) {
						unlink($basedir.$fileName);
					}
				}
			}
		}
	}

	static function normalize($field, $object, &$param) {
		$fileName = NULL;
		$basedir = 'ai/'.$param['format'].'/'.$object['id'].'/'.$field->name;
		$d = new NLDir(BASEPATH.'/site/htdocs/'.$basedir);

		if ($d->Exist()) {
			while ($entry = $d->Read()) {
				$fileName = $entry;
			}
		}

		return array(
			'filename' => isset($fileName) ? $fileName                  : NULL,
			'fullpath' => isset($fileName) ? '/'.$basedir.'/'.$fileName : NULL,

			'type'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? mime_content_type($d->name.'/'.$fileName) : NULL,
			'size'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? filesize($d->name.'/'.$fileName)          : 0
		);
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
?>
