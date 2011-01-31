<?php

PXDecorativeWidgetsCollection::addToCollection(
	new PXFiltersAdminWidget,
	'PXAdminTable',
	'FILTERS',
	PXAdminTable::BEFORE_CONTENT_ALWAYS
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXOnPageAdminWidget('before'),
	'PXAdminPager',
	'ONPAGE',
	PXAdminPager::AFTER_PAGE_LIST
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXOnPageAdminWidget('after'),
	'PXAdminPager',
	'ONPAGE',
	PXAdminPager::AFTER_PAGE_LIST
);

?>
