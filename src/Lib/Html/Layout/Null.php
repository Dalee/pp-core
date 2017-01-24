<?php

namespace PP\Lib\Html\Layout;

/**
 * Class Null
 * @package PP\Lib\Html\Layout
 */
class Null implements LayoutInterface {

	/**
	 * {@inheritdoc}
	 */
	public function assign($name, $value) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setApp(\PXApplication $app) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setLang($lang = 'rus') {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	function getLang() {
		return null;
	}

	/**
	 * {@inheritdoc}
	 */
	function getSmarty() {
		return null;
	}

	/**
	 * {@inheritdoc}
	 */
	function getIndexTemplate() {
		return null;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setContent($content) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function getContent() {
		return null;
	}

}
