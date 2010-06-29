<?php

if($directory = @PXRegistry::getApp()->directory[$field->source]) {
	$dir_object = $directory->values[$object[$field->name]];
	return (string) @$dir_object[$directory->displayField];
}

return "";

?>
