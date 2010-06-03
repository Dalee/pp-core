<?php
	abstract class PXRichEditDecorator extends PXAdminWidget {
		
		protected $editorName;
		
		function initiate(){
			$this->parentWidget->addEditor($this->editorName);
		}
	}

?>