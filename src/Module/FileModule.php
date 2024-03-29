<?php

namespace PP\Module;

require_once PPLIBPATH . 'Filesys/listing.class.inc';
require_once PPLIBPATH . 'HTML/filelisting.class.inc';

/**
 * Class FileModule.
 *
 * @package PP\Module
 */
class FileModule extends AbstractModule
{
    public $settings;
    public $area;
    protected $protected;

    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        $this->area = $area;
        $this->settings = [];

        $this->_parseSettings($settings);
    }

    public function _parseSettings($settings)
    {
        if (!isset($settings['dir'])) {
            return;
        }

        $openDirs = $settings['dir'];
        $this->protected = $settings['protected'] ?? [];

        if (is_string($openDirs)) {
            $openDirs = [$openDirs];
        }

        foreach ($openDirs as $element) {
            $tmp = explode('=', (string) $element);
            $d = $tmp[0];
            $d = str_replace('\\', '/', $d);

            $this->settings[trim($d)] = trim($tmp[1]);
        }
    }

    public function error()
    {
        if (sizeof($this->settings)) {
            return false;
        }

        $this->layout->setOneColumn();
        $this->layout->assign('INNER.0.0', '<div class="error">Нет ни одного каталога <strong>доступного</strong> для просмотра!!!</div>');
        return true;
    }

    public function adminIndex()
    {
        if ($this->error()) {
            return;
        }

        $this->_setInnerLayout();

        $tabs = [
            [
                'cur' => 'ldir',
                'oth' => 'rdir',
                'cell' => '0',
                'side' => 'l',
            ],

            [
                'cur' => 'rdir',
                'oth' => 'ldir',
                'cell' => '1',
                'side' => 'r',
            ],
        ];

        $lDir = $this->request->getVar('ldir');
        $rDir = $this->request->getVar('rdir');

        foreach ($tabs as $tab) {
            $curDir = $tab['side'] == 'l' ? $lDir : $rDir;
            $othDir = $tab['side'] != 'l' ? $lDir : $rDir;

            $href = '?area=' . $this->area . '&' . $tab['oth'] . '=' . rawurlencode((string) $othDir);

            $listingClass = $curDir ? 'PXFileListing' : 'PXFileListingRoots';
            $listing = new $listingClass($this->settings);

            $listing->setDestination($othDir);
            $listing->getList($curDir);
            $listing->setDecorator(new \PXHTMLFileListing($href, $tab['side'], function ($alias, $f) {
                $url = $alias;

                $catalog = rtrim((string) $f->catalog, '/');
                foreach ($this->protected as $protected) {
                    if (strncmp($catalog, $protected, mb_strlen($protected)) === 0) {
                        $mimeType = mime_content_type($f->path);
                        $url = 'action.phtml?area=file&url=' . $alias . '&type=' . $mimeType . '&action=protected';
                        break;
                    }
                }

                return $url;
            }));

            $this->layout->assign('INNER.' . $tab['cell'] . '.0', $listing->html());

            if ($listing->writable()) {
                $this->layout->assign('INNER.' . $tab['cell'] . '.1', $this->_uploadForm($this->area, $lDir, $rDir, $tab['side'], false, $this->area));
            }

            unset($listing);
        }
    }

    public function _uploadForm($area, $ldir, $rdir, $side = 'l', $name = false, $outside = null)
    {
        $_ =
            <<<HTML
<div class="upload-buttons">
	<input type="button" value="Новый каталог" onclick="javascript:return CreateDir('%DIR%', '%HREF%', '%SIDE%');">
	<input type="button" value="Новый файл" onclick="ShowUploadForm('%SIDE%UploadForm');">
</div>

<form action="action.phtml" method="POST" enctype="multipart/form-data" id="%SIDE%UploadForm" class="uploader hide">
	<input type="hidden" name="action"  value="upload"   >
	<input type="hidden" name="area"    value="%AREA%"   >
	<input type="hidden" name="outside" value="%OUTSIDE%">
	<input type="hidden" name="mdir"    value="%MDIR%"   >
	<input type="hidden" name="ldir"    value="%LDIR%"   >
	<input type="hidden" name="rdir"    value="%RDIR%"   >
	<input type="hidden" name="side"    value="%SIDE%"   >
	<input type="hidden" name="name"    value="%NAME%"   >

	Выберите файл:<input type="file" name="uploadfile" size="10"/> или введите url:<input type="text" name="uploadfileurl">

	<input type="submit" value="Закачать">
</form>
HTML;

        $replaces = [
            'SIDE' => $side,
            'DIR' => ($side == 'l' ? urlencode((string) $ldir) : urlencode((string) $rdir)),
            'HREF' => '?area=' . $this->area . '&ldir=' . urlencode((string) $ldir) . '&rdir=' . urlencode((string) $rdir) . ($name ? '&name=' . $name : ''),
            'AREA' => $this->area,
            'OUTSIDE' => $outside,
            'MDIR' => ($side == 'l' ? $ldir : $rdir),
            'LDIR' => $ldir,
            'RDIR' => $rdir,
            'NAME' => $name,
        ];

        foreach ($replaces as $label => $value) {
            $_ = str_replace('%' . $label . '%', $value, $_);
        }

        return $_;
    }

    public function _setInnerLayout()
    { // FIXME!!!
        $_html =
            <<<HTML
<table class="filemanager">
	<thead>
		<tr>
			<td>
				<div {INNER.0.1.CONTEXT}>
					{INNER.0.1}
				</div>
			</td>

			<td>
				<div {INNER.1.1.CONTEXT}>
					{INNER.1.1}
				</div>
			</td>
		</tr>
	</thead>

	<tbody>
		<tr>
			<td>
				<div {INNER.0.0.CONTEXT} class="content">
					{INNER.0.0}
				</div>
			</td>

			<td>
				<div {INNER.1.0.CONTEXT} class="content">
					{INNER.1.0}
				</div>
			</td>
		</tr>
	</tbody>
</table>
HTML;
        $this->layout->assign('OUTER.MAINAREA', $_html);
    }

    public function adminPopup()
    {
        $this->layout->setGetVarToSave('id', $this->request->GetId());
        $this->layout->setGetVarToSave('format', $this->request->GetFormat());
        $this->layout->setGetVarToSave('action', $this->request->GetAction());

        $this->_initRequestVars();

        $this->layout->setOuterForm('action.phtml', 'POST', 'multipart/form-data');
        $html = match ($this->request->getVar('action')) {
            'edit' => '<div class="edit-file-form">' . $this->editFileForm() . '</div>',
            default => '<div class="error">Действие не определено</div>',
        };

        return $html;
    }

    public function editFileForm()
    {
        $filePath = $this->catalog . $this->mFile;

        if (is_file($filePath) && is_writable($filePath) && !is_binary($filePath)) {
            $object = [];
            $form = new \PXAdminForm($object, null);

            $this->layout->assign('OUTER.TITLE', 'Редактирование файла &laquo;' . $this->mFile . '&raquo;');
            $form->leftControls();
            $form->rightControls();

            return $form->editTextFileForm($this->mFile, $this->catalog);
        } else {
            return 'Редактирование файлов подобного формата еще не реализовано';
        }
    }

    public function _initRequestVars()
    {
        $rSide = $this->request->getVar('side');
        $lDir = $this->request->getVar('ldir');
        $rDir = $this->request->getVar('rdir');

        $outSide = $this->request->getVar('outside');

        $this->catObject = new \PXFileListing($this->settings);

        $this->redir = (!empty($outSide) && $outSide !== $this->area ? 'popup.phtml?action=' . $outSide . '&name=' . $this->request->getVar('name') . '&' : './?');
        $this->redir .= 'area=' . $this->area . '&ldir=' . $lDir . '&rdir=' . $rDir;

        $catalog = ($rSide != 'r' ? $lDir : $rDir);
        $this->catalog = $catalog && $this->catObject->isValid($catalog) ? $this->fullPath($catalog) . '/' : null;

        $destination = ($rSide == 'r' ? $lDir : $rDir);
        $this->destination = $destination && $this->catObject->isValid($destination) ? $this->fullPath($destination) . '/' : null;

        $this->mFile = _stripBadFileChars($this->request->getVar('mfile'));
        $this->nFile = $this->_escapeFileName($this->request->getVar('nfile'));
    }

    public function adminAction()
    {
        $this->_initRequestVars();

        match ($this->request->getVar('action')) {
            'createdir' => $this->withArgs([$this->catalog], '_aCreateDir'),
            'copy' => $this->withArgs([$this->catalog, $this->destination, $this->mFile], '_aCopy'),
            'move' => $this->withArgs([$this->catalog, $this->destination, $this->mFile], '_aMove'),
            'rename' => $this->withArgs([$this->catalog, $this->mFile, $this->nFile], '_aRename'),
            'delete' => $this->withArgs([$this->catalog, $this->mFile], '_aDelete'),
            'edit' => $this->withArgs([$this->mFile], '_aEdit'),
            'unzip' => $this->withArgs([$this->catalog, $this->mFile], '_aUnZip'),
            'upload' => $this->withArgs([$this->catalog], '_aUpload'),
            'protected' => $this->_aProtectedAccess(),
            default => $this->redir,
        };

        return $this->redir;
    }

    protected function fullPath($path)
    {
        return BASEPATH . DIRECTORY_SEPARATOR . $path;
    }

    protected function withArgs($fields, $action)
    {
        (is_countable($fields) ? count($fields) : 0) == count((array) array_filter($fields)) && $this->{$action}();
    }

    public function _escapeFileName($name)
    {
        return $name = _TranslitFilename(_stripBadFileChars($name));
    }

    # Actions

    /**
     * Для того, чтобы работали защищенные ссылки на файлы, нужно добавить соответствующие internal location в nginx
     */
    public function _aProtectedAccess()
    {
        $url = $this->request->getVar('url');
        $type = $this->request->getVar('type');

        $this->response->addHeader('X-Accel-Redirect', $url)
            ->setContentType($type)
            ->downloadFile(basename((string) $url))
            ->send();
    }

    public function _aEdit()
    {
        $this->redir = null;

        if (!$this->catObject->isValid($mdir = $this->request->getVar('mdir'))) {
            return;
        }

        $mdir = $this->fullPath($mdir);
        $source = $this->request->getVar('filesource');
        $filePath = $mdir . $this->mFile;

        if (is_file($filePath) && is_writable($filePath) && !is_binary($filePath)) {
            $fp = fopen($filePath, 'w');
            fwrite($fp, (string) $source);
            fclose($fp);
        }
    }

    public function _aDelete()
    {
        $filePath = $this->catalog . $this->mFile;

        if (!is_writable($this->catalog)) {
            return;
        }

        switch (true) {
            case is_file($filePath):
                unlink($filePath);
                break;

            case is_dir($filePath):
                $d = new \NLDir($filePath);
                $d->Delete();
                break;
        }
    }

    public function _aCopy()
    {
        $entry = $this->mFile;

        if (file_exists($this->catalog . $entry) && is_writable($this->destination)) {
            copyr($this->catalog . $entry, $this->destination . $entry);
        }
    }

    public function _aMove()
    {
        $entry = $this->mFile;

        if (file_exists($this->catalog . $entry) && is_writable($this->destination) && is_writable($this->catalog)) {
            copyr($this->catalog . $entry, $this->destination . $entry);

            $this->_aDelete();
        }
    }

    public function _aCreateDir()
    {
        $ndir = $this->_escapeFileName($this->request->getVar('ndir'));

        if (is_dir($this->catalog) && is_writable($this->catalog) && !is_dir($this->catalog . $ndir)) {
            MakeDirIfNotExists($this->catalog . $ndir);
        }
    }

    public function _aRename()
    {
        $oldFile = $this->mFile;
        $newFile = $this->nFile;

        if ((is_file($this->catalog . $oldFile) || is_dir($this->catalog . $oldFile)) && is_writable($this->catalog) && (!is_file($this->catalog . $newFile) && !is_dir($this->catalog . $newFile))) {
            rename($this->catalog . $oldFile, $this->catalog . $newFile);
        }
    }

    public function _aUnZip()
    {
        if (is_file($this->catalog . $this->mFile) && is_writable($this->catalog)) {
            system(sprintf("unzip -oq %s -d %s", escapeshellarg($this->catalog . $this->mFile), escapeshellarg((string) $this->catalog)));
        }
    }

    public function _aUpload()
    {
        if ($this->request->GetUploadFile('uploadfile')) {
            $this->uploadFileFromUser($this->request->GetUploadFile('uploadfile'));
        } elseif ($this->request->GetVar('uploadfileurl')) {
            $this->uploadFileFromWeb($this->request->GetVar('uploadfileurl'));
        }
    }

    public function uploadFileFromUser($file)
    {
        if ($file && $file != 'none' && $file['name'] && is_writable($this->catalog)) {
            $filename = _TranslitFilename(_stripBadFileChars(stripslashes((string) $file['name'])));
            if (!@copy($file['tmp_name'], $this->catalog . $filename)) {
                FatalError('Не удалось скопировать файл ' . $file['tmp_name'] . ' в каталог ' . $this->catalog);
            }

            if (!@chmod($this->catalog . $filename, 0664)) {
                FatalError('Не удалось изменить права доступа файлу ' . $this->catalog . $filename);
            }
        }
    }

    public function uploadFileFromWeb($url)
    {
        if ($url && mb_strlen((string) $url) && preg_match('#^https?://#', (string) $url) && is_writable($this->catalog)) {
            if ($fp = @fopen($url, 'r')) {
                $fileSource = '';

                while ($r = fread($fp, 2048)) {
                    $fileSource .= $r;
                }

                fclose($fp);

                if (mb_strlen($fileSource)) {
                    $filename = tempnam($this->catalog, 'file');

                    if ($fp1 = @fopen($filename, 'w')) {
                        @fwrite($fp1, $fileSource);
                        @fclose($fp1);
                    } else {
                        FatalError('Не удалось открыть/создать файл ' . $filename);
                    }

                    if (!@chmod($filename, 0664)) {
                        FatalError('Не удалось изменить права доступа файлу ' . $filename);
                    }

                    if (preg_match_all("#/([\w\-]+\.\w{1,5})(\?.*)?$#", (string) $url, $matches, PREG_SET_ORDER) && isset($matches[0][0])) {
                        $matches[0][0] = _TranslitFilename($matches[0][0]);
                        rename($filename, $this->catalog . $matches[0][0]);
                        $filename = $this->catalog . $matches[0][0];
                    }

                    $map = [
                        '1' => 'gif',
                        '2' => 'jpg',
                        '3' => 'png',
                        '6' => 'bmp',
                    ];

                    $fileAttr = getimagesize($filename);

                    if (isset($map[$fileAttr[2]])) {
                        $subname = str_replace($this->catalog, '', $filename);
                        if (mb_strpos($subname, '.')) {
                            $subname = mb_substr($subname, 0, mb_strpos($subname, '.'));
                            rename($filename, $this->catalog . $subname . "." . $map[$fileAttr[2]]);
                            $filename = $this->catalog . $subname . "." . $map[$fileAttr[2]];
                        } else {
                            rename($filename, $filename . "." . $map[$fileAttr[2]]);
                            $filename = $filename . "." . $map[$fileAttr[2]];
                        }
                    }
                }
            }
        }
    }

}
