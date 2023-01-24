<?php

	final class PXHtmlAssetsBundler {

		// singleton
		private static ?\PXHtmlAssetsBundler $instance = null;

		/**
		 * getInstance
		 * @static
		 * @param string $root
		 * @param array $allowedPaths
		 * @return PXHtmlImageTag
		 */
		public static function getInstance () {
			if (!self::$instance) {
				$args = func_get_args();
				$reflection = new ReflectionClass(self::class);
				self::$instance = $reflection->newInstanceArgs($args);
			}
			return self::$instance;
		}

		private function __clone() {
			// deny cloning
		}

		// uniquely importing
		protected static $imported = [];

		// builder
		private array $bundles = [];
		private $root;

		public function __construct ($root, private readonly array $allowedPaths) {
			$this->root = realpath($root);
		}

		public function addFileToBundle ($name, $type, $group = null) {
			$group || $group = 'default';
			if (!ctype_alpha((string) $type)) {
				return;
			}
			$isSafe = false;
			$namepath = realpath($name);
			if (isset(self::$imported[$namepath])) {
				return;
			}
			foreach ($this->allowedPaths as $base) {
				if (str_starts_with($namepath, realpath($base))) {
					$isSafe = true;
					break;
				}
			}
			if (!$isSafe) {
				return;
			}
			self::$imported[$namepath] = true;
			$this->bundles[$type][$group][] = $name;
		}

		public function makeAssetsBundle ($type, $delim = "\n", $group = null) {
			$group || $group = 'default';

			if (empty($this->root) || empty($this->bundles[$type][$group])) {
				return;
			}

			$bundleContent = [];
			foreach ($this->bundles[$type][$group] as $name) {
				$bundleContent[] = sprintf("\n/* %s */\n%s", pathinfo((string) $name, PATHINFO_FILENAME), file_get_contents($name));
			}
			$bundleContent = join($delim, $bundleContent);

			$md5 = md5($bundleContent);
			$md5subdir = substr($md5, 0, 2);
			$bundleName     = sprintf("asset-%s.%s", $md5, $type);
			$bundleTypeRoot = "{$this->root}/{$type}/{$group}/{$md5subdir}";
			$bundleRelPath  = "/{$type}/{$group}/{$md5subdir}/{$bundleName}";
			$bundleFullPath = "{$bundleTypeRoot}/{$bundleName}";

			if (!file_exists($bundleFullPath)) {
				MakeDirIfNotExists($bundleTypeRoot);
				WriteStringToFile($bundleFullPath, $bundleContent);
			}

			// Clean up asset container for future
			unset($this->bundles[$type][$group]);

			return [$bundleFullPath, $bundleRelPath, filemtime($bundleFullPath)];
		}

	}

