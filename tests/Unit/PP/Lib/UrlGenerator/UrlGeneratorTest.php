<?php

namespace Unit\PP\Lib\UrlGenerator;

use Tests\Base\AbstractUnitTest;
use PP\Lib\UrlGenerator\UrlGenerator;
use PP\Lib\UrlGenerator\UserUrlGenerator;
use PP\Lib\UrlGenerator\AdminUrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;

class UrlGeneratorTest extends AbstractUnitTest {

	public function testGetUserGenerator() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$userUrlGenerator = $urlGenerator->getUserGenerator();
		$this->assertTrue($userUrlGenerator instanceof UserUrlGenerator);
	}

	public function testGetAdminGenerator() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$userUrlGenerator = $urlGenerator->getAdminGenerator();
		$this->assertTrue($userUrlGenerator instanceof AdminUrlGenerator);
	}
}
