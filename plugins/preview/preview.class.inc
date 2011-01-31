<?php

class PXPluginPreview extends PXAbstractPlugin {
	const MODULE_NAME = 'adminpreview';
	const PREVIEW_ACL_TYPE = 'admin_preview';
	const ENABLE_PREVIEW_FLAG = 'admin_preview';

	public function initialize($app) {
		$this->loadModule(self::MODULE_NAME);
		$this->loadTrigger('database/preview');
		$this->loadOnlyInAdmin('lib/widgets/preview.class.inc');
		$this->loadOnlyInAdmin('decorators.php');
	}
}

?>
