<?php

class CheckerObject {

	/**
	 * @param $format
	 * @return ModuleChecker|ObjectChecker
	 */
	public static function get(&$format) {
		if ($format === NULL) {
			$format = new PXTypeDescription;
		}

		if ($format instanceof PXTypeDescription) {
			return new ObjectChecker();
		}

		if ($format instanceof PXModuleDescription) {
			return new ModuleChecker();
		}

		FatalError("This Acl type doesn't implements!");
	}
}
