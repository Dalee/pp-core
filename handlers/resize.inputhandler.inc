<?php
	function PXInputHandlerResize($field, $object, $extraArgs) {
		@list($width, $height) = explode('x', strtolower($extraArgs[0]));

		if(!isset($object['tmp_name'])                                           || 
		   !is_numeric($width)                                                   || 
		   !is_numeric($height)                                                  || 
		    $width  <= 0                                                         || 
		    $height <= 0                                                         || 
		   !$img     = getimagesize($object['tmp_name'])                         || 
		   !array_key_exists($img['mime'], unserialize(ENUM_ALLOWED_IMAGETYPES)) || 
		   ($width >= $img[0] && $height >= $img[1])
		) return $object;
		
		GDImageResize($object['tmp_name'], $object['tmp_name'], $width, $height);
		
		$object['name'] = preg_replace('#\.[^.]*$#', '', $object['name']);
		$object['type'] = 'image/jpeg';
		$object['size'] = filesize($object['tmp_name']);
		
		return $object;
	}
?>