<?php
	##############    Decorate AdminTable Widget with MultiOperations    ##############
	/* core components - wouldn't works without it */
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableMultiCheckboxes, 
		'PXAdminTable', 
		'MULTIOPERATIONS',
		PXAdminTable::BEFORE_CONTENT,
		'core_component'
	);
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableBottomLabel, 
		'PXAdminTable', 
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT,
		'core_component'
	);
	/* default multioperations widgets */
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableCommonControls, 
		'PXAdminTable', 
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT
	);
	PXDecorativeWidgetsCollection::addToCollection(
		new PXAdminTableChangeParent, 
		'PXAdminTable', 
		'MULTIOPERATIONS',
		PXAdminTable::AFTER_CONTENT
	);
?>