<?php

namespace PP\Lib\Html\Layout;

use PXUserHTMLLang;

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
	 * @param mixed $value
	 * @return $this
	 */
	function assign(string $name, $value): LayoutInterface;

	/**
	 * @param PXUserHTMLLang $lang
	 * @return $this
	 */
	function setLang(PXUserHTMLLang $lang): LayoutInterface;

	/**
	 * @param string $langCode
	 * @return $this
	 */
	function setLangCode(string $langCode): LayoutInterface;

	/**
	 * @return PXUserHTMLLang|null
	 */
	function getLang(): ?PXUserHTMLLang;

	/**
	 * @return string|null
	 */
	function display(): ?string;

	/**
	 * @param bool $value
	 * @return LayoutInterface
	 */
	function setDebug(bool $value): LayoutInterface;
}
