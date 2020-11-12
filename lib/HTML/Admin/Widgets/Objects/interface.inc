<?php

interface IPXAdminObjects
{
//	function showCaption();
//	function hideCaption();
    public function appendControl($name, $template);

    public function has($id);

    public function get($id);
//	function showChildren($requestVarName);
}
?>
