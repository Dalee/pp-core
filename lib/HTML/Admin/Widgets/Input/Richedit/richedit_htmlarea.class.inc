<?php

class PXRichEditHTMLArea extends PXRichEditDecorator
{

    protected $editorName = 'HTML Area (IE only)';

    public function html()
    {
        //REMEMBER - $layout->assign...Js push its code into OUTER.FORMEND, which places after main form content
        //in the future this behaviour can be changed
        $this->parentWidget->toLayout->assignJS('/admin/tools/htmlarea/init.js');
        $this->parentWidget->toLayout->assignInlineJS(
            <<<JS
				editor_generate('{$this->parentWidget->container}');
JS
            ,
            false
        );
        return '';
    }
}
