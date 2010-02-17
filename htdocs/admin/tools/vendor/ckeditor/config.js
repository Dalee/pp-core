/*
Copyright (c) 2003-2009, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

CKEDITOR.editorConfig = function( config ) {
	// Define changes to default configuration here. For example:
	// config.language = 'fr';
	// config.uiColor = '#AADC6E';
	config.language = 'ru';
	config.skin     = 'v2';
	config.toolbar = 'Basic';

	config.toolbar_Basic =
	[
		['Source','-','Maximize'],
		['Cut','Copy','Paste', 'PasteText','PasteFromWord', 'SpellChecker', 'Scayt'],
		['Undo','Redo','-','Find','Replace', 'ShowBlocks'],
		'/',
		['Bold', 'Italic','Underline', 'Strike'], ['NumberedList', 'BulletedList', 'Table'], ['Format','FontSize'],
		'/',
		['Outdent','Indent','Blockquote','Subscript', 'Superscript'],
		['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock'],
		['Image', 'Link','Unlink','Anchor','HorizontalRule','SpecialChar', 'TextColor','BGColor']
	];
};
