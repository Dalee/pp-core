<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use Tests\Base\AbstractUnitTest;
use PP\Lib\UrlGenerator\UrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;

class UrlGeneratorTest extends AbstractUnitTest {

	public function testGetUserGenerator() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$userUrlGenerator = $urlGenerator->getUserGenerator();
		$this->assertInstanceOf('\PP\Lib\UrlGenerator\UserUrlGenerator', $userUrlGenerator);

		$userUrlGenerator2 = $urlGenerator->getUserGenerator();
		$this->assertTrue($userUrlGenerator === $userUrlGenerator2);

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content2 */
		$content2 = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator->setContext($content2);
		$userUrlGenerator3 = $urlGenerator->getUserGenerator();
		$this->assertFalse($userUrlGenerator3 === $userUrlGenerator2);
	}

	public function testGetAdminGenerator() {
		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$adminUrlGenerator = $urlGenerator->getAdminGenerator();
		$this->assertInstanceOf('\PP\Lib\UrlGenerator\AdminUrlGenerator', $adminUrlGenerator);

		$adminUrlGenerator2 = $urlGenerator->getAdminGenerator();
		$this->assertTrue($adminUrlGenerator === $adminUrlGenerator2);

		/** @var \PHPUnit_Framework_MockObject_MockObject | ContextUrlGenerator $content2 */
		$content2 = $this->getMockBuilder('\PP\Lib\UrlGenerator\ContextUrlGenerator')
			->getMock();
		$urlGenerator->setContext($content2);
		$adminUrlGenerator3 = $urlGenerator->getAdminGenerator();
		$this->assertFalse($adminUrlGenerator3 === $adminUrlGenerator2);
	}
}
