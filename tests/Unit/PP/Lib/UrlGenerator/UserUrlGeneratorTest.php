<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;


use PP\Lib\UrlGenerator\UserUrlGenerator;

class UserUrlGeneratorTest extends \PHPUnit_Framework_TestCase {

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

}
