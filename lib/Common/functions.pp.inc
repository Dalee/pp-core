<?php

/**
 * pp_paths returns proxima's base paths or full paths for subpath
 * @param string $sub subpath
 * @return array
 */
function pp_base_paths($sub = null) {
	$sub && $sub = ltrim(trim($sub,'/') . '/', '/');

	static $cache = array();
	if (!empty($cache[$sub])) {
		return $cache[$sub];
	}

	$r = array();

//	$r[] = SHAREDPATH . $sub; // uncomment it if possible
	$r[] = LOCALPATH . $sub;

//	foreach (pp_modules() as $module) { // probably we can integrate it somehow
//		$r[] = $module->path . $sub;
//	}

	$r[] = PPPATH . $sub;

	$cache[$sub] = $r;

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
		if (empty($wherev[0]) || $wherev[0] != '/') {
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
