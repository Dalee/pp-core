<?php

namespace Tests\Unit\PP\Lib\UrlGenerator;

use PHPUnit\Framework\MockObject\MockObject;
use Tests\Base\AbstractUnitTest;
use PP\Lib\UrlGenerator\UrlGenerator;
use PP\Lib\UrlGenerator\ContextUrlGenerator;

class UrlGeneratorTest extends AbstractUnitTest {

	public function testGetUserGeneratorForDifferentContext() {
		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$userUrlGenerator = $urlGenerator->getUserGenerator();
		$this->assertInstanceOf(\PP\Lib\UrlGenerator\UserUrlGenerator::class, $userUrlGenerator);

		$userUrlGenerator2 = $urlGenerator->getUserGenerator();
		$this->assertSame($userUrlGenerator, $userUrlGenerator2);

		/** @var MockObject | ContextUrlGenerator $content2 */
		$content2 = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->getMock();
		$urlGenerator->setContext($content2);
		$userUrlGenerator3 = $urlGenerator->getUserGenerator();
		$this->assertNotSame($userUrlGenerator3, $userUrlGenerator2);
	}

	public function testGetAdminGeneratorForDifferentContext() {
		/** @var MockObject | ContextUrlGenerator $content */
		$content = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->getMock();
		$urlGenerator = new UrlGenerator($content);
		$adminUrlGenerator = $urlGenerator->getAdminGenerator();
		$this->assertInstanceOf(\PP\Lib\UrlGenerator\AdminUrlGenerator::class, $adminUrlGenerator);

		$adminUrlGenerator2 = $urlGenerator->getAdminGenerator();
		$this->assertSame($adminUrlGenerator, $adminUrlGenerator2);

		/** @var MockObject | ContextUrlGenerator $content2 */
		$content2 = $this->getMockBuilder(\PP\Lib\UrlGenerator\ContextUrlGenerator::class)
			->getMock();
		$urlGenerator->setContext($content2);
		$adminUrlGenerator3 = $urlGenerator->getAdminGenerator();
		$this->assertNotSame($adminUrlGenerator3, $adminUrlGenerator2);
	}

}
