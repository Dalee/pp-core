<?php
 
PXDecorativeWidgetsCollection::addToCollection(
	new PXVersion('content'),
	'PXAdminForm',
	'VERSION_CONTENT',
	PXAdminForm::CONTENT
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXVersion('leftcontrols'),
	'PXAdminForm',
	'VERSION_CONTENT',
	PXAdminForm::LEFTCONTROLS
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXVersion('title'),
	'PXAdminForm',
	'VERSION_CONTENT',
	PXAdminForm::TITLE
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXVersionsTab,
	'PXWidgetTabs',
	'VERSION_CONTENT',
	PXWidgetTabs::APPEND
);

?>
