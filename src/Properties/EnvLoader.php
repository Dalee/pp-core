<?php

namespace PP\Properties;

use Dotenv\Dotenv;

/**
 * Class EnvMapper
 * @package PP\Properties
 */
class EnvLoader {

	public const TYPE_NOT_EMPTY = 0; // default

	/** @var string[] */
	protected $errors;

	/** @var string[int] */
	protected $required;

	/** @var bool */
	protected $valid;

	/** @var string */
	protected $path;

	/** @var string */
	protected $file;

	/**
	 * EnvLoader constructor.
	 *
	 * @param string $path
	 * @param string $file
	 */
	public function __construct($path, $file = '.env') {
		$this->path = $path;
		$this->file = $file;

		$this->errors = [];
		$this->required = [];
	}

	/**
	 * Perform ENV injection
	 * it's safe to call it multiple times
	 */
	public static function inject() {
		(new EnvLoader(BASEPATH))
			->addRequired(['DATABASE_DSN'])
			->load();
	}

	/**
	 * Add key or list of keys to be required in environment,
	 * otherwise, exception will be raised.
	 *
	 * @param string|string[] $key
	 * @return $this
	 */
	public function addRequired($key) {
		if (!is_array($key)) {
			$key = [$key];
		}

		foreach ($key as $item) {
			$this->required[$item] = static::TYPE_NOT_EMPTY;
		}

		return $this;
	}

	/**
	 * Load environment variables
	 *
	 * @throws EnvLoaderException
	 */
	public function load() {
		$envFile = join(DIRECTORY_SEPARATOR, [rtrim($this->path, DIRECTORY_SEPARATOR), $this->file]);
		if (file_exists($envFile)) {
			$dotenv = new Dotenv($this->path, $this->file);
			$dotenv->overload();
		}

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

	/**
	 * Return single value of key.
	 *
	 * @param string $key
	 * @return string
	 * @throws EnvLoaderException
	 */
	public static function get($key) {
		if (isset($_ENV[$key])) {
			return $_ENV[$key];
		}

		return null;
	}

	/**
	 * Just validation loop
	 */
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

	/**
	 * @param string $key
	 * @return bool
	 */
	protected function isNotEmpty($key) {
		$res = (isset($_ENV[$key])) && (strlen($_ENV[$key]) > 0);

		if (!$res) {
			$this->errors[] = "${key} should not be empty";
		}

		return $res;
	}
}
