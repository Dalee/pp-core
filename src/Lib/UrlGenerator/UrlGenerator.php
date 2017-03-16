<?php

namespace PP\Lib\UrlGenerator;

use PP\Lib\UrlGenerator\Roles\GeneratorInterface;

class UrlGenerator {

	/**
	 * @param GeneratorInterface $generator
	 * @param string $action
	 * @param array[string]string $params
	 * @return string
	 * @throws Exception
	 */
	public static function generate(GeneratorInterface $generator, $action, $params =[]) {

		switch ($action) {
			case GeneratorInterface::ACTION_INDEX:
				$url = $generator->indexUrl($params);
				break;
			case GeneratorInterface::ACTION_ACTION:
				$url = $generator->actionUrl($params);
				break;
			case GeneratorInterface::ACTION_JSON:
				$url = $generator->jsonUrl($params);
				break;
			case GeneratorInterface::ACTION_POPUP:
				$url = $generator->popupUrl($params);
				break;
			default:
				throw new Exception("Action '$action' doesn't exist.");
		}

		return $url;
	}

}
