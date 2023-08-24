<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use PHPUnit\Framework\MockObject\MockObject;
use PP\Lib\UrlGenerator\AdminUrlGenerator;
use PP\Lib\UrlGenerator\UserUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;
use Tests\Base\AbstractUnitTest;
use PP\Module\ModuleInterface;

class UserUrlGeneratorTest extends AbstractUnitTest {

	protected AdminUrlGenerator|MockObject $generator;

	protected MockObject|ContextUrlGenerator $content;

	/**
	 * @before
	 */
	public function before() {
		$this->content = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->addMethods(['_'])
			->getMock();

		$this->generator = $this->getMockBuilder(\PP\Lib\UrlGenerator\UserUrlGenerator::class)
			->setConstructorArgs([$this->content])
			->onlyMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();
	}

	public function testIndexUrl() {
		$this->expectExceptionMessage("You cannot use the method:");
		$this->expectException(\LogicException::class);

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

	public function testPopupUrl() {
		$this->expectExceptionMessage("You cannot use the method:");
		$this->expectException(\LogicException::class);

		$generator = $this->getGenerator();
		$generator->popupUrl([]);
	}

	/**
	 * @return UserUrlGenerator
	 */
	protected function getGenerator() {
		/** @var UserUrlGenerator|MockObject $generator */
		$generator = $this->getMockBuilder(\PP\Lib\UrlGenerator\UserUrlGenerator::class)
			->disableOriginalConstructor()
			->onlyMethods(['getArea'])
			->getMock();

		$generator->expects($this->any())
			->method('getArea')
			->willReturn('testArea');

		return $generator;
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
		$content = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->addMethods(['_'])
			->getMock();

		$content->setTargetAction($targetAction);

		/** @var UserUrlGenerator $generator */
		$generator = $this->getMockBuilder(\PP\Lib\UrlGenerator\UserUrlGenerator::class)
			->setConstructorArgs([$content])
			->onlyMethods(['indexUrl', 'actionUrl', 'jsonUrl', 'popupUrl'])
			->getMock();

		$generator->generate([]);
	}

}
