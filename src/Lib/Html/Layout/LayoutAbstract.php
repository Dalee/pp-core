<?php

namespace PP\Lib\Html\Layout;

use PP\Lib\Http\Response;

define('TABLECOLOR1', '#E7E9ED');
define('TABLECOLOR2', '#C9CED6');
define('TABLECOLOR3', '#385A94');

require_once PPLIBPATH . 'Common/functions.array.inc';
require_once PPLIBPATH . 'Common/functions.string.inc';


abstract class LayoutAbstract implements LayoutInterface
{
    private $html = '';
    private array $labels = [];

    /** @var array */
    protected $template_dirs;

    public $getData;
    public $outerLayout;

    public function __construct()
    {
        $this->getData = [];
        $this->template_dirs = [
            LOCALPATH . '/templates/admin/',
            PPCOREPATH . '/templates/admin/',
        ];
    }

    /**
     * @param $template
     * @return $this
     */
    public function setOuterLayout($template)
    {
        $this->outerLayout = $template;
        $this->html = $this->template($template . '.tmpl');
        return $this;
    }

    public function template($filename, $label = null)
    {
        foreach ($this->template_dirs as $dir) {
            if (file_exists($dir . $filename)) {
                $html = file_get_contents($dir . $filename);

                if (is_string($label)) {
                    $this->append($label, $html);
                }

                return $html;
            }
        }

        FatalError('Template ' . $filename . ' does not exists');
    }

    /**
     * {@inheritdoc}
     */
    public function setApp(\PXApplication $app)
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setLang($lang = 'rus')
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getLang()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function getSmarty()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function getIndexTemplate()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function setContent($content)
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getContent()
    {
        return null;
    }

    public function _arrayToAttrs($array)
    {
        if (!sizeof($array)) {
            return '';
        }

        $attrs = [' '];

        foreach ($array as $k => $v) {
            $attrs[] = $k . '="' . $v . '"';
        }

        return implode(' ', $attrs);
    }

    /**
     * @param $table
     * @return $this
     * @deprecated
     */
    public function setInnerLayout($table)
    {
        $html = '<table class="inner-layout">';

        foreach ($table as $rk => $row) {
            $html .= '<tr>';

            foreach ($row as $ck => $col) {
                $td = [];

                if (!empty($col[0])) {
                    $td['width'] = $col[0];
                }
                if (!empty($col[1])) {
                    $td['style'] = 'height:' . $col[1];
                }
                if (!empty($col[2])) {
                    $td['colspan'] = $col[2];
                }
                if (!empty($col[3])) {
                    $td['rowspan'] = $col[3];
                }

                $divClass = !empty($col[1]) ? ' class="content"' : '';

                $html .= '<td' . $this->_arrayToAttrs($td) . '>';
                $html .= '<div {INNER.' . $ck . '.' . $rk . '.CONTEXT}' . $divClass . '>{INNER.' . $ck . '.' . $rk . '}</div>';
                $html .= '</td>';
            }

            $html .= '</tr>';
        }

        $html .= '</table>';

        $this->assign('OUTER.MAINAREA', $html);
        return $this;
    }

    /**
     * @return $this
     */
    public function setOneColumn()
    {
        $this->setSimpleInnerLayout(['100%'], ['100%', '']);
        return $this;
    }

    /**
     * @return $this
     */
    public function setTwoColumns($equalWidth = false)
    {
        $widths = $equalWidth ? ['50%', '50%'] : ['25%', '75%'];
        $this->setSimpleInnerLayout($widths, ['100%', '']);
        return $this;
    }

    /**
     * @return $this
     */
    public function setThreeColumns()
    {
        $this->setSimpleInnerLayout(['25%', '40%', '35%'], ['100%', '']);
        return $this;
    }

    /**
     * @param $widthArray
     * @param $heightArray
     * @return $this
     * @deprecated
     */
    public function setSimpleInnerLayout($widthArray, $heightArray)
    {
        $table = [];

        foreach ($heightArray as $hk => $height) {
            $table[$hk] = [];

            foreach ($widthArray as $wk => $width) {
                $table[$hk][$wk] = [$width, $height, null, null];
            }
        }

        $this->setInnerLayout($table);
        return $this;
    }

    /**
     * @param $action
     * @param $method
     * @param $enctype
     * @param bool $autoHeight
     * @return $this
     */
    public function setOuterForm($action, $method, $enctype, $autoHeight = false)
    {
        $this->assign('OUTER.FORMBEGIN', '<FORM action="' . $action . '" method="' . $method . '" name="outer" enctype="' . $enctype . '" class="edit' . ($autoHeight ? ' autoheight' : '') . '">');
        $this->assign('OUTER.FORMEND', '</FORM>');
        return $this;
    }

    /**
     * @param $image
     * @param $text
     * @param $width
     * @param $height
     * @param string $href
     * @return $this
     */
    public function setOuterLogo($image, $text, $width, $height, $href = '')
    {
        if (!empty($image)) {
            $html = '<a href="' . $href . '"><img src="' . $image . '" width="' . $width . '" height="' . $height . '" border="0" alt="' . $text . '" class="logo"></a>';
        } else {
            $pad = round($height - 22) / 2;
            $html = '<div style="width:' . $width . '; height:' . $height . '; text-align:center; padding:' . $pad . ';"><h1><a href="' . $href . '" style="color:#FFFFFF;">' . $text . '</a></h1></div>';
        }

        $this->assign('OUTER.LOGO', $html);
        return $this;
    }

    /**
     * @param $menuItems
     * @param $current
     * @param string $getParam
     * @param bool $buildHref
     * @return $this
     */
    public function setMenu($menuItems, $current, $getParam = 'area', $buildHref = true)
    {
        $menu = new \PXWidgetMenuTabbed();

        $menu->items = $menuItems;
        $menu->selected = $current;
        $menu->varName = $getParam;
        $menu->buildHref = $buildHref;

        $this->assign('OUTER.MENU', $menu);
        return $this;
    }

    /**
     * @param $href
     * @return $this
     */
    public function setLogoutForm($href)
    {
        $this->assign('OUTER.EXIT', $this->template('form-logout.tmpl'));
        $this->assign('LOGOUT.HREF', $href);
        $this->assign('USER.TITLE', \PXRegistry::getUser()->getTitle());
        return $this;
    }

    /**
     * @param $formAction
     * @param $formMethod
     * @param $namesArray
     * @param $valuesArray
     * @return $this
     */
    public function setLoginForm($formAction, $formMethod, $namesArray, $valuesArray)
    {
        $this->assign('OUTER.MAINAREA', $this->template('form-login.tmpl'));

        $this->assign('LOGIN.FORMACTION', $formAction);
        $this->assign('LOGIN.FORMMETHOD', $formMethod);

        $this->assign('LOGIN.LOGINNAME', getFromArray($namesArray, 'login'));
        $this->assign('LOGIN.PASSWDNAME', getFromArray($namesArray, 'passwd'));
        $this->assign('LOGIN.LOGINVALUE', quot(getFromArray($valuesArray, 'login')));
        $this->assign('LOGIN.REFERERNAME', getFromArray($namesArray, 'referer'));
        $this->assign('LOGIN.REFERERVALUE', quot(getFromArray($valuesArray, 'referer')));
        $this->assign('LOGIN.AREANAME', getFromArray($namesArray, 'area'));
        $this->assign('LOGIN.AREAVALUE', getFromArray($valuesArray, 'area'));
        $this->assign('CAPTCHA.KEY', getFromArray($valuesArray, 'captchaKey'));
        $this->assign('CAPTCHA.NOTE', getFromArray($valuesArray, 'captchaNote'));
        $this->assign('CAPTCHA.KEYNAME', getFromArray($namesArray, 'captchaKey'));
        $this->assign('CAPTCHA.VALNAME', getFromArray($namesArray, 'captchaVal'));
        return $this;
    }

    public function setGetVarToSave($key, $value)
    {
        $this->getData[$key] = $value;
        $this->assign(strtoupper((string) $key), $value);
    }

    public function clearGetVar($key)
    {
        unset($this->getData[$key]);
        $this->assign(strtoupper((string) $key), '');
    }

    public function clear($label)
    {
        $this->assign($label, null);
    }

    /**
     * alias for assign
     *
     * @param $label
     * @param $value
     * @return $this
     * @deprecated
     */
    public function set($label, &$value)
    {
        $this->labels[$label] = [];
        $this->add($label, $value);
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function assign($label, $value)
    {
        $refValue = $value;
        $this->set($label, $refValue);

        return $this;
    }

    public function isWidget($value)
    {
        return is_object($value) && $value instanceof \PXAdminWidgetIF;
    }

    /**
     * alias for append
     *
     * @param $label
     * @param $value
     * @return $this
     * @deprecated
     */
    public function add($label, &$value)
    {
        if (!isset($this->labels[$label]) || !is_array($this->labels[$label])) {
            $this->labels[$label] = [];
        }

        switch (true) {
            case $this->isWidget($value):
                $this->labels[$label][] = & $value;
                break;

            case is_scalar($value):
                $this->labels[$label][] = (string)$value;
                break;

            case is_array($value):
                $this->labels[$label][] = (array)$value;
                break;

            case is_null($value):
                // pass
                break;

            default:
                FatalError('Undefined type for layout content ' . var_export($value, true));
                break;
        }

        return $this;
    }

    /**
     * @param $label
     * @param $value
     * @return $this
     */
    public function append($label, $value)
    {
        $refValue = $value;
        $this->add($label, $refValue);
        return $this;
    }

    /**
     * @param $label
     * @param $list
     * @param $varName
     * @param $selected
     * @return $this
     */
    public function assignKeyValueList($label, $list, $varName, $selected)
    {
        $list = new \PXAdminList($list);

        $list->setVarName($varName);
        $list->setSelected($selected);
        $list->setGetData($this->getData);

        $this->assign($label, $list);
        return $this;
    }

    /**
     * @return string
     */
    public function html()
    {
        // widgets to html
        // replace labels to html
        // WARNING: DO NOT use foreach() for $this->labels, because $widget->html() calls
        // can append data to $this->labels! array_walk() acts like deprecated each() here
        array_walk($this->labels, function ($widgets, $label) {
            if (!str_contains((string) $this->html, '{' . $label . '}')) {
                return;
            }
            $html = '';
            foreach ($widgets as $widget) {
                $html .= $this->isWidget($widget) ? $widget->html() : $widget;
            }

            $this->html = str_replace('{' . $label . '}', $html, (string) $this->html);
        });

        // delete labels without content
        $this->html = preg_replace('/\{(?>\w[\w\.]*(?!\.))\}/', '', (string) $this->html);

        return $this->html;
    }

    public function flush($charset = null)
    {
        $result = $this->html();
        $response = Response::getInstance();
        $response->send($result);
    }

    // static
    public static function _buildHref($key, $value)
    {
        return static::buildHref($key, $value);
    }

    // static
    public static function buildHref($key, $value, $getData = [], $href = "?")
    {
        $layoutData = \PXRegistry::getLayout()->getData;
        $getData = array_merge($layoutData, $getData);

        parse_str((string) $href, $href_vars);

        foreach ($getData as $k => $v) {
            $flag = false;
            $flag = $flag || (is_array($v) && count($v));
            $flag = $flag || (is_string($v) && strval($v) !== "");
            $flag = $flag || (is_numeric($v));
            $flag = $flag && ($k != $key && empty($href_vars[$k]));

            if ($flag) {
                $href = appendParamToUrl($href, $k, $v);
            }
        }

        $href = parse_url((string) appendParamToUrl($href, $key, $value));
        return @"?{$href['query']}";
    }

}
