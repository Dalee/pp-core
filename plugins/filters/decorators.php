<?php

PXDecorativeWidgetsCollection::addToCollection(
	new PXFiltersAdminWidget('filters'),
	'PXAdminTable',
	'FILTERS',
	PXAdminTable::BEFORE_CONTENT_ALWAYS
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXOnPageAdminWidget('filters', 'before'),
	'PXAdminPager',
	'ONPAGE',
	PXAdminPager::AFTER_PAGE_LIST
);

PXDecorativeWidgetsCollection::addToCollection(
	new PXOnPageAdminWidget('filters', 'after'),
	'PXAdminPager',
	'ONPAGE',
	PXAdminPager::AFTER_PAGE_LIST
);

?>
