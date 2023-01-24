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
  */
 public function __construct(FileLocatorInterface $locator) {
		$this->locator = $locator;
	}

	/**
  * Locates the file.
  *
  * @param string $name
  * @param bool $first
  */
 public function locate($name, $first = true): array|string {
		return $this->locator->locate($name, null, $first);
	}

	/**
  * Same as locate but suppress all exceptions if file's absent.
  *
  * @param string $name
  * @param bool $first
  */
 public function locateQuiet($name, $first = true): array|string {
		$paths = $first ? '' : [];

		try {
			$paths = $this->locate($name, $first);
		} catch (FileLocatorFileNotFoundException) {
			//
		}

		return $paths;
	}

}
