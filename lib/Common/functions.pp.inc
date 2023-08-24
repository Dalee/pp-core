<?php

/**
 * Checks for registry instance
 * @return bool
 */
function pp_loaded() {
	return class_exists('PXRegistry');
}

/**
 * pp_paths returns proxima's base paths or full paths for subpath
 * @param string $sub subpath
 * @param bool $withPlugins adds plugins directories to paths list
 * @return array
 */
function pp_base_paths($sub = null, $withPlugins = false) {
	$sub && $sub = ltrim(trim($sub, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
	$ck = $sub . intval($withPlugins);

	static $cache = [];
	if (!empty($cache[$ck])) {
		return $cache[$ck];
	}

	$r = [];

	$r[] = SHAREDPATH . $sub;
	$r[] = LOCALPATH . $sub;

	if ($withPlugins && pp_loaded()) {
		foreach (pp_plugins() as $plugin) { // probably we can integrate it somehow
			$r[$plugin['tag']] = $plugin['path'] . $sub;
		}
	}

	$r[] = PPCOREPATH . $sub;

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
function pp_fetch_file(string|array $where, string|array $what, string|array $postfix = '', $ext = '.inc') {
	$ext = (isset($ext[0]) && $ext[0] == '.' ? '' : '.') . $ext;

	$where = (array)($where);
	$what = (array)($what);
	$postfix = (array)($postfix);

	// normalize paths
	$fullwhere = [];
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
				if (mb_substr((string) $file, -4) != $p . $ext) {
					$filenames = [
						$path . $file . $p . $ext,
						$path . $file
					];
				} else {
					$filenames = [$path . $file];
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
		return [];
	}

	$result = [];
	foreach (PXRegistry::getApp()->plugins as $tag => $plugin) {
		$result[] = [
			'tag' => $tag,
			'name' => $plugin->getName(),
			'path' => pathinfo((string) $plugin->getPathToPlugin(), PATHINFO_DIRNAME) . DIRECTORY_SEPARATOR,
		];
	}
	return $result;
}
