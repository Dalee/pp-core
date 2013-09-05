<?php

function pp_loaded() {
	return class_exists('PXRegistry');
}

/**
 * pp_paths returns proxima's base paths or full paths for subpath
 * @param string $sub subpath
 * @return array
 */
function pp_base_paths($sub = null, $withPlugins = false) {
	$sub && $sub = ltrim(trim($sub, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
	$ck = $sub . intval($withPlugins);

	static $cache = array();
	if (!empty($cache[$ck])) {
		return $cache[$ck];
	}

	$r = array();

	$r[] = SHAREDPATH . $sub;
	$r[] = LOCALPATH . $sub;

	if ($withPlugins && pp_loaded()) {
		foreach (pp_plugins() as $plugin) { // probably we can integrate it somehow
			$r[$plugin['tag']] = $plugin['path'] . $sub;
		}
	}

	$r[] = PPPATH . $sub;

	$cache[$ck] = $r;

	return $r;
}

/**
 * pp_fetch_file searchs for file in all known places by special order
 * @param string|array $where subpath or an array of them
 * @param string|array $what filename or an array of them
 * @param string|array $postfix something like '.module' or '.trigger', '.cronrun'
 * @param string $ext
 * @return string pathname with the highest order
 * @todo optimize me. dont look file in unexistant paths at least
 */
function pp_fetch_file($where, $what, $postfix = '', $ext = '.inc') {
	$ext = (isset($ext[0]) && $ext[0] == '.' ? '' : '.') . $ext;

	$where = (array)($where);
	$what = (array)($what);
	$postfix = (array)($postfix);

	// normalize paths
	$fullwhere = array();
	foreach ($where as $k => $wherev) {
		if (empty($wherev[0]) || $wherev[0] != DIRECTORY_SEPARATOR) {
			$fullwhere = array_merge($fullwhere, pp_base_paths($wherev));
		} else {
			$fullwhere[] = $wherev;
		}
	}
	$where = $fullwhere;

	foreach ($where as $path) {
		if (!is_dir($path) || !is_readable($path)) {
			continue;
		}
		foreach ($what as $file) {
			foreach ($postfix as $p) {
				if (substr($file, -4) != $p . $ext) {
					$filenames = array(
						$path . $file . $p . $ext,
						$path . $file
					);
				} else {
					$filenames = array( $path . $file );
				}
				foreach ($filenames as $filename) {
					if (is_readable($filename) && is_file($filename)) {
						return $filename;
					}
				}
			}
		}
	}

	return false;
}

/**
 * fetchs all active plugins and some info about them
 */
function pp_plugins() {
	if (!pp_loaded()) {
		return array();
	}

	$result = array();
	foreach (PXRegistry::getApp()->plugins as $tag => $plugin) {
		$result[] = array(
			'tag' => $tag,
			'name' => $plugin->getName(),
			'path' => pathinfo($plugin->getPathToPlugin(), PATHINFO_DIRNAME) . DIRECTORY_SEPARATOR,
		);
	}
	return $result;
}
