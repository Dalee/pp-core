<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use PP\Lib\UrlGenerator\AdminUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;
use Tests\Base\AbstractUnitTest;
use PP\Module\ModuleInterface;

class AdminUrlGeneratorTest extends AbstractUnitTest {

	public function testIndexUrlFromTargetModule() {
		$testArea = 'targetArea';
		$params = [
			'a' => '1-+',
			'b' => '2 2 а',
		];
		$expectedUrl = '/admin/?area=targetArea&a=1-%2B&b=2+2+%D0%B0';

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setTargetModule($testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}

	public function testIndexUrlFromCurrentModule() {
		$testArea = 'currentArea';
		$params = [
			'a' => '1',
			'b' => '2',
		];
		$expectedUrl = '/admin/?area=currentArea&a=1&b=2';

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setCurrentModule($testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}

	/**
	 * @expectedException \Exception
	 * @expectedExceptionMessage Don't given target module and current module.
	 */
	public function testIndexUrlError() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();
		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl([]);
	}

	/**
	 * @dataProvider actionUrlProvider
	 */
	public function testActionUrl($sid, $testArea, $overriderParams, $expectedActionUrl) {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();
		/** @var \PHPUnit_Framework_MockObject_MockObject | \PXRequest $request */
		$request = $this->getMockBuilder('\PXRequest')
			->disableOriginalConstructor()
			->setMethods(['getSid'])
			->getMock();

		$request->expects($this->any())
			->method('getSid')
			->willReturn($sid);

		$content->setTargetModule($testArea);
		$content->setRequest($request);
		$generator = new AdminUrlGenerator($content);


		$actualUrl = $generator->actionUrl($overriderParams);
		$this->assertEquals($expectedActionUrl, $actualUrl);
	}

	public function actionUrlProvider() {
		return [
			[
				'123',
				'targetArea',
				['a' => '1', 'b' => '2'],
				'/admin/action.phtml?area=targetArea&sid=123&a=1&b=2'
			],
			[
				'123',
				'targetArea',
				['sid' => '1', 'area' => '2'],
				'/admin/action.phtml?area=2&sid=1'
			],
		];
	}

	public function testJsonUrl() {
		$testArea = 'targetArea';
		$params = [
			'a' => '1',
			'b' => '2',
		];
		$expectedUrl = '/admin/json.phtml?area=targetArea&a=1&b=2';

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setTargetModule($testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->jsonUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}

	/**
	 * @dataProvider popupUrlProvider
	 */
	public function testPopupUrl($sid, $testArea, $overriderParams, $expectedPopupUrl) {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();
		/** @var \PHPUnit_Framework_MockObject_MockObject | \PXRequest $request */
		$request = $this->getMockBuilder('\PXRequest')
			->disableOriginalConstructor()
			->setMethods(['getSid'])
			->getMock();

		$request->expects($this->any())
			->method('getSid')
			->willReturn($sid);

		$content->setTargetModule($testArea);
		$content->setRequest($request);
		$generator = new AdminUrlGenerator($content);


		$actualUrl = $generator->popupUrl($overriderParams);
		$this->assertEquals($expectedPopupUrl, $actualUrl);
	}

	public function popupUrlProvider() {
		return [
			[
				'123',
				'targetArea',
				['a' => '1', 'b' => '2'],
				'/admin/popup.phtml?area=targetArea&sid=123&a=1&b=2'
			],
			[
				'123',
				'targetArea',
				['sid' => '1', 'area' => '2'],
				'/admin/popup.phtml?area=2&sid=1'
			],
		];
	}

	public function testGenerate() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		/** @var AdminUrlGenerator | \PHPUnit_Framework_MockObject_MockObject $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AdminUrlGenerator')
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

		/** @var AdminUrlGenerator $generator */
		$generator = $this->getMockBuilder('\PP\Lib\UrlGenerator\AdminUrlGenerator')
			->setConstructorArgs([$content])
			->setMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->generate([]);
	}
}
