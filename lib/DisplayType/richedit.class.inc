<?php

require_once PPLIBPATH . 'DisplayType/text.class.inc';

/**
 * Визуальный редактор
 *
 * storageType string
 * displayType RICHEDIT|width|height
 *             width не используется
 *             height это высота в пикселях
 */
class PXDisplayTypeRichedit extends PXDisplayTypeText
{
    public function buildInput($field, $object, $param = NULL)
    {
        $value = $object[$field->name] ?? NULL;
        $req = PXRegistry::getRequest();
        $layout = PXRegistry::getLayout();
        $redactor = $req->getCookieVar('richedit');
        $html = NLAbstractHTMLForm::BuildRichEdit($field->name, $value, $field->displayTypeArgs[1], $redactor);
        $layout->assignInlineJS(<<<JS
			var selects = document.getElementsByTagName('select');
			for(var i = 0; i < selects.length; i++) {
				if(selects[i].parentNode.className == 'redactor') {
					selects[i].onchange = function() {
						var date = new Date();
						date = new Date (date.getFullYear() + 1, date.getMonth(), date.getDate(),  23, 59, 59);
						document.cookie = ['richedit=', this.value, '; expires=', date.toUTCString()].join('');
						location.reload();
					}
				}
			}
JS
        );
        return $html;
    }
}
