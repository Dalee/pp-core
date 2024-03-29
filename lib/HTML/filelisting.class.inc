<?php
/*
FIXME: UNUSED?
class PXHTMLFileListingLink extends PXHTMLFileListing {

	function __construct($href, $side, $name) {
		parent::__construct($href, $side);
		$this->name = htmlspecialchars($name, ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET);
	}

	public function htmlHeader() {
		$_html =   // FIXME !!!
<<<HTML
<table>
	<colgroup>
		<col class="name" />
		<col class="size" />
		<col class="rules"/>
		<col />
	</colgroup>

	<thead>
		<tr>
			<th>имя</th>
			<th>размер</th>
			<th>права</th>
			<th></th>
		</tr>
	</thead>
HTML;
		return $this->_breadCrumbs().$_html;
	}

	public function htmlList($files) {
		$_ = '';

		foreach($files as $f) {
			$_ .= "\n".'<tr>';

			$_ .= $this->_listTitle($f);
			$_ .= $this->_listSize($f);
			$_ .= $this->_listPermissions($f);
			$_ .= $this->_listLink($f);

			$_ .= '</tr>';
		}

		return $_;
	}

	function _listLink($f) {
		$_ = '<td></td>';

		if($f->isDir || !$f->alias || !mb_strlen($f->alias)) {
			return $_;
		}

		return '<td><button class="link" onclick="LinkFile(\''.$this->name.'\', \''.dirname($f->alias).'/\', \''.$f->name.'\', '.$f->size.');">Привязать</button></td>';
	}

	function _listContext($f) {
		return '<span>'.$f->name.'</span>';
	}
}
*/

class PXHTMLFileListing
{

    protected $urlBuilder;

    public function __construct($href, $side, ?callable $urlBuilder = null)
    {
        $this->base = $href . '&' . $side . 'dir=';
        $this->side = $side;
        $this->files = [];
        $this->href = $href;
        $this->urlBuilder = $urlBuilder ?: fn($alias) => $alias;
    }

    public function htmlHeader()
    {
        $_html =   // FIXME !!!
            <<<HTML
<table>
	<colgroup>
		<col class="name" />
		<col class="size" />
		<col class="rules"/>
	</colgroup>

	<thead>
		<tr>
			<th>имя</th>
			<th>размер</th>
			<th>права</th>
		</tr>
	</thead>
HTML;
        return $this->_breadCrumbs() . $_html;
    }

    public function htmlUp($canUp, $up)
    {
        $_html = // FIX ME !!!
            <<<HTML
<tr>
	<td class="file dir-up">
		<a href="%HREF%" title="Наверх">
			<span>%TEXT%</span>
		</a>
	</td>

	<td class="size-dir">каталог</td>
	<td>%PERM%</td>
</tr>
HTML;
        $_replaces = [
            'HREF' => $this->base . ($canUp ? $up : ''),
            'TEXT' => ($canUp ? '..' : '&#8595;'),
            'PERM' => $this->_filePermissions($this->catalog),
        ];

        foreach ($_replaces as $label => $value) {
            $_html = str_replace('%' . $label . '%', $value, $_html);
        }

        return $_html;
    }

    public function _breadCrumbs()
    {
        $pathA = array_map($this->escapeHTML(...), explode('/', (string) $this->catalog));

        array_unshift($pathA, '&darr;');

        $res = [];

        foreach ($pathA as $k => $d) {
            if (trim((string) $d) == '') {
                continue;
            }

            $href = implode('/', array_slice($pathA, 1, $k));

            $res[] = '<a href="' . $this->base . rawurlencode($href) . '">' . $d . '</a>';
        }

        $_ = implode('/', $res);

        return '<div class="breadcrumds">' . $_ . '</div>';
    }

    public function htmlList($files)
    {
        $_ = '';

        foreach ($files as $f) {
            $_ .= "\n" . '<tr>';

            $_ .= $this->_listTitle($f);
            $_ .= $this->_listSize($f);
            $_ .= $this->_listPermissions($f);

            $_ .= '</tr>';
        }

        return $_;
    }

    public function _listTitle($f)
    {
        $td_class = $f->isDir ? ' ' . $f->type : '';
        $_ = '<td class="file' . $td_class . '">';

        if ($f->isDir) {
            if ($f->readable) {
                $_ .= '<a href="' . $this->base . rawurlencode($this->catalog . $f->name) . '" title="' . $this->escapeHTML($f->name) . '">';
                $_ .= $this->_listContext($f);
                $_ .= '</a>';
            } else {
                $_ .= '<del>' . $this->escapeHTML($f->name) . '</del>';
            }

            if ($f->comment) {
                $_ .= ' <em>' . $f->comment . '</em>';
            }

        } else {
            $_ .= $this->_listContext($f);
        }

        $_ .= '</td>';

        return $_;
    }

    private function escapeJS($str)
    {
        return "'" . $this->escapeHTML(addslashes((string) $str)) . "'";
    }

    private function escapeHTML($str)
    {
        return htmlspecialchars((string) $str, ENT_COMPAT | ENT_HTML401 | ENT_SUBSTITUTE | ENT_DISALLOWED, DEFAULT_CHARSET);
    }

    public function _listContext($f)
    {

        $name = $this->escapeHTML($f->name);
        $alias = $this->escapeHTML($f->alias);

        $url = ($this->urlBuilder)($alias, $f);

        $tmp = [
            "Context(event",
            "'file'",
            $this->escapeJS($f->name),                                     // title
            $f->isDir,                                                     // isDir
            $this->escapeJS(str_replace('\\', '/', (string) $url)),            // isRead / alias
            $f->writable,                                                  // isWrite
            $f->deletable,                                                 // isDelete
            $f->binary,                                                    // isBinary
            "'" . rawurlencode(str_replace('\\', '/', (string) $this->catalog)) . "'",  // dir
            $this->escapeJS(str_replace('\\', '/', (string) $this->href)),          // href
            "'" . $this->side . "'",                                           // side
            $f->copyable,                                                  // isCopy
            "'" . rawurlencode((string) $f->name) . "'",                                // fileName
            intval(!mb_check_encoding($f->name, DEFAULT_CHARSET)) . "); return false;" // isBrokenFilename
        ];

        $context = ' onContextMenu="' . implode(', ', $tmp) . '"';

        if ($f->isDir) {
            $click = $name;
        } elseif ($f->alias) {
            $click = '<a style="margin-left:-21px" href="' . $url . '"><img src="/admin/i/files/' . $f->type . '.gif" /></a>';
            $click .= '<a class="memory" href="' . $url . '">' . $name . '</a>';
        } else {
            $click = $name;
        }

        return "<span" . $context . ">" . $click . "</span>";
    }

    public function _listSize($f)
    {
        if ($f->isDir) {
            return '<td class="size-dir">каталог</td>';

        } else {
            return '<td>' . $f->size . '</td>';
        }
    }

    public function setCatalog($catalog)
    {
        $this->catalog = $catalog;
    }

    public function _filePermissions($filepath)
    {
        if (!function_exists('posix_getpwuid') || !file_exists($filepath)) {
            return '---';
        }

        $owner = posix_getpwuid(fileowner($filepath));
        $oct = mb_substr(decoct(fileperms($filepath)), -3);
        $str = '';

        for ($i = 0; $i < 3; $i++) {
            switch (mb_substr($oct, $i, 1)) {
                case '7':
                    $str = 'rwx';
                    break;
                case '6':
                    $str = 'rw-';
                    break;
                case '5':
                    $str = 'r-x';
                    break;
                case '4':
                    $str = 'r--';
                    break;
                case '3':
                    $str = '-wx';
                    break;
                case '2':
                    $str = '-w-';
                    break;
                case '1':
                    $str = '--x';
                    break;
                case '0':
                    $str = '---';
                    break;
            }
        }

        return $owner['name'] . ' ' . $str;
    }

    public function _listPermissions($f)
    {
        return '<td>' . $this->_filePermissions($f->path) . '</td>';
    }

    public function htmlFooter()
    {
        return '</table>';
    }

    public function error()
    {
        return '<div class="error">Этот каталог <strong>не доступен</strong> для просмотра<br/> Вернитесь к <a href="' . $this->href . '">списку каталогов</a></div>';
    }
}
