<?php
	//TODO: we urgently need in plugins abstraction, it is plugins decorator config prototype
	##############    Decorate AdminTable Widget with MultiOperations    ##############
	/* core components - wouldn't works without it */
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableMultiCheckboxes(),
		'PXAdminTable',
		'MULTIOPERATIONS',
		PXAdminTable::BEFORE_CONTENT,
		'core_component'
	);
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableBottomLabel(),
		'PXAdminTable',
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT,
		'core_component'
	);
	/* default multioperations widgets */
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableCommonControls(),
		'PXAdminTable',
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT
	);
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableChangeParent(),
		'PXAdminTable',
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT
	);

	##############    Decorate TabbedMenu Widget with Regions Changer    ##############
/*	PXDecorativeWidgetsCollection::addToCollection(
		new PXTabbedMenuRegionChanger,
		'PXWidgetTabbedMenu',
		'INDEX_EXTRA_CONTROLS',
		PXWidgetTabbedMenu::BEFORE_TABS
	);*/

	##############    Decorate Richedit Widget with various richeditors    ##############
	PXDecorativeWidgetsCollection::addToCollection(
		new PXRichEditHTMLArea(),
		'PXInputRichedit',
		'RICHEDITORS',
		PXInputRichedit::AFTER_TEXTAREA
	);

	PXDecorativeWidgetsCollection::addToCollection(
		new PXRichEditCKEditor(),
		'PXInputRichedit',
		'RICHEDITORS',
		PXInputRichedit::AFTER_TEXTAREA
	);
?>
