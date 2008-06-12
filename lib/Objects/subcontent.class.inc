<?php 
class PXSubContent extends PXContent {
	public $base   = 'subcontent';

	function getAllowedChilds() {
		FatalError('I can\'t have children anymore!!!!');
	}
}

?>