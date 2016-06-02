<?php

namespace PP\Lib\Html\Layout;


class Null implements LayoutInterface {

	/**
	 * {@inheritdoc}
	 */
	function assign($name, $value) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	function setApp(\PXApplication $app) {
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	function setLang($lang = 'rus') {
		return $this;
	}
}
