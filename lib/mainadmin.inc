<?php
require_once 'maincommon.inc';

require_once 'HTML/Abstract/layout.class.inc';
require_once 'HTML/Abstract/htmlform.class.inc';

require_once 'HTML/Admin/layout.class.inc';
require_once 'HTML/Admin/objectform.class.inc';

require_once 'HTML/Admin/Decorators/widgetscollection.class.inc';

require_once 'HTML/Admin/Widgets/classes.inc';

require_once 'HTML/Admin/admin_setup.inc';

if(class_exists('PXInputRichEdit')){
	class PXHelperRichEditHtmlArea {
		function init(&$richEdit){
			//REMEMBER - $layout->assign...Js push its code into OUTER.FORMEND, which places after main form content
			//in the future this behaviour can be changed
			$richEdit->toLayout->assignJS('/admin/tools/htmlarea/init.js');
			$richEdit->toLayout->assignInlineJS(<<<JS
				editor_generate('{$richEdit->container}');
JS
			, false);
			return '';
		}
	}
	
	PXInputRichEdit::addEditor('htmlarea', array('name' => 'HTML Area (IE only)', 'init_script' => new PXHelperRichEditHtmlArea));

	class PXHelperRichEditCKEditor {
		function init(&$richEdit){
			//REMEMBER - $layout->assign...Js push its code into OUTER.FORMEND, which places after main form content
			//in the future this behaviour can be changed
			$richEdit->toLayout->assignJS('/admin/tools/vendor/ckeditor/ckeditor.js');
			$richEdit->toLayout->assignInlineJS(<<<JS
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

			$richEdit->toLayout->assignInlineJS(<<<JS
				var \$editorAreas = \$editorAreas || [];
				\$editorAreas.push(function(){CKEDITOR.replace('{$richEdit->container}')});
JS
			, false);
			return '';
		}
	}
	
	PXInputRichEdit::addEditor('ckeditor', array('name' => 'CK Editor', 'init_script' => new PXHelperRichEditCKEditor));
	
	PXInputRichEdit::addEditor('', array('name' => 'Без редактора', 'init_script' => null));
}

require_once BASEPATH.'/local/lib/mainadmin.inc';

?>