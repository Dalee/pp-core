<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use PHPUnit\Framework\MockObject\MockObject;
use PP\Lib\UrlGenerator\AdminUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;
use Tests\Base\AbstractUnitTest;
use PP\Module\ModuleInterface;

class AdminUrlGeneratorTest extends AbstractUnitTest {

	protected AdminUrlGenerator|MockObject $generator;

	protected MockObject|ContextUrlGenerator $content;

	/**
	 * @before
	 */
	public function before() {
		$this->content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();

		$this->generator = $this->getMockBuilder('PP\Lib\UrlGenerator\AdminUrlGenerator')
			->setConstructorArgs([$this->content])
			->onlyMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();
	}

	public function testIndexUrlFromTargetModule() {
		$testArea = 'targetArea';
		$params = [
			'a' => '1-+',
			'b' => '2 2 а',
		];
		$expectedUrl = '/admin/?area=targetArea&a=1-%2B&b=2+2+%D0%B0';

		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
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

		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();

		$content->setCurrentModule($testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}

	public function testIndexUrlError() {
		$this->expectExceptionMessage("Don't given target module and current module.");
		$this->expectException(\Exception::class);

		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();
		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl([]);
	}

	/**
	 * @dataProvider actionUrlProvider
	 */
	public function testActionUrl($sid, $testArea, $overriderParams, $expectedActionUrl) {
		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();
		/** @var MockObject | \PXRequest $request */
		$request = $this->getMockBuilder('PXRequest')
			->disableOriginalConstructor()
			->onlyMethods(['getSid'])
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

		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
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
		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();
		/** @var MockObject | \PXRequest $request */
		$request = $this->getMockBuilder('PXRequest')
			->disableOriginalConstructor()
			->onlyMethods(['getSid'])
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

	public function testGenerateActionIndex() {
		$this->generator->expects($this->once())
			->method('indexUrl')
			->with([]);

		$this->content->setTargetAction(ModuleInterface::ACTION_INDEX);
		$this->generator->generate([]);
	}

	public function testGenerateActionAction() {
		$this->generator->expects($this->once())
			->method('actionUrl')
			->with([]);

		$this->content->setTargetAction(ModuleInterface::ACTION_ACTION);
		$this->generator->generate([]);
	}

	public function testGenerateActionJson() {
		$this->generator->expects($this->once())
			->method('jsonUrl')
			->with([]);

		$this->content->setTargetAction(ModuleInterface::ACTION_JSON);
		$this->generator->generate([]);
	}

	public function testGenerateActionPopup() {
		$this->generator->expects($this->once())
			->method('popupUrl')
			->with([]);

		$this->content->setTargetAction(ModuleInterface::ACTION_POPUP);
		$this->generator->generate([]);
	}

	public function testGenerateError() {
		$this->expectExceptionMessage("Action 'targetAction' doesn't exist.");
		$this->expectException(\Exception::class);

		$targetAction = 'targetAction';
		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('PP\Lib\UrlGenerator\ContextUrlGenerator')
			->addMethods(['_'])
			->getMock();

		$content->setTargetAction($targetAction);

		/** @var AdminUrlGenerator $generator */
		$generator = $this->getMockBuilder('PP\Lib\UrlGenerator\AdminUrlGenerator')
			->setConstructorArgs([$content])
			->onlyMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->generate([]);
	}

}
