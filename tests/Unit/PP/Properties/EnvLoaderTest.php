<?php

namespace Tests\Unit\PP\Properties;

use PP\Properties\EnvLoader;
use Tests\Base\AbstractUnitTest;

class EnvLoaderTest extends AbstractUnitTest {

	/**
	 * Ensure correct array is produced
	 */
	public function testMappingList() {
		$_ENV = [
			'DB_TYPE' => 'psql',
			'DB_NAME' => 'project',
			'CAPTCHA_KEY' => 'hohoho',
		];

		$result = EnvLoader::getMappedArray([
			'DB_TYPE',
			'DB_NAME'
		]);

		$this->assertEquals([
			'DB_TYPE' => 'psql',
			'DB_NAME' => 'project'
		], $result);
	}

	/**
	 * Ensure mapping key -> value works
	 */
	public function testMappingArray() {
		$_ENV = [
			'DB_TYPE' => 'psql',
			'DB_NAME' => 'project',
			'CAPTCHA_KEY' => 'hohoho',
		];

		$result = EnvLoader::getMappedArray([
			'DB_TYPE' => 'dbtype',
			'DB_NAME' => 'dbname',
		]);

		$this->assertEquals([
			'dbtype' => 'psql',
			'dbname' => 'project'
		], $result);
	}


	public function testSingleCheck() {
		(new EnvLoader(__DIR__, 'env.txt'))
			->addRequired('NOT_EMPTY')
			->load();

		$this->assertArrayHasKey('NOT_EMPTY', $_ENV);
		$this->assertArrayHasKey('EMPTY', $_ENV);
	}

	public function testArrayCheck() {
		(new EnvLoader(__DIR__, 'env.txt'))
			->addRequired(['NOT_EMPTY', 'ANOTHER_ONE'])
			->load();

		$this->assertArrayHasKey('NOT_EMPTY', $_ENV);
		$this->assertArrayHasKey('ANOTHER_ONE', $_ENV);
	}


	public function testEmptyNonEmptyFail() {
		$this->expectExceptionMessage("EMPTY should not be empty");
		$this->expectException(\PP\Properties\EnvLoaderException::class);

		(new EnvLoader(__DIR__, 'env.txt'))
			->addRequired(['EMPTY', 'NOT_EMPTY'])
			->load();
	}
}
