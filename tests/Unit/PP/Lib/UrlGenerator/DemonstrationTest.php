<?php

namespace Tests\Unit\PP\Lib\UrlGeneratorTest;

use PP\Lib\UrlGenerator\Context;
use PP\Lib\UrlGenerator\Roles\GeneratorInterface;
use Tests\Base\AbstractUnitTest;

class DemonstrationTest extends AbstractUnitTest {

	public function testBuildAdminUrl() {
		// Тот реквест, который у нас есть
		$request = new \PXRequest();

		// Оборачиваем реквест объектом контекст, для добавления каких-либо контекстных вещей
		// пока что это только название целевого модуля
		$context = new Context($request, 'targetModule');

		// Два сценария, для админки, для юзера
		$userGenerator = new \PP\Lib\UrlGenerator\Roles\UserUrlGenerator($context);
		$userGenerator = new \PP\Lib\UrlGenerator\Roles\AdminUrlGenerator($context);

		// какие либо параметры
		$params = [
			'a' => 'blablabla',
			'ttt' => 'hmmm',
		];

		// Генерируем ссылку для пользователя в index
		\PP\Lib\UrlGenerator\UrlGenerator::generate($userGenerator, GeneratorInterface::ACTION_INDEX, $params);

		// Генерируем ссылку для админа в json
		\PP\Lib\UrlGenerator\UrlGenerator::generate($userGenerator, GeneratorInterface::ACTION_JSON, $params);

	}
}
