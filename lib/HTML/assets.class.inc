<?php
/**
 * Assets manager
 *
 * [CONFIG.INLINE_CDN_LIST]
 * # list of servers ready to serve static content
 * server1 = "//fully-qualified-domain1"
 * server2 = "//fully-qualified-domain2"
 * ... 
 */
if (!defined('BASEPATH')) {
	FatalError('BASEPATH is not defined');
}

/**
 * PXHTMLAssets class
 * @property-read bool $enabled Main trigger
 * @property-read array $cdn_domains FQDNs list
 * @property-read bool $inline_images Trigger for inlining images
 * @property-read int $inline_images_max_treshold Maximal size in bytes to force inlining of images
 * @property-read bool $delayed_print Lazy print of makeBundles. True by default
 * @property-read array $delayed_assets 
 */
final class PXHTMLAssets {

	// singleton, yeah
	private static $instance;
	public static function getInstance () {
		self::$instance || self::$instance = new PXHTMLAssets();
		return self::$instance;
	}
	private function __clone() {
		// deny cloning
	}

	/**
	 * Magic part of town
	 */
	public function __get ($k) {
		$k = (string)($k);
		static $cache = array();

		if (array_key_exists($k, $cache)) {
			return $cache[$k];
		}

		$v = null;
		switch ($k) {
			case 'enabled':
			case 'assets_enabled':
				$v = parseBool($this->getProperty('CONFIG.ASSETS_ENABLED', 0));
				break;
			case 'assets_yui':
				$v = parseBool($this->getProperty('CONFIG.ASSETS_YUI', $this->getProperty('CONFIG.ASSETS_USE_YUI', 0)));
				break;
			case 'assets_gzip_on':
				$v = (int)($this->getProperty('CONFIG.ASSETS_GZIP_ON', 0));
				break;
			case 'delayed_print':
				$v = parseBool($this->getProperty('CONFIG.ASSETS_DELAYED_PRINT', 1));
				break;

			case 'delayed_assets':
				$v = self::$delayed;
				break;

			case 'inlineimages_enabled':
				$v = parseBool($this->getProperty('CONFIG.INLINEIMAGES_ENABLED', $this->getProperty('CONFIG.INLINE_IMAGES', 0)));
				break;
			case 'inlineimages_max_treshold':
			case 'inline_images_max_treshold':
				$v = (int)($this->getProperty('CONFIG.INLINEIMAGES_MAX_TRESHOLD', $this->getProperty('CONFIG.INLINE_IMAGES_MAX_TRESHOLD', 0)));
				break;

			case 'cdn_enabled':
				return $this->cdnEnabled;

			case 'cdn_list':
			case 'cdn_domains':
				// o-ho-ho here. php is so dirty. truly shit but it works
				$v = array_map('trim', (array)($this->getProperty('CONFIG.CDN_DOMAINS', $this->getProperty('CONFIG.CDN_LIST'))));
				$v = array_values(array_filter($v)); // drop empty values and also keys
				break;

			case 'allowed_paths':
				return $this->allowedPaths;

			default:
				trigger_error('Property '.$k.' is not defined.', E_USER_ERROR);
				return;
		}

		return ($cache[$k] = $v);
	}



	protected $config;

	/**
	 * loading configuration file
	 */
	protected function loadConfig () {
		$file = pp_fetch_file('etc', 'properties.ini');
		$file || FatalError("Can't open ./etc/properties.ini");
		$this->config = parse_ini_file($file, true);
	}

	public function getProperty ($propertyName, $defaultValue = null) {
		$ret = $defaultValue;
		if (array_key_exists($propertyName, $this->config)) {
			$ret = $this->config[$propertyName];
		}
		return $ret;
	}




	private function __construct () {

		// config and final initialize
		$this->loadConfig();
		$this->initialize();
	}

	protected function initialize () {

		$this->allowedPaths = array_map('realpath', pp_base_paths('htdocs')); // there is no assets in pp by now but ok

		$this->cdnDomains = $this->cdn_list;

		if (!$this->enabled) { // if assets disabled, do not enable cdn
			return;
		}

		$this->cdnCount = count($this->cdnDomains);
		$this->cdnEnabled = ($this->cdnCount > 0);

	}



// cdn logic below

	protected $allowedPaths  = array();
	protected $cdnDomains   = array();
	protected $cdnEnabled   = false;
	protected $cdnCount     = 0;

	/**
	 * CDNize url
	 * @param string $url of some resource (img, js, css, font, swf)
	 * @param callable $filter a callback to filter  
	 * @return null or full path to image
	 */
	public function findLocalPathByURL ($url, $filter = null) {

		// because of bug in php 5.4.6 and above we must strip host manually
		// todo: this can be bug with user:pass values. but i think we don't need it here
		if (!preg_match('#^(?:(?P<scheme>https?:)?//(?P<host>[^/]+))?(?P<relative>/.+?)$#', ltrim($url), $murl)) {
			return null;
		}

		$parts = parse_url($murl['relative']);
		empty($parts['host'])   && $parts['host'] = $murl['host'];
		empty($parts['scheme']) && $parts['scheme'] = $murl['scheme'];

		// don't touch external urls 
		if (!empty($parts['host'])) { //todo: || $parts['host'] not in $localDomains
			return null;
		}

		$baseName = pathinfo($parts['path'], PATHINFO_BASENAME);
		$extension = strtolower(pathinfo($parts['path'], PATHINFO_EXTENSION));

		if (empty($baseName) || (is_callable($filter) && !call_user_func($filter, $parts))) {
			return null;
		}

		$fileURL = $parts['path'];
		$filePath = null;
		foreach ($this->allowedPaths as $_ => $path) {
			$testPath = realpath($path . '/' . $fileURL);
			if (strpos($testPath, $path) === 0 && file_exists($testPath)) {
				$filePath = $testPath;
				break;
			}
		}

		return $filePath;
	}

	public function applyDelayed ($content) {
		$from = array();
		$to = array();

		foreach ($this->delayed_assets as $type => $list) {
			foreach ($list as $group => $true) {
				$from[] = $this->makePlaceholder($type, $group);
				$to[] = $this->import(array(
					'tag' => $type,
					'asset_group' => $group,
					'asset_print' => true,
					'asset_print_delayed' => true,
				));
			}
		}

		return str_replace($from, $to, $content);
	}

	public function makePlaceholder ($type, $group) {
		return sprintf('<!--# asset %s %s #-->', $type, $group);
	}

	/**
	 * cdnize uri if possible
	 * @param string $uri of some resource (img, js, css, font, swf)
	 * @return string cdnized or unmodified uri
	 */
	public function cdnize ($url) {
		if (!$this->cdnEnabled) {
			return $url;
		}

		$code = hexdec(substr(md5($url), -3)); // 1, 3, 5, 7 are the best values here relates on cdns qty
		$domainIndex = ($code % $this->cdnCount);

		$link = $this->cdnDomains[$domainIndex] . $url;

		return $link;
	}



// worker here below

	// import uniquely
	protected static $imported = array();
	protected static $delayed = array();

	protected static $tags = array(
		'style' => array(
			'type'           => 'css',
			'pattern'        => '<link rel="stylesheet" type="text/css" href="%s" %s />',
			'pattern_inline' => "<style type=\"text/css\" %2\$s>\n%1\$s\n</style>",
			'delimiter'      => "\n",
			// 'default_bundle' => 'head',
		),
		'script' => array(
			'type'           => 'js',
			'pattern'        => '<script type="text/javascript" src="%s" %s></script>',
			'pattern_inline' => "<script type=\"text/javascript\" %2\$s>\n//<![CDATA[\n%1\$s\n//]]>\n</script>",
			'delimiter'      => ";",
			// 'default_bundle' => 'foot',
		),
	);

	public function import ($params) {

		require_once (BASEPATH . 'libpp/lib/HTML/assetsbundler.class.inc');

		if (empty($params['tag'])) {
			FatalError ('broken assets? wtf!');
			return;
		}

		$types = array_flat(self::$tags, null, 'type');

		$tag_key = $params['tag'];
		$tag_key = isset($types[$tag_key])? $tag_key : (in_array($tag_key, $types) ? array_search($tag_key, $types) : null);

		if ((empty($params['src']) && empty($params['print_asset']) && empty($params['asset_print'])) || empty(self::$tags[$tag_key])) {
			return;
		}

		$tag_data = self::$tags[$tag_key];

		$extra_params     = array();
		$extra_attributes = "";
		$asset_mode       = $this->enabled && empty($params['noasset']);
		$print_tag        = !$asset_mode;

		foreach ($params as $param => $value) {
			switch ($param) {
				case 'tag':
				case 'src':
				case 'noasset':
				case 'asset_group':
				case 'print_asset':
				case 'asset_print':
				case 'asset_print_delayed':
				case 'inline':
				case 'unique':
					break;
				default:
					$extra_params[] = sprintf('%s="%s"', convertStringToDashed($param), htmlspecialchars($value));
			}
		}

		if (sizeof($extra_params) != 0) {
			$extra_attributes = implode(' ', $extra_params);
		}

		$asset_id     = '';
		$assetH       = null;
		$assets_group = isset($params['asset_group']) ? $params['asset_group'] : null;
		$assets_dir   = ''; //FIXME
		$assets_print = !empty($params['print_asset']) || !empty($params['asset_print']);

		$inline       = isset($params['inline']) ? $params['inline'] : null;

		$assets_delayed = $this->delayed_print && empty($params['asset_print_delayed']);

		if ($asset_mode) {
			$assetH = PXHtmlAssetsBundler::getInstance(BASEPATH . '/site/htdocs' . $assets_dir, $this->allowedPaths);
		}

		$is_bundle = false;
		switch (true) {
			case $inline && strpos($params['src'], 'http') !== 0:
				foreach ($this->allowedPaths as $localpath) {
					if (file_exists($localfile = $localpath . $params['src'])) {
						$params['content'] = file_get_contents($localfile);
						$print_tag = true;
						break;
					}
				}
				break;

			case $assetH && $assets_print && $assets_delayed:
				$out = $this->makePlaceholder($tag_data['type'], $assets_group);
				self::$delayed[$tag_data['type']][$assets_group] = true;
				$print_tag = true;
				break;

			case $assetH && $assets_print:
				list($fullPath, $localPath, $mtime) = $assetH->makeAssetsBundle($tag_data['type'], $tag_data['delimiter'], $assets_group);
				if (!empty($localPath)) {
					$asset_id      = $mtime;
					$params['src'] = $assets_dir . $localPath;
					$print_tag = true; //allow write result asset tag in asset mode
					$is_bundle = true;
				}
				break;

			case !empty($params['src']) && strpos($params['src'], 'http') !== 0:
				$print_tag = false;
				foreach ($this->allowedPaths as $localpath) {
					if (file_exists($localfile = $localpath . $params['src'])) {
						if ($assetH) {
							$assetH->addFileToBundle($localfile, $tag_data['type'], $assets_group);
							break;
						}
						$asset_id = filemtime($localfile);
						$print_tag = true;
						break;
					}
				}
				break;

			case $assets_print && !$asset_mode:
				$print_tag = false; //skip empty print of {html_import asset_print=1 ...} when asset_mode turned off
				break;

			default:
				$print_tag = true;
		}

		if (!empty($params['unique']) && isset(self::$imported[$params['src']])) {
			return;
		}
		self::$imported[$params['src']] = true;

		if (!$print_tag) {
			return '';
		}


		if (!empty($out)) {
			// dummy

		} elseif ($inline) {
			$out = sprintf($tag_data['pattern_inline'], $params['content'], $extra_attributes);

		} else {
			$resource_uri = $params['src'];
			if ($this->findLocalPathByURL($resource_uri)) {
				$resource_uri = $this->cdnize($resource_uri);
			}
			if ($asset_id && !$is_bundle && strpos($resource_uri, '?') === false) {
				$resource_uri .= sprintf('?_=%s', $asset_id);
			}

			$out = sprintf($tag_data['pattern'], $resource_uri, $extra_attributes);
		}

		return $out;
	}
}