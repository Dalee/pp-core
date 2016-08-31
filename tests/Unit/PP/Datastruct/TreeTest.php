<?php

namespace Tests\Unit\PP\Datastruct;

use PP\Lib\Datastruct\Leaf;
use PP\Lib\Datastruct\Tree;
use Tests\Base\AbstractUnitTest;

/**
 * Class TreeTest
 * @package Tests\Unit\PP\Datastruct
 */
class TreeTest extends AbstractUnitTest {

	/**
	 * Ensure tree can be build without orphans
	 */
	public function testToTableWithoutOrphans() {
		$tree = new Tree([
			['id' => 1, 'parent' => null, 'title' => 'The Root'],
			['id' => 5, 'parent' => 3, 'title' => 'Contacts'],
			['id' => 2, 'parent' => 1, 'title' => 'News'],
			['id' => 3, 'parent' => 1, 'title' => 'About Us'],
			['id' => 4, 'parent' => 7, 'title' => 'Orphan'],
		]);

		$result = $tree->toTable();
		$this->assertEquals([
			1 => ['id' => 1, 'parent' => null, 'title' => 'The Root'],
			2 => ['id' => 2, 'parent' => 1, 'title' => 'News'],
			3 => ['id' => 3, 'parent' => 1, 'title' => 'About Us'],
			5 => ['id' => 5, 'parent' => 3, 'title' => 'Contacts'],
		], $result);
	}

	/**
	 * Ensure tree can be build with orphans
	 */
	public function testToTableWithOrphans() {
		$tree = new Tree([
			['id' => 1, 'parent' => null, 'title' => 'The Root'],
			['id' => 5, 'parent' => 3, 'title' => 'Contacts'],
			['id' => 2, 'parent' => 1, 'title' => 'News'],
			['id' => 3, 'parent' => 1, 'title' => 'About Us'],
			['id' => 4, 'parent' => 7, 'title' => 'Orphan'],
		], 'id', 'parent', 'title', true);

		$result = $tree->toTable();
		$this->assertEquals([
			1 => ['id' => 1, 'parent' => null, 'title' => 'The Root'],
			2 => ['id' => 2, 'parent' => 1, 'title' => 'News'],
			3 => ['id' => 3, 'parent' => 1, 'title' => 'About Us'],
			4 => ['id' => 4, 'parent' => 7, 'title' => 'Orphan'],
			5 => ['id' => 5, 'parent' => 3, 'title' => 'Contacts'],
		], $result);
	}

	/**
	 * Ensure leafs can added and removed
	 */
	public function testAddRemoveLeafs() {
		$tree = new Tree([]);
		$result = $tree->toTable();
		$this->assertEquals([], $result);

		// add leaf
		$tree->addLeaf(new Leaf(1, 'hello', null, [], $tree));
		$result = $tree->toTable();

		$this->assertEquals([
			1 => ['id' => 1, 'title' => 'hello', 'parent' => null],
		], $result);

		// remove leaf
		$tree->removeLeaf(1);
		$result = $tree->toTable();
		$this->assertEquals([], $result);
	}

	/**
	 * Ensure orphan leafs can't be added
	 */
	public function testAddOrphanLeaf() {
		$tree = new Tree([]);
		$result = $tree->toTable();
		$this->assertEquals([], $result);

		// add leaf with saveOrphans = false
		$tree->addLeaf(new Leaf(1, 'hello', 5, [], $tree));
		$result = $tree->toTable();
		$this->assertEquals([], $result);

		// add leaf with saveOrphans = true
		$tree->setSaveOrphans(true);
		$tree->addLeaf(new Leaf(1, 'hello', 5, [], $tree));
		$result = $tree->toTable();
		$this->assertEquals([
			1 => ['id' => 1, 'title' => 'hello', 'parent' => 5],
		], $result);
	}

}
