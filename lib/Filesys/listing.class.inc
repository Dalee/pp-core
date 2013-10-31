<?php

include_once PPLIBPATH . 'Filesys/dir.class.inc';

class PXFileListingRoots extends PXFileListing {
	var $_comments;

	function getList($catalog) {
		return;
	}

	function setRoots($catalogs) {
		$this->_comments = array();

		foreach($catalogs as $catalog => $comment) {
			$catalog = $this->_prepare($catalog);

			$_root = new PXFileListingItemRoot($catalog, $this);
			$_root->_Comment($comment);

			$this->_files[$catalog] = $_root;
		}
	}

	function html() {
		$this->_decorator->setCatalog($this->_catalog);

		$_  = $this->_decorator->htmlHeader();
		$_ .= $this->_decorator->htmlList($this->_files);
		$_ .= $this->_decorator->htmlFooter();

		return $_;
	}

	function writable() {
		return false;
	}
}

class PXFileListing {
	var $_roots;
	var $_files;
	var $_catalog;
	var $_destination;

	function PXFileListing($catalogs) {
		$this->_roots = array();
		$this->_files = null;

		$this->setRoots($catalogs);
	}

	function setDecorator(&$htmlDecorator) {
		$this->_decorator =& $htmlDecorator;
	}

	function writable() {
		return is_writable($this->_fullPath($this->_catalog));
	}

	function html() {
		$up      = preg_replace('|[^/]+/$|', '', $this->_catalog);

		$canBrowse = $this->_isValid($this->_catalog);
		$canUp     = $this->_isValid($up);

		$this->_decorator->setCatalog($this->_catalog);

		$_  = $this->_decorator->htmlHeader();
		$_ .= $this->_decorator->htmlUp($canUp, $up);

		if(is_array($this->_files)) {
			$_ .= $this->_decorator->htmlList($this->_files);
			$_ .= $this->_decorator->htmlFooter();

		} else {
			$_ .= $this->_decorator->htmlFooter();
			$_ .= $this->_decorator->error();
		}

		return $_;
	}

	function setDestination($destination) {
		$destination = $this->_prepare($destination);

		if(!$this->_isValid($destination)) {
			$this->_destination = null;
			return false;
		}

		$this->_destination = $destination;
	}

	function setRoots($catalogs) {
		foreach(array_keys($catalogs) as $catalog) {
			$catalog = $this->_prepare($catalog);

			if(is_dir($this->_fullPath($catalog))) {
				$this->_roots[] = $catalog;
			}
		}
	}

	function getList($catalog) {
		$catalog = $this->_prepare($catalog);
		$this->_catalog = $catalog;
		if(!$this->_isValid($catalog)) {
			return null;
		}
		return $this->_ls($catalog);
	}

	# public interface
	public function isValid($catalog) {
		return $this->_isValid($catalog);
	}

	function _ls($catalog) {
		$this->_files = array();

		$catalog = $this->_prepare($this->_fullPath($catalog));
		$_dir = new NLDir($catalog);
		
		foreach($_dir->getDirs(true) as $path) {
			$this->_files[$path] = new PXFileListingItemCatalog($path, $this);
		}

		foreach($_dir->getFileNames(true) as $path) {
			$this->_files[$path] = new PXFileListingItem($path, $this);
		}

		return $this->_files;
	}
	
	function _getRealPath($catalog) {
		return realpath($this->_fullPath($catalog));
	}

	function _isValid($catalog) {
		if(!is_dir($this->_fullPath($catalog))) {
			return false;
		}
		
		foreach($this->_roots as $root) {
			if(mb_strpos($this->_getRealPath($catalog), $this->_getRealPath($root)) === 0 && mb_strlen($root) <= mb_strlen($catalog)) {
				return true;
			}
		}

		return false;
	}

	function _prepare ($tmp) {
		$tmp = str_replace('\\\\', '||', $tmp);
		$tmp = str_replace('//', '||', $tmp);
		$tmp = str_replace('\\', '/', $tmp);
		$tmp .= '/';
		$tmp = preg_replace('|/{2,}|', '/', $tmp);
		$tmp = str_replace('||', '\\\\', $tmp);

		return $tmp;
	}

	function _isSmbShare($path) {
		return mb_strpos($path, '\\\\') === 0;
	}

	function _fullPath($catalog) {
		if($this->_isSmbShare($catalog)) {
			$catalog = str_replace('/', '\\', $catalog);

			return $catalog;
		}

		return BASEPATH. DIRECTORY_SEPARATOR . $catalog;
	}
}

class PXFileListingItem {
	function PXFileListingItem($filename, $parent) {
		$this->parent      =& $parent;

		$this->name        = $filename;
		$this->catalog     = $parent->_catalog;
		$this->destination = $parent->_destination;
		$this->path        = $this->_fullPath($parent->_catalog).$filename;

		$this->_isReadable();
		$this->_isCopyable();
		$this->_isWritable();
		$this->_isDeletable();
		$this->_isBinary();
		$this->_isDir();

		$this->_Type();
		$this->_Size();
		$this->_Comment('');
		$this->_Alias();
	}

	function _fullPath($catalog) {
		return $this->parent->_fullPath($catalog);
	}

	function _isCopyable() {
		if(!$this->readable || is_null($this->destination) || !is_writable($this->_fullPath($this->destination))) {
			$this->copyable = 0;
			return;
		}

		$this->copyable = (int)file_exists($this->_fullPath($this->destination).$this->name) ? 2 : 1;
	}

	function _isBinary() {
		$this->binary = (int)is_binary($this->path);
	}

	function _isDeletable() {
		$this->deletable = (int)is_writable($this->_fullPath($this->catalog));
	}

	function _isReadable() {
		$this->readable = (int)is_readable($this->path);
	}

	function _isWritable() {
		$this->writable = (int)is_writable($this->path);
	}

	function _Type() {
		preg_match('|[^/.\\\\]+$|', $this->name, $tmp);
		$this->type = isset($tmp[0]) ? mb_strtolower($tmp[0]) : '';
	}

	function _Size() {
		$this->size = filesize($this->path);
	}

	function _Comment($text) {
		$this->comment = $text;
	}

	function _isDir() {
		$this->isDir = 0;
	}

	function _Alias() {
		$this->alias = 0;

		if(!$this->readable) {
			return;
		}

		if($this->parent->_isSmbShare($this->catalog)) {
			$this->alias = str_replace('\\', '\\\\', $this->catalog.$this->name);
			return;
		}

		$htdocsPrefix = null;
		foreach(array('site', 'local', 'libpp') as $prefix) {
			if(mb_strpos($this->catalog, $prefix.'/htdocs') === 0) {
				$htdocsPrefix = $prefix.'/htdocs';
				break;
			}
		}

		if(is_null($htdocsPrefix)) {
			return;
		}

		$this->alias = str_replace($htdocsPrefix, '', $this->catalog.$this->name);
	}

	function isRoot() {
		return false;
	}
}

class PXFileListingItemCatalog extends PXFileListingItem {
	function _isBinary() {
		$this->binary = 0;
	}

	function _Type() {
		$this->type = 'dir';
	}

	function _Size() {
		$this->size = 'каталог';
	}

	function _isDir() {
		$this->isDir = 1;
	}

	function _Alias() {
		$this->alias = 0;
	}
}

class PXFileListingItemRoot extends PXFileListingItemCatalog {
	function _Comment($text) {
		$this->comment = $text;
	}

	function isRoot() {
		return true;
	}

	function _isDeletable() {
		$this->deletable = 0;
	}

	function _fullPath($catalog) {
		if($this->parent->_isSmbShare($this->name)) {
			return '';
		}

		return $this->parent->_fullPath($catalog);
	}
}
?>
