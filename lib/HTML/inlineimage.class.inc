<?php
	/**
	 * 
	 * CONFIG.INLINE_IMAGES 0/1 (activate module)
	 * CONFIG.INLINE_IMAGES_MAX_TRESHOLD 2048 (max size in bytes of inlined file, optional)
	 * CONFIG.INLINE_CDN_LIST = "fully-qualified-domain1, fully-qualified-domain2" 
	 * 		(list of servers ready to serve static content)
	 */
	if (!defined('BASEPATH')) {
		FatalError('BASEPATH is not defined');
	}

	class PXHtmlImageTag {
		const INLINE_MAX_TRESHOLD = 4096;

		static private $instance = null;

		protected $tagCachedData = null;
		protected $propertyCachedData = null;
		protected $allowedExt = null;
		protected $allowedDir = null;
		protected $maxTreshold = null;
		protected $config = null;
		protected $cdnDomains = null;
		protected $cdnEnabled = false;
		protected $maxIndex = 0;

		//
		public static function getInstance() {
			if (self::$instance == null) {
				self::$instance = new PXHtmlImageTag();
			}

			return self::$instance;
		}


		//
		private function __clone() {
		}
		
		// 
		private function __construct() {
			$this->tagCachedData = array();
			$this->propertyCachedData = array();

			$this->allowedExt = array(
					'jpg' => 'image/jpeg', 
					'jpeg' => 'image/jpeg', 
					'JPEG' => 'image/jpeg', 
					'JPG' => 'image/jpeg', 
					'png' => 'image/png', 
					'PNG' => 'image/png', 
					'gif' => 'image/gif', 
					'GIF' => 'image/gif', 
				);

			$this->allowedDir = array(
					'site/htdocs',
					'local/htdocs',
					/* 'libpp/htdocs', */
				);

			// current CDN selector
			$this->currentIndex = 0;
			$this->maxTreshold = self::INLINE_MAX_TRESHOLD;


			// config and final initialize
			$this->_config = array();
			$this->loadConfig();
			$this->initialize();
		}

		/**
		 *
		 *
		 *
		 */
		protected function loadConfig () {
			$configFile = BASEPATH . 'site/etc/properties.ini';
			if (!file_exists($configFile)) {
				FatalError("Can't open {$configFile}");
			}

			$this->config = parse_ini_file($configFile, true);
		}

		/**
		 *
		 *
		 *
		 */
		public function getProperty ($propertyName, $defaultValue = null) {
			$ret = $defaultValue;
			if (array_key_exists($propertyName, $this->config)) {
				$ret = $this->config[$propertyName];
			}
			return $ret;
		}

		/**
		 *
		 *
		 *
		 */
		protected function initialize () {
			// define max treshold
			$this->maxTreshold = (intval($this->getProperty('CONFIG.INLINE_IMAGES_MAX_TRESHOLD', 0)));
			$this->maxTreshold = ($this->maxTreshold > 0) ? $this->maxTreshold : self::INLINE_MAX_TRESHOLD;

			// loading CDN list 
			$rawDomainList = array_filter($this->getProperty('CONFIG.CDN_LIST', array()));
			$this->cdnDomains = array();
			foreach ($rawDomainList as $_ => $domainName) {
				$domainName = trim($domainName);
				if(empty($domainName)) {
					continue;
				}
				$this->cdnDomains[] = $domainName;
			}

			$this->maxIndex = count($this->cdnDomains);
			if($this->maxIndex > 0) {
				$this->cdnEnabled = true;
			}
		}
		
		public function setMaxTreshold ($maxTreshold) {
			$this->maxTreshold = intval($maxTreshold);
		}


		/**
		 * @param requestURI string
		 * 
		 * @return null or full path to image
		 */
		protected function _mapRequestURI ($requestURI) {

			$requestURI = urldecode($requestURI);
			$pathPieces = explode('/', $requestURI);

			$pathPieces = array_filter($pathPieces);
			foreach ($pathPieces as $idx => $pathPiece) {
				if (!preg_match('/^[\.\S\#]+$/', $pathPiece)) {
					unset($pathPieces[$idx]);
				}
			}

			$pathPieces = array_values($pathPieces);
			$fileName = (count($pathPieces) > 0) ? 
				$pathPieces[count($pathPieces) - 1] : null;

			if ($fileName == null) {
				return null;
			}

			$filenameParts = explode('.', $fileName);
			$fileExt = (count($filenameParts) > 0) ?
				$filenameParts[count($filenameParts) - 1] : null;

			if ($fileExt == null) {
				return null;
			}

			if (!array_key_exists($fileExt, $this->allowedExt)) {
				return null;
			}

			$fileURI = implode('/', $pathPieces);
			$filePath = null;
			foreach($this->allowedDir as $_ => $dirPart) {
				$checkPath = BASEPATH . $dirPart . '/' . $fileURI;
				if (file_exists($checkPath)) {
					$filePath = $checkPath;
					break;
				}
			}

			return $filePath;
		}


		/**
		 *
		 *
		 *
		 */
		protected function _getInlineData ($fileName, $mimeType) {
			$rawData = base64_encode(file_get_contents($fileName));
			$rawData = 'data:'.$mimeType.';base64,'.$rawData;
			return $rawData;
		}

		/**
		 * @param attrData array
		 * convert array into string a-la 'key1="value1" key2="value2"'
		 *
		 */
		protected function _buildAttrString ($attrData) {
			$attrStrings = array();

			foreach ($attrData as $key => $value) {
				$attrStrings[] = $key . '="' . htmlspecialchars($value) . '"';
			}

			return implode(' ', $attrStrings);
		}

		/**
		 *
		 *
		 *
		 */
		protected function _realTagBuild ($imageData) {
			$attrString = $this->_buildAttrString($imageData);
			return '<img ' . $attrString . ' />';
		}

		/**
		 *
		 *
		 *
		 */
		public function buildCdnUrl ($imageSource) {
			if (!$this->cdnEnabled) {
				return $imageSource;
			}

			$code = ord(md5($imageSource));
			$domainIndex = ($code % $this->maxIndex);

			$link = $this->cdnDomains[$domainIndex] . $imageSource;
			return $link;
		}

		/**
		 *
		 *
		 *
		 */
		protected function rebuildImageData (&$imageData) {
			/* some logic processed */
			$imageDestination = $this->_mapRequestURI($imageData['src']);
			if ($imageDestination !== null) {
				$statData = stat($imageDestination);
				$metaData = getimagesize($imageDestination);

				$forceSize = (empty($imageData['force_size'])) ? false : (!!$imageData['force_size']);
				$forceInline = (empty($imageData['force_inline'])) ? false : (!!$imageData['force_inline']);

				unset($imageData['force_size']);
				unset($imageData['force_inline']);
				
				if (is_array($statData) && is_array($metaData)) {
					if (!$forceInline) {
						$forceInline = (intval($statData['size'])	<= self::INLINE_MAX_TRESHOLD) ? true : false;
					}

					if ($forceInline) {
						$imageData['src'] = $this->_getInlineData($imageDestination, $metaData['mime']);
					}

					if (!$forceInline && $this->cdnEnabled) {
						$imageData['src'] = $this->buildCdnUrl($imageData['src']);
					}

					if ($forceSize) {
						$imageData['width'] = $metaData[0];
						$imageData['height'] = $metaData[1];
					}
				}
			}
		}


		/**
		 * @param imageData array
		 * <code>
		 *	$imageData = array (
		 *		'src' => image source path (optional)
		 *		'title' => image title (optional)
		 *		'width' => image width (optional)
		 *		'height' => image height (optional)
		 *		'force_size' => force output image weight and height (optional)
		 *		'force_inline' => force inline output (optional)
		 *  )
		 * </code>
		 *
		 */
		public function buildTag ($imageData) {
			if (intval($this->getProperty('CONFIG.INLINE_IMAGES')) === 0) {
				return $this->_realTagBuild($imageData);
			}

			$imageData['src'] = (empty($imageData['src'])) ? '' : strval($imageData['src']);
			if (empty($imageData['src'])) {
				$imageData['src'] = '#';
				return $this->_realTagBuild($imageData);
			}

			$cacheKey = implode('_', $imageData);
			if (array_key_exists($cacheKey, $this->tagCachedData)) {
				return $this->tagCachedData[$cacheKey];
			}

			/* building, and caching tag value */
			$this->rebuildImageData($imageData);
			$this->tagCachedData[$cacheKey] = $this->_realTagBuild($imageData);
			return $this->tagCachedData[$cacheKey];
		}


		protected function _realPropertyBuild($imageData) {
			return "url('{$imageData['src']}')";
		}

		/**
		 * Builds CSS property
		 *
		 * @param imageData array
		 * <code>
		 *	$imageData = array (
		 *		'src' => image source path (optional)
		 *  );
		 *
		 * FIXME: not work with ../path/to/image :(
		 */
		public function buildProperty ($imageData) {
			if (intval($this->getProperty('CONFIG.INLINE_IMAGES')) === 0) {
				return $this->_realPropertyBuild($imageData);
			}

			$cacheKey = implode('_', $imageData);
			if (array_key_exists($cacheKey, $this->propertyCachedData)) {
				return $this->propertyCachedData[$cacheKey];
			}

			/* building, and caching property value */
			$this->rebuildImageData($imageData);
			$this->propertyCachedData[$cacheKey] = $this->_realPropertyBuild($imageData);
			return $this->propertyCachedData[$cacheKey];
		}

	}

?>