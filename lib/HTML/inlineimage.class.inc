<?php
	/**
	 * 
	 * CONFIG.INLINEIMAGES 0/off/1/on (activate module)
	 * CONFIG.INLINEIMAGES_MAX_TRESHOLD 2048 (max size in bytes of inlined file, optional)
	 * CONFIG.CDN_LIST = "fully-qualified-domain1, fully-qualified-domain2" 
	 * 		(list of servers ready to serve static content)
	 */
	if (!defined('BASEPATH')) {
		FatalError('BASEPATH is not defined');
	}

	require_once (BASEPATH . 'libpp/lib/HTML/assets.class.inc');

	class PXHtmlImageTag {

		const INLINE_MAX_TRESHOLD = 4096;
		const GIF1PX = 'data:image/gif;base64,R0lGODlhAQABAIAAAP///////yH5BAEKAAEALAAAAAABAAEAAAICTAEAOw=='; // 1px gif

		// singleton
		static private $instance = null;

		/**
		 * getInstance
		 * @static
		 * @return PXHtmlImageTag
		 */
		public static function getInstance() {
			self::$instance || self::$instance = new PXHtmlImageTag();
			return self::$instance;
		}

		private function __clone() {
			// deny cloning
		}


		static private $assets = null;

		protected $enabled = false;

		protected $tagCachedData      = null;
		protected $propertyCachedData = null;

		protected $allowedExt   = null;
		protected $allowedDir   = null;

		protected $maxTreshold  = null;

		// deny instant creating
		private function __construct() {
			self::$assets = PXHTMLAssets::getInstance();

			$this->tagCachedData = array();
			$this->propertyCachedData = array();

			$this->allowedExt = array(
				'jpg' => 'image/jpeg',
				'jpeg' => 'image/jpeg',
				'png' => 'image/png',
				'gif' => 'image/gif',
			);

			$this->enabled = self::$assets->enabled && self::$assets->inlineimages_enabled;

			// final initialize
			$this->initialize();
		}

		/**
		 * init
		 */
		protected function initialize () {
			// define max treshold
			$this->maxTreshold = $this->setMaxTreshold(self::$assets->inline_images_max_treshold);
		}

		public function setMaxTreshold ($maxTreshold) {
			$maxTreshold = (int)($maxTreshold);
			$this->maxTreshold = ($maxTreshold > 0)? $maxTreshold : self::INLINE_MAX_TRESHOLD;
		}

		/**
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
		 */
		protected function _realTagBuild ($imageData) {
			$attrString = $this->_buildAttrString($imageData);
			return '<img ' . $attrString . ' />';
		}

		public function __checkFile($parts) {
			if (empty($parts['path'])) {
				return false;
			}
			$extension = strtolower(pathinfo($parts['path'], PATHINFO_EXTENSION));
			return isset($this->allowedExt[$extension]);
		}

		/**
		 * 
		 * @param array $imageData
		 */
		protected function rebuildImageData (&$imageData) {

			/* some logic processed */
			$imageDestination = self::$assets->findLocalPathByURL($imageData['src'], array($this, '__checkFile'));
			if ($imageDestination === null) {
				return;
			}

			$statData = stat($imageDestination);
			$metaData = getimagesize($imageDestination);

			$forceSize = isset($imageData['force_size']) && $imageData['force_size'];
			$forceInline = isset($imageData['force_inline']) && $imageData['force_inline'];

			unset ($imageData['force_size'], $imageData['force_inline']);

			if (!(is_array($statData) && is_array($metaData))) {
				return;
			}

			$inline = $forceInline || ($statData['size'] <= self::INLINE_MAX_TRESHOLD);

			$imageData['src'] = $inline
				? $this->_getInlineData($imageDestination, $metaData['mime'])
				: self::$assets->cdnize($imageData['src']);

			if ($forceSize) {
				$imageData['width'] = $metaData[0];
				$imageData['height'] = $metaData[1];
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
			if (!$this->enabled) {
				return $this->_realTagBuild($imageData);
			}

			if (empty($imageData['src'])) {
				$imageData['src'] = self::GIF1PX;
				return $this->_realTagBuild($imageData);
			}

			$imageData['src'] = (string)$imageData['src'];

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
			if (!self::$assets->inlineimages_enabled) {
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