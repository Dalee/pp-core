<?php

class PXRichEditCKEditor extends PXRichEditDecorator {

	protected $editorName = 'CK Editor';

	function html() {
		//REMEMBER - $layout->assign...Js push its code into OUTER.FORMEND, which places after main form content
		//in the future this behaviour can be changed
		$this->parentWidget->toLayout->assignJS('/admin/tools/vendor/ckeditor/ckeditor.js');
		$this->parentWidget->toLayout->assignInlineJS(
			<<<JS
				var \$flashEditors = function(){
					for(var cb in \$editorAreas){
					\$editorAreas[cb]()
					}
				}
				if ( window.addEventListener )
					window.addEventListener( 'load', \$flashEditors, false );
				else if ( window.attachEvent )
					window.attachEvent( 'onload', \$flashEditors );
JS
		);

		$this->parentWidget->toLayout->assignInlineJS(
			<<<JS
				var \$editorAreas = \$editorAreas || [];
				\$editorAreas.push(function(){CKEDITOR.replace('{$this->parentWidget->container}')});
JS
			,
			false
		);
		return '';
	}
}
