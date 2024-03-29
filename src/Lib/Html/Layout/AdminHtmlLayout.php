<?php

namespace PP\Lib\Html\Layout;

/**
 * Class AdminHtmlLayout
 * @package PP\Lib\Html\Layout
 */
class AdminHtmlLayout extends LayoutAbstract
{
    public $_scripts = [];


    //TODO: there must be right places in <HEAD> for this
    public $_scriptsTemplates = [
        ':css' => [
            ':area' => 'OUTER.FORMBEGIN',
            ':proto' => '<link rel="stylesheet" href="%s" type="text/css" />',
        ],
        ':js' => [
            ':area' => 'OUTER.FORMEND',
            ':proto' => '<script src="%s" type="text/javascript"></script>',
        ],
        ':inline_js' => [
            ':area' => 'OUTER.FORMEND',
            ':proto' => "<script type=\"text/javascript\">\n%s\n</script>",
        ],
        ':inline_css' => [
            ':area' => 'OUTER.FORMBEGIN',
            ':proto' => "<style type=\"text/css\">\n%s\n</style>",
        ],
    ];

    public function __construct($outerLayout, public $types)
    {
        parent::__construct();

        $this->assignTitle("Proxima Portal");
        $this->setOuterLogo('i/admin.gif', 'Proxima Portal', 126, 36);
        $this->setOuterLayout($outerLayout);

        $this->assign('BODY.CLASS', quot(\PXRegistry::getRequest()->getArea()));
    }

    protected function assignVersion()
    {
        $user = \PXRegistry::getUser();
        $version = ($user->isAuthed())
            ? PP_VERSION
            : '';

        $this->assign('OUTER.VERSION', $version);
    }

    /**
     * Retrieves all flash messages from session, makes html and
     * assigns it to layout.
     */
    public function assignFlashes()
    {
        $htmlFlashes = '';
        $session = \PXRegistry::getSession();

        foreach ($session->getFlashBag()->all() as $type => $messages) {
            foreach ($messages as $message) {
                $htmlFlashes .= sprintf(
                    '<div class="flash flash_%s"><a class="flash_close" href="#">×</a>%s</div>',
                    $type,
                    $message
                );
            }
        }

        $this->assign('FLASHES', $htmlFlashes);
    }

    /**
     * @param null|string $title
     * @return $this
     */
    public function assignTitle($title = null)
    {
        $title = ((is_null($title) || !mb_strlen(trim($title)))
            ? ''
            : mb_strtr($title, ['<' => '&lt;', '>' => '&gt;']));

        $this->assign("OUTER.TITLE", $title);

        return $this;
    }

    public function assignError($label, $errorText)
    {
        $this->assign($label, '<p class="error">' . $errorText . '</p>');
    }

    public function assignContentControls($label, $selectedSid, $allowedFormats): void
    {
        throw new \Exception("please use AdminHtmlLayout::AssignControls");
    }

    public function assignControls($label, $selectedSid, $allowedFormats)
    {
        $this->clear($label);
        $this->appendControls($label, $selectedSid, $allowedFormats);
    }

    public function appendControls($label, $selectedSid, $allowedFormats)
    {
        foreach ($allowedFormats as $format) {
            $button = new \PXControlButton($this->types[$format]->title);
            $button->setClickCode('AddContent(\'' . $format . '\', ' . (int)$selectedSid . ')');
            $button->setClass('add');

            $button->addToParent($label);
        }
    }

    public function _makeContextMenu($selectedSid, $allowedFormats)
    {
        $html = '';

        if (sizeof($allowedFormats)) {
            $html .= ' onContextMenu="Context(event, \'add\', \'' . quot(quot($selectedSid), false) . '\'';

            foreach ($this->types as $k => $v) {
                if (in_array($k, $allowedFormats)) {
                    $html .= ' , \'' . $k . '\', \'' . $v->title . '\'';
                }
            }

            $html .= '); return false;" ';
        }

        return $html;
    }

    public function assignContextMenu($label, $selectedSid, $allowedFormats)
    {
        $this->assign($label, $this->_makeContextMenu($selectedSid, $allowedFormats));
    }

    public function appendContextMenu($label, $selectedSid, $allowedFormats)
    {
        // store formats and labels for each parent
        // some times there are same parents so we can share context menus
        static $stored = [];
        if (!isset($stored[$selectedSid])) {
            $stored[$selectedSid] = [
                'labels' => [],
                'formats' => [],
            ];
        }

        // append to label
        $collection = &$stored[$selectedSid];
        $collection['labels'][] = $label;
        $collection['formats'] = array_merge($collection['formats'], $allowedFormats);

        // draw it for each label (each space)
        foreach ($collection['labels'] as $_label) {
            $this->assign($_label, $this->_makeContextMenu($selectedSid, $collection['formats']));
        }
    }

    public function appendTable($label, $objectFormat, $table, $selected = null, $varToTitle = null, $page = 1, $objectsOnPage = 0, $count = 0, $withLinks = true, $parentPathname = null)
    {
        $htmltable = new \PXAdminTable($table, $this->types[$objectFormat], $this->getData);
        $htmltable->setCaption($this->types[$objectFormat]->title . '(' . $count . ')');

        $pager = new \PxAdminPager($page, $objectsOnPage, $count, $this->types[$objectFormat], $this->getData);
        $htmltable->setPosition($pager->getPosition());

        $this->append($label, $htmltable->html() . $pager->html());
    }

    public function appendUserTable($label, $objectFormat, $title, $table, &$userClassName, $userFuncName, $page = 1, $objectsOnPage = 0, $count = 0)
    {
        $this->Append($label, '<H2>' . $title . ' (' . $count . ')</H2>');
        $html = null;
        $page = $page ?: 1; // ?

        if (!(is_countable($table) ? count($table) : 0)) {
            $html = '';
            $this->Append($label, $html);
            return;
        }

        $html .= call_user_func([$userClassName, $userFuncName], 'header');
        foreach ($table as $rowPos => $row) {
            $html .= call_user_func([$userClassName, $userFuncName], 'row', $row);
        }
        $html .= call_user_func([$userClassName, $userFuncName], 'footer');

        if ($count > $objectsOnPage && $objectsOnPage > 0) {
            $html .= '<DIV style="padding: 2px;">';
            $html .= '<STRONG style="color: #385A94;">Страницы:</STRONG> ';

            $allPages = ceil($count / $objectsOnPage) + 1;
            $start = (ceil($page / 10) - 1) * 10 + 1;
            $max = $start + 10;

            if ($max > $allPages) {
                $max = $allPages;
            }

            if ($page > 10) {
                $prev = (ceil($start / 10) - 1) * 10 - 9;
            }

            $last = $allPages - $start - 10;

            if ($last > 0) {
                $next = (ceil($start / 10) - 1) * 10 + 11;
            }

            if (isset($prev)) {
                $html .= '<A href="' . static::_BuildHref($objectFormat . '_page', $prev) . '">';
                $html .= '<IMG src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница ' . $prev . '">';
                $html .= '</A>';
            }

            for ($i = $start; $i < $max; $i++) {
                $html .= '<A  style="padding: 2px 4px 2px 4px; text-decoration: none;';

                if ($i == $page) {
                    $html .= 'background-color: #385A94; color: #FFFFFF; font-weight: bold;';
                }

                $html .= '" href="' . static::_BuildHref($objectFormat . '_page', $i) . '" title="Страница ' . $i . '">' . $i . '</A> ';
            }

            if (isset($next) && $next > 0) {
                $html .= '<A href="' . static::_BuildHref($objectFormat . '_page', $next) . '">';
                $html .= '<IMG src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница ' . $next . '">';
                $html .= '</A>';
            }

            $html .= '</DIV>';
        }
        $this->Append($label, $html);
    }

    public function assignKeyValueList($label, $list, $selected, $varName = 'sid')
    {
        parent::AssignKeyValueList($label, $list, $varName, $selected);
    }

    public function assignJS($pathToScript)
    {
        $this->_renderScript($pathToScript, ':js', true);
    }

    public function assignInlineJS($scriptBody, $uniq = true)
    {
        $this->_renderScript($scriptBody, ':inline_js', $uniq);
    }

    public function assignCSS($pathToScript)
    {
        $this->_renderScript($pathToScript, ':css', true);
    }

    public function assignInlineCSS($scriptBody)
    {
        $this->_renderScript($scriptBody, ':inline_css', true);
    }

    public function _renderScript($body, $template, $singleton)
    {
        $hash = md5((string) $body);
        if (!mb_strlen((string) $body) || ($singleton && isset($this->_scripts[$hash]))) {
            return false;
        }

        $this->append($this->_scriptsTemplates[$template][':area'], sprintf($this->_scriptsTemplates[$template][':proto'], $body));

        return $singleton ? ($this->_scripts[$hash] = true) : true;
    }

    public function template($filename, $label = null)
    {
        $this->assignVersion();

        return parent::template($filename, $label);
    }

    public function notSetAllowedChilds($label, $format, $id)
    {
        $format = \PXRegistry::getTypes($format);

        $this->assign(
            $label,
            <<<HTML
			<h2>Для добавления в раздел информации необходимо указать разрешенные форматы</h2>

			<ul>
				<li>
					<a href="javascript:EditContent('{$format->id}', '{$id}', 'children')">Разрешенные форматы раздела</a>
				</li>
			</ul>
HTML
        );
    }
}
