<?php

namespace PP\Properties;

use Dotenv\Dotenv;

/**
 * Class EnvMapper
 * @package PP\Properties
 */
class EnvLoader {

	const TYPE_NOT_EMPTY = 0; // default

	/** @var Dotenv */
	protected $dotenv;

	/** @var string[] */
	protected $errors;

	/** @var string[int] */
	protected $required;

	/** @var bool */
	protected $valid;

	public function __construct($path, $file = '.env') {
		$this->dotenv = new Dotenv($path, $file);
		$this->errors = [];

		$this->required = [];
	}

	public function addRequired($key) {
		if (!is_array($key)) {
			$key = [$key];
		}

		foreach ($key as $item) {
			$this->required[$item] = static::TYPE_NOT_EMPTY;
		}

		return $this;
	}

	public function load() {
		$this->dotenv->load();
		$this->validate();

		if (!$this->valid) {
			throw new EnvLoaderException(join(', ', $this->errors));
		}
	}

	/**
	 * Build array from list of provided keys
	 *
	 * @param string[] $mappings
	 * @return array
	 */
	public static function getMappedArray($mappings) {
		$result = [];

		foreach ($mappings as $key => $mapped) {
			if (is_int($key)) { // list is here..
				$key = $mapped;
				$mapped = null;
			}

			if (isset($_ENV[$key])) {
				if ($mapped === null) {
					$result[$key] = $_ENV[$key];
				} else {
					$result[$mapped] = $_ENV[$key];
				}
			}
		}

		return $result;
	}

	protected function validate() {
		$validationResult = true;

		foreach ($this->required as $key => $flags) {
			switch (true) {
				case ($flags & static::TYPE_NOT_EMPTY) == static::TYPE_NOT_EMPTY:
					$validationResult = $validationResult && $this->isNotEmpty($key);
					break;
			}
		}

		$this->valid = $validationResult;
	}

	protected function isNotEmpty($key) {
		$res = (isset($_ENV[$key])) && (strlen($_ENV[$key]) > 0);

		if (!$res) {
			$this->errors[] = "${key} should not be empty";
		}

		return $res;
	}
}
