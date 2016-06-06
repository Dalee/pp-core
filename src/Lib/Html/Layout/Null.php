<?php

namespace PP\Lib\Html\Layout;


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
