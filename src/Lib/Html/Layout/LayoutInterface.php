<?php

namespace PP\Lib\Html\Layout;

/**
 * Base layout interface for Admin and Client-side layouts
 * action/json handlers always uses Null layout.
 *
 * Interface LayoutInterface
 * @package PP\Lib\Html\Layout
 */
interface LayoutInterface
{
    /**
     * @param string $name
     * @param string $value
     * @return $this
     */
    public function assign($name, $value);

    /**
  * @return $this
  */
    public function setApp(\PXApplication $app);

    /**
     * TODO: should be refactored to setLangCode
     *
     * @param string $lang
     * @return $this
     */
    public function setLang($lang = 'rus');

    /**
     *
     * @return \PXUserHTMLLang
     */
    public function getLang();

    /**
     * @return \Smarty
     */
    public function getSmarty();

    /**
     * @return string
     */
    public function getIndexTemplate();

    /**
     * Null layout in action/json handler Null layout is used, so it should be in interface
     *
     * @param object
     * @return $this
     */
    public function setContent($content);

    /**
     * Null layout in action/json handler Null layout is used, so it should be in interface
     *
     * @return null|\PPBEMJSONContent
     */
    public function getContent();
}
