<?php

use \Symfony\Component\Yaml\Yaml;

/**
 * Yaml с предобработчиком для совместимости со старыми yaml файлами (табы для отступов, функция include).
 * @deprecated Следует использовать просто Symfony\Yaml.
 */
class PXYaml extends Yaml {

	/**
	 * {@inheritdoc}
	 */
	public static function parse($input, $exceptionOnInvalidType = false, $objectSupport = false, $objectForMap = false) {
		$input = preg_replace_callback('|^((\s*)include\s+(.+?))$|im'.REGEX_MOD, [__CLASS__, 'replaceIncludeWhenParseLangYaml'], $input);
		$input = preg_replace('/\t/'.REGEX_MOD, '  ' , $input);

		return parent::parse($input, $exceptionOnInvalidType, $objectSupport, $objectForMap);
	}

	/**
	 * Колбэк для preg_replace_callback.
	 * Подставляет файл вместо "include path/filename.ext"
	 * @param array $matches
	 * @return string
	 */
	private static function replaceIncludeWhenParseLangYaml($matches) {
		$indent = $matches[2];
		$filename = $matches[3];

		$string = '';
		$tmp = null;

		if(file_exists($filename)) {
			$tmp = @file($filename);

		} elseif(file_exists(BASEPATH.$filename)) {
			$tmp = @file(BASEPATH.$filename);

		} else {
			FatalError('Не могу найти '.$filename);
		}

		if(is_array($tmp)) {
			foreach($tmp as $s) {
				if(!strlen(trim($s))) {
					continue;
				}

				$string .= $indent.$s;
			}
		}

		return $string;
	}

}
