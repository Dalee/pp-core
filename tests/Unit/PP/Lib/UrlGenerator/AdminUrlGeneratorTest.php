<?php

namespace Unit\PP\Lib\UrlGenerator;

use PP\Lib\UrlGenerator\AdminUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;
use PP\Module\AbstractModule;
use Tests\Base\AbstractUnitTest;

class AdminUrlGeneratorTest extends AbstractUnitTest {

	/**
	 * @expectedException \Exception
	 * @expectedExceptionMessage Don't given target module and current module.
	 */
	public function testIndexUrlError() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();

		$generator = new AdminUrlGenerator($content);
		$generator->indexUrl();
	}

	public function testIndexUrlWithCurrentModule() {
		$testArea = 'currentArea';
		$params = [
			'a' => '1',
			'b' => '2',
		];
		$expectedUrl = '/admin/?a=1&b=2&area=currentArea';

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		/** @var \PHPUnit_Framework_MockObject_MockObject | AbstractModule $currentModule */
		$currentModule = $this->getMockBuilder('\PP\Module\AbstractModule')
			->disableOriginalConstructor()
			->getMock();

		$content->setCurrentModule($currentModule);
		$this->setProtectedProperty($content, 'currentModule', $currentModule);
		$this->setProtectedProperty($currentModule, 'area', $testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}

	public function testIndexUrlWithTargetModule() {
		$testArea = 'targetArea';
		$params = [
			'a' => '1',
			'b' => '2',
		];
		$expectedUrl = '/admin/?a=1&b=2&area=targetArea';

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->setMethods(['_'])
			->getMock();

		$content->setTargetModule($testArea);

		$generator = new AdminUrlGenerator($content);
		$actualUrl = $generator->indexUrl($params);

		$this->assertEquals($expectedUrl, $actualUrl);
	}
}
