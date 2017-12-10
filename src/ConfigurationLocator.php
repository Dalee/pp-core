<?php

namespace PP;

use Symfony\Component\Config\FileLocatorInterface;
use Symfony\Component\Config\Exception\FileLocatorFileNotFoundException;

/**
 * Class ConfigurationLocator.
 *
 * @package PP
 */
class ConfigurationLocator {

	/**
	 * @var FileLocatorInterface
	 */
	protected $locator;

	/**
	 * ConfigurationLocator constructor.
	 *
	 * @param FileLocatorInterface $locator
	 */
	public function __construct(FileLocatorInterface $locator) {
		$this->locator = $locator;
	}

	/**
	 * Locates the file.
	 *
	 * @param string $name
	 * @param bool $first
	 * @return array|string
	 */
	public function locate($name, $first = true) {
		return $this->locator->locate($name, null, $first);
	}

	/**
	 * Same as locate but suppress all exceptions if file's absent.
	 *
	 * @param string $name
	 * @param bool $first
	 * @return array|string
	 */
	public function locateQuiet($name, $first = true) {
		$paths = $first ? '' : [];

		try {
			$paths = $this->locate($name, $first);
		} catch (FileLocatorFileNotFoundException $ex) {
			//
		}

		return $paths;
	}

}
