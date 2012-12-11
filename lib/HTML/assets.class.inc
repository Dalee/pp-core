<?php

	final class PXHtmlAssetsManager {
		
		private $assets = array();
		private $assetsRoot;
		private $allowedBase;
		private static $self;
		
		public static function getInstance(){
			if (!self::$self) {
				$args = func_get_args();
				$self = new ReflectionClass(__CLASS__);
				self::$self = $self->newInstanceArgs($args);
			}
			
			return self::$self;
		}
		
		public function __construct($assetsRoot, array $allowedBase) {
			$this->assetsRoot  = $assetsRoot;
			$this->allowedBase = $allowedBase;
		}
		
		public function addFileToBundle($name, $type, $group = null) {
			$group || $group = 'default';
			if (!ctype_alpha($type)) {
				return;
			}
			$isSafe = false;
			foreach($this->allowedBase as $base) {
				if (strpos(realpath($name), realpath($base)) === 0) {
					$isSafe = true;
					break;
				}
			}
			if (!$isSafe) {
				return;
			}
			$this->assets[$type][$group][] = $name;
		}
		
		public function makeAssetsBundle($type, $delim = "\n", $group = null) {
			$group || $group = 'default';
			if (empty($this->assetsRoot) || empty($this->assets[$type][$group])) {
				return;
			}
			
			$bundleName     = sprintf("asset-%s.%s", md5(join('|', $this->assets[$type][$group])), $type);
			$bundleRelPath  = "/{$type}/{$bundleName}";
			$bundleFullPath = $this->assetsRoot . $bundleRelPath;
			
			if (!file_exists($bundleFullPath)) {
				$bundleContent = array();
				foreach($this->assets[$type][$group] as $name) {
					$bundleContent[] = file_get_contents($name);
				}
				$bundleContent = join($delim, $bundleContent);
				$bundleTypeRoot = "{$this->assetsRoot}/{$type}";

				MakeDirIfNotExists($bundleTypeRoot);
				WriteStringToFile($bundleFullPath, $bundleContent);

				if(in_array($type, array('css', 'js')) && 
					PXRegistry::getApp()->getProperty('CONFIG.ASSETS_USE_YUI') && 
					($compressorBinary = FindSystemFile('yui-compressor'))) {
					// try to minify css and javascript
					$outputFile = tempnam($bundleTypeRoot, $bundleName);

					$cmd = array();
					$cmd[] = $compressorBinary; // /usr/bin/yui-compressor
					$cmd[] = $bundleFullPath;   // .../asset-....(js|css)
					$cmd[] = "--type {$type}";
					$cmd[] = "-o";
					$cmd[] = $outputFile;

					$cmd_string = implode(' ', $cmd);
					$statusCode = -1;
					$compressionResult = exec($cmd_string, $dummy, $statusCode);

					if((intval($statusCode) === 0)) {
						rename($outputFile, $bundleFullPath);
					}
				}
			}
			
			$this->assets[$type][$group] = array(); //Make an asset blank for future use
			
			return array($bundleFullPath, $bundleRelPath, filemtime($bundleFullPath));
		}
	}