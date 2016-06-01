<?php

namespace PP\Lib\Html\Layout;

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
	 * @param string $lang
	 * @return $this
	 */
	function setLang($lang = 'rus');
}
