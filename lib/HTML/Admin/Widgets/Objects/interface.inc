<?php
interface IPXAdminObjects {
//	function showCaption();
//	function hideCaption();
	function appendControl($name, $template);
	function has($id);
	function get($id);
//	function showChildren($requestVarName);
}
?>