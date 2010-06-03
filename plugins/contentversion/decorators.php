<?php
require_once(BASEPATH . '/libpp/plugins/contentversion/widget.class.inc');
 
PXDecorativeWidgetsCollection::addToCollection(
	new PXVersion,
	'PXAdminForm',
	'VERSION_CONTENT',
	PXAdminForm::CONTENT
);

/*PXDecorativeWidgetsCollection::addToCollection(
	new ,
	'PXAdminForm',
	'VERSION_CONTENT',
	PXAdminForm::CONTENT
);*/

?>
