<?php
	function PXInputHandlerResize($field, $object, $extraArgs) {
		@list($width, $height) = explode('x', strtolower($extraArgs[0]));

		if(!isset($object['tmp_name'])                                                          ||
		   !is_numeric($width)                                                                  || 
		   !is_numeric($height)                                                                 ||
			 (!$width && !$height)                                                               ||
		    (isset($extraArgs[1]) && ((int)$extraArgs[1] < 0 || (int)$extraArgs[1] > 0xFFFFFF)) ||
		    (isset($extraArgs[2]) && ((int)$extraArgs[2] <= 0 || (int)$extraArgs[2] > 100))     || 
		   !($img     = getimagesize($object['tmp_name']))                                      || 
		   !array_key_exists($img['mime'], unserialize(ENUM_ALLOWED_IMAGETYPES))                || 
		   ($width >= $img[0] && $height >= $img[1])
		) return $object;

		// пропорциональный ресайз по одной из сторон
		if ($width == 0 && $img[1]) {
			$ratio = $height/$img[1];
			$width = ceil($img[0] * $ratio);
		}
		else if ($height == 0 && $img[0]) {
			$ratio = $width/$img[0];
			$height = ceil($img[1] * $ratio);
		}

		switch(true){
			case isset($extraArgs[2]):
				GDImageResize($object['tmp_name'], $object['tmp_name'], $width, $height, (int)$extraArgs[1], (int)$extraArgs[2]);
				break;
			case isset($extraArgs[1]):
				if ( substr( $extraArgs[1], 0, 1 ) == '#' ) {
					$extraArgs[1] = base_convert( substr( $extraArgs[1], 1 ), 16, 10 );
				}
				GDImageResize($object['tmp_name'], $object['tmp_name'], $width, $height, (int)$extraArgs[1]);
				break;
			default:
				 GDImageResize($object['tmp_name'], $object['tmp_name'], $width, $height);
		}
		
		$object['name'] = preg_replace('#\.[^.]*$#', '', $object['name']);
		$object['type'] = 'image/jpeg';
		$object['size'] = filesize($object['tmp_name']);
		
		return $object;
	}
?>