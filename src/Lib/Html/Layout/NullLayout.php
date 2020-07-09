<?php

namespace PP\Lib\Html\Layout;

/**
 * Class NullLayout
 * @package PP\Lib\Html\Layout
 */
class NullLayout implements LayoutInterface {

	/**
	 * {@inheritdoc}
	 */
	public function assign(string $name, $value): LayoutInterface
	{
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setLang(\PXUserHTMLLang $lang): LayoutInterface
	{
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getLang(): ?\PXUserHTMLLang
	{
		return null;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setLangCode(string $langCode): LayoutInterface
	{
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function display(): ?string
	{
		return null;
	}

}
