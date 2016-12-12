<?php

namespace PP\Lib\Html\Layout;

/**
 * Base layout interface for Admin and Client-side layouts
 * action/json handlers always uses Null layout.
 *
 * Interface LayoutInterface
 * @package PP\Lib\Html\Layout
 */
interface LayoutInterface {

	/**
	 * @param string $name
	 * @param string $value
	 * @return $this
	 */
	function assign($name, $value);

	/**
	 * @param \PXApplication $app
	 * @return $this
	 */
	function setApp(\PXApplication $app);

	/**
	 * TODO: should be refactored to setLangCode
	 *
	 * @param string $lang
	 * @return $this
	 */
	function setLang($lang = 'rus');

	/**
	 *
	 * @return \PXUserHTMLLang
	 */
	function getLang();

	/**
	 * @return \Smarty
	 */
	function getSmarty();

	/**
	 * @return string
	 */
	function getIndexTemplate();

	/**
	 * Null layout in action/json handler Null layout is used, so it should be in interface
	 *
	 * @param object
	 * @return $this
	 */
	function setContent($content);

	/**
	 * Null layout in action/json handler Null layout is used, so it should be in interface
	 *
	 * @return null|\PPBEMJSONContent
	 */
	function getContent();
}
