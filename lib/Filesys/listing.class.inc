<?php

include_once PPLIBPATH . 'Filesys/dir.class.inc';

class PXFileListingRoots extends PXFileListing
{
    public $_comments;

    public function getList($catalog)
    {
	}

    public function setRoots($catalogs)
    {
        $this->_comments = [];

        foreach ($catalogs as $catalog => $comment) {
            $catalog = $this->_prepare($catalog);

            $_root = new PXFileListingItemRoot($catalog, $this);
            $_root->_Comment($comment);

            $this->_files[$catalog] = $_root;
        }
    }

    public function html()
    {
        $this->_decorator->setCatalog($this->_catalog);

        $_ = $this->_decorator->htmlHeader();
        $_ .= $this->_decorator->htmlList($this->_files);
        $_ .= $this->_decorator->htmlFooter();

        return $_;
    }

    public function writable()
    {
        return false;
    }
}

class PXFileListing
{
    public $_roots;
    public $_files;
    public $_catalog;
    public $_destination;
    public $_decorator;

    public function __construct($catalogs)
    {
        $this->_roots = [];
        $this->_files = null;

        $this->setRoots($catalogs);
    }

    public function setDecorator($htmlDecorator)
    {
        $this->_decorator = $htmlDecorator;
    }

    public function writable()
    {
        return is_writable($this->_fullPath($this->_catalog));
    }

    public function html()
    {
        $up = preg_replace('|[^/]+/$|' . REGEX_MOD, '', (string) $this->_catalog);

        $canBrowse = $this->_isValid($this->_catalog);
        $canUp = $this->_isValid($up);

        $this->_decorator->setCatalog($this->_catalog);

        $_ = $this->_decorator->htmlHeader();
        $_ .= $this->_decorator->htmlUp($canUp, $up);

        if (is_array($this->_files)) {
            $_ .= $this->_decorator->htmlList($this->_files);
            $_ .= $this->_decorator->htmlFooter();

        } else {
            $_ .= $this->_decorator->htmlFooter();
            $_ .= $this->_decorator->error();
        }

        return $_;
    }

    public function setDestination($destination)
    {
        $destination = $this->_prepare($destination);

        if (!$this->_isValid($destination)) {
            $this->_destination = null;
            return false;
        }

        $this->_destination = $destination;
    }

    public function setRoots($catalogs)
    {
        foreach (array_keys($catalogs) as $catalog) {
            $catalog = $this->_prepare($catalog);

            if (is_dir($this->_fullPath($catalog))) {
                $this->_roots[] = $catalog;
            }
        }
    }

    public function getList($catalog)
    {
        $catalog = $this->_prepare($catalog);
        $this->_catalog = $catalog;
        if (!$this->_isValid($catalog)) {
            return null;
        }
        return $this->_ls($catalog);
    }

    # public interface
    public function isValid($catalog)
    {
        return $this->_isValid($catalog);
    }

    public function _ls($catalog)
    {
        $this->_files = [];

        $catalog = $this->_prepare($this->_fullPath($catalog));
        $_dir = new NLDir($catalog);

        foreach ($_dir->getDirs(true) as $path) {
            $this->_files[$path] = new PXFileListingItemCatalog($path, $this);
        }

        foreach ($_dir->getFileNames(true) as $path) {
            $this->_files[$path] = new PXFileListingItem($path, $this);
        }

        return $this->_files;
    }

    public function _getRealPath($catalog)
    {
        return realpath($this->_fullPath($catalog));
    }

    public function _isValid($catalog)
    {
        if (!is_dir($this->_fullPath($catalog))) {
            return false;
        }

        foreach ($this->_roots as $root) {
            if (mb_strpos((string) $this->_getRealPath($catalog), (string) $this->_getRealPath($root)) === 0 && mb_strlen((string) $root) <= mb_strlen((string) $catalog)) {
                return true;
            }
        }

        return false;
    }

    public function _prepare($tmp)
    {
        $tmp = str_replace('\\\\', '||', (string) $tmp);
        $tmp = str_replace('//', '||', $tmp);
        $tmp = str_replace('\\', '/', $tmp);
        $tmp .= '/';
        $tmp = preg_replace('|/{2,}|' . REGEX_MOD, '/', (string) $tmp);
        $tmp = str_replace('||', '\\\\', $tmp);

        return $tmp;
    }

    public function _isSmbShare($path)
    {
        return mb_strpos((string) $path, '\\\\') === 0;
    }

    public function _fullPath($catalog)
    {
        if ($this->_isSmbShare($catalog)) {
            $catalog = str_replace('/', '\\', (string) $catalog);

            return $catalog;
        }

        return BASEPATH . DIRECTORY_SEPARATOR . $catalog;
    }
}

class PXFileListingItem
{
    public function __construct($filename, $parent)
    {
        $this->parent =& $parent;

        $this->name = $filename;
        $this->catalog = $parent->_catalog;
        $this->destination = $parent->_destination;
        $this->path = $this->_fullPath($parent->_catalog) . $filename;

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

    public function _fullPath($catalog)
    {
        return $this->parent->_fullPath($catalog);
    }

    public function _isCopyable()
    {
        if (!$this->readable || is_null($this->destination) || !is_writable($this->_fullPath($this->destination))) {
            $this->copyable = 0;
            return;
        }

        $this->copyable = (int)file_exists($this->_fullPath($this->destination) . $this->name) ? 2 : 1;
    }

    public function _isBinary()
    {
        $this->binary = (int)is_binary($this->path);
    }

    public function _isDeletable()
    {
        $this->deletable = (int)is_writable($this->_fullPath($this->catalog));
    }

    public function _isReadable()
    {
        $this->readable = (int)is_readable($this->path);
    }

    public function _isWritable()
    {
        $this->writable = (int)is_writable($this->path);
    }

    public function _Type()
    {
        preg_match('|[^/.\\\\]+$|' . REGEX_MOD, (string) $this->name, $tmp);
        $this->type = isset($tmp[0]) ? mb_strtolower($tmp[0]) : '';
    }

    public function _Size()
    {
        $this->size = filesize($this->path);
    }

    public function _Comment($text)
    {
        $this->comment = $text;
    }

    public function _isDir()
    {
        $this->isDir = 0;
    }

    public function _Alias()
    {
        $this->alias = 0;

        if (!$this->readable) {
            return;
        }

        if ($this->parent->_isSmbShare($this->catalog)) {
            $this->alias = str_replace('\\', '\\\\', $this->catalog . $this->name);
            return;
        }

        $htdocsPrefix = null;
        foreach (['site', 'local', 'libpp'] as $prefix) {
            if (mb_strpos((string) $this->catalog, $prefix . '/htdocs') === 0) {
                $htdocsPrefix = $prefix . '/htdocs';
                break;
            }
        }

        if (is_null($htdocsPrefix)) {
            return;
        }

        $this->alias = str_replace($htdocsPrefix, '', $this->catalog . rawurlencode((string) $this->name));
    }

    public function isRoot()
    {
        return false;
    }
}

class PXFileListingItemCatalog extends PXFileListingItem
{
    public function _isBinary()
    {
        $this->binary = 0;
    }

    public function _Type()
    {
        $this->type = 'dir';
    }

    public function _Size()
    {
        $this->size = 'каталог';
    }

    public function _isDir()
    {
        $this->isDir = 1;
    }

    public function _Alias()
    {
        $this->alias = 0;
    }
}

class PXFileListingItemRoot extends PXFileListingItemCatalog
{
    public function _Comment($text)
    {
        $this->comment = $text;
    }

    public function isRoot()
    {
        return true;
    }

    public function _isDeletable()
    {
        $this->deletable = 0;
    }

    public function _fullPath($catalog)
    {
        if ($this->parent->_isSmbShare($this->name)) {
            return '';
        }

        return $this->parent->_fullPath($catalog);
    }
}
