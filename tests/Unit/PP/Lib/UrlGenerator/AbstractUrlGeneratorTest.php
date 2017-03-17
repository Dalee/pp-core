<?php

namespace Unit\PP\Lib\UrlGenerator;

use PP\Lib\UrlGenerator\AbstractUrlGenerator;
use PP\Module\ModuleInterface;
use Tests\Base\AbstractUnitTest;
use PP\Lib\UrlGenerator\ContextUrlGenerator;

class AbstractUrlGeneratorTest extends AbstractUnitTest {

	/**
	 * @expectedException \Exception
	 * @expectedExceptionMessage Don't given target module and current module.
	 */
	public function testGetAreaError() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();

		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
			->setConstructorArgs([$content])
			->getMock();

		$this->callProtectedMethod($generator, 'getArea', []);
	}

	public function testGetAreaFromCurrentModule() {
		$currentModule = 'currentModule';
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setCurrentModule($currentModule);

		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
			->setConstructorArgs([$content])
			->getMock();

		$actualArea = $this->callProtectedMethod($generator, 'getArea', []);
		$this->assertEquals($currentModule, $actualArea);
	}

	public function testGetAreaFromTargetModule() {
		$targetModule = 'targetModule';
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setTargetModule($targetModule);

		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
			->setConstructorArgs([$content])
			->getMock();

		$actualArea = $this->callProtectedMethod($generator, 'getArea', []);
		$this->assertEquals($targetModule, $actualArea);
	}

	public function testGenerateUrl() {
		$url = '/currentArea/';
		$params = [
			'a' => '1-+',
			'b' => '2 2 а',
		];
		$expectedUrl = '/currentArea/?a=1-%2B&b=2+2+%D0%B0';
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
			->disableOriginalConstructor()
			->getMock();

		$actualUrl = $this->callProtectedMethod($generator, 'generateUrl', [$url, $params]);
		$this->assertEquals($expectedUrl, $actualUrl);
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

		/** @var AbstractUrlGenerator $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
			->setConstructorArgs([$content])
			->setMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->generate([]);
	}

	public function testGenerate() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		/** @var AbstractUrlGenerator | \PHPUnit_Framework_MockObject_MockObject $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AbstractUrlGenerator')
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
}
