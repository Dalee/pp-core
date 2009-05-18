<?php
class ObjectCache extends PXCacheFile { //FIXME ! Remove 'extends' ! It is only for back compatibility !
	
	// Factory
	public static function &get($cache, $cacheDomain = null, $defaultExpire = 3600){
		$klass = 'PXCache' . ucfirst($cache);
		if(!class_exists($klass)) FatalError("Caching method '{$cache}' not implemented !");
		$instance = new $klass($cacheDomain, $defaultExpire);
		return $instance;
	}
}

class NullCache extends PXCacheNull {} //FIXME ! See above
?>
