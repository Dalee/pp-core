<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use PP\Lib\UrlGenerator\UserUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;
use Tests\Base\AbstractUnitTest;
use PP\Module\ModuleInterface;

class UserUrlGeneratorTest extends AbstractUnitTest {

	/**
	 * @expectedException \LogicException
	 * @expectedExceptionMessage You cannot use the method:
	 */
	public function testIndexUrl() {
		$generator = $this->getGenerator();
		$generator->indexUrl([]);
	}


	public function testActionUrl() {
		$generator = $this->getGenerator();
		$params = [
			'a' => 1,
			'b' => 2,
		];
		$expectedUrl = '/testArea.action?a=1&b=2';
		$actualUrl = $generator->actionUrl($params);
		$this->assertEquals($expectedUrl, $actualUrl);
	}

	public function testJsonUrl() {
		$generator = $this->getGenerator();
		$params = [
			'a' => 1,
			'b' => 2,
		];
		$expectedUrl = '/testArea.json?a=1&b=2';
		$actualUrl = $generator->jsonUrl($params);
		$this->assertEquals($expectedUrl, $actualUrl);
	}

	/**
	 * @expectedException \LogicException
	 * @expectedExceptionMessage You cannot use the method:
	 */
	public function testPopupUrl() {
		$generator = $this->getGenerator();
		$generator->popupUrl([]);
	}

	/**
	 * @return UserUrlGenerator
	 */
	protected function getGenerator() {
		/** @var UserUrlGenerator | \PHPUnit_Framework_MockObject_MockObject $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\UserUrlGenerator')
			->disableOriginalConstructor()
			->setMethods(['getArea'])
			->getMock();

		$generator->expects($this->any())
			->method('getArea')
			->willReturn('testArea');

		return $generator;
	}

	public function testGenerate() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		/** @var UserUrlGenerator | \PHPUnit_Framework_MockObject_MockObject $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\UserUrlGenerator')
			->setConstructorArgs([$content])
			->setMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->expects($this->at(0))
			->method('indexUrl')
			->with([]);
		$generator->expects($this->at(1))
			->method('actionUrl')
			->with([]);
		$generator->expects($this->at(2))
			->method('jsonUrl')
			->with([]);
		$generator->expects($this->at(3))
			->method('popupUrl')
			->with([]);

		$content->setTargetAction(ModuleInterface::ACTION_INDEX);
		$generator->generate([]);

		$content->setTargetAction(ModuleInterface::ACTION_ACTION);
		$generator->generate([]);

		$content->setTargetAction(ModuleInterface::ACTION_JSON);
		$generator->generate([]);

		$content->setTargetAction(ModuleInterface::ACTION_POPUP);
		$generator->generate([]);
	}

	/**
	 * @expectedException \Exception
	 * @expectedExceptionMessage Action 'targetAction' doesn't exist.
	 */
	public function testGenerateError() {
		$targetAction = 'targetAction';
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setTargetAction($targetAction);

		/** @var UserUrlGenerator $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\UserUrlGenerator')
			->setConstructorArgs([$content])
			->setMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->generate([]);
	}
}
