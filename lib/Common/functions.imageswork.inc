<?php
function GDInDrawPicture($src_im, &$dst_im) {
	$src_width  = ImageSX($src_im);
	$src_height = ImageSY($src_im);
	$dst_width  = ImageSX($dst_im);
	$dst_height = ImageSY($dst_im);

	if ($src_width/$src_height < $dst_width/$dst_height) {
		$new_width = ($dst_height*$src_width)/$src_height;
		ImageCopyResized($dst_im, $src_im, $dst_width/2-$new_width/2, 0, 0, 0, $new_width, $dst_height, $src_width, $src_height);
	} else {
		$new_height = ($dst_width*$src_height)/$src_width;
		ImageCopyResized($dst_im, $src_im, 0, $dst_height/2-$new_height/2, 0, 0, $dst_width, $new_height, $src_width, $src_height);
	}
}

function FindMagicTools($component){
	($magicPath = FindSystemFile($component)) or FatalError('Cant find ImageMagick '.strtoupper($component).' tools');
	return $magicPath;
}

function FindNconvert() {
	return FindMagicTools('nconvert');
}

function FindConvert() {
	return FindMagicTools('convert');
}

function NConvertInDrawPicture($src_filename, $dst_filename) {
	$src_im = ImageCreateFromJpeg($src_filename);
	$dst_im = ImageCreateFromJpeg($dst_filename);
	$src_width  = ImageSX($src_im);
	$src_height = ImageSY($src_im);
	$dst_width  = ImageSX($dst_im);
	$dst_height = ImageSY($dst_im);
	$nconvert = FindNconvert();

	if ($src_width/$src_height < $dst_width/$dst_height) {
		$new_width = round(($dst_height*$src_width)/$src_height);
		$fname = "/tmp/".time().".".getmypid().".jpg";
		system($nconvert." -v -o ".$fname." -ratio -resize $new_width 0 $src_filename > /dev/null 2>/dev/null");
		$tmp_im = ImageCreateFromJpeg($fname);
		$tmp_width  = ImageSX($tmp_im);
		$tmp_height = ImageSY($tmp_im);
		ImageCopyResized($dst_im, $tmp_im, $dst_width/2-$new_width/2, 0, 0, 0, $new_width, $dst_height, $tmp_width, $tmp_height);

	} else {
		$new_height = round(($dst_width*$src_height)/$src_width);
		$fname = "/tmp/".time().".".getmypid().".jpg";
		system($nconvert." -v -o ".$fname." -ratio -resize 0 $new_height $src_filename > /dev/null 2>/dev/null");
		$tmp_im = ImageCreateFromJpeg($fname);
		$tmp_width  = ImageSX($tmp_im);
		$tmp_height = ImageSY($tmp_im);
		ImageCopyResized($dst_im, $tmp_im, 0, $dst_height/2-$new_height/2, 0, 0, $dst_width, $new_height, $tmp_width, $tmp_height);
	}
	unlink($fname);
	ImageDestroy($src_im);
	ImageDestroy($tmp_im);
	return $dst_im;
}

function GDSafeImageResize($im, $x, $y) {
	$width  = ImageSX($im);
	$height = ImageSY($im);
	if ($width/$height < $x/$y) {
		$blank = ImageCreate($y*$width/$height, $y);
		ImageCopyResized($blank, $im, 0, 0, 0, 0, $y*$width/$height, $y, $width, $height);
		return $blank;
	} else {
		$blank = ImageCreate($x, $x*$height/$width);
		ImageCopyResized($blank, $im, 0, 0, 0, 0, $x, $x*$height/$width, $width, $height);
		return $blank;
	}
}

function GDImageResize($src, $dest, $width, $height, $rgb=0xFFFFFF, $quality=100) {
	if (!file_exists($src)) {
		FatalError('File is not exists');
	}

	$size = getimagesize($src);

	if ($size === false) {
		FatalError('File ('.$src.') have null size');
	}

	// Определяем исходный формат по MIME-информации, предоставленной
	// функцией getimagesize, и выбираем соответствующую формату
	// imagecreatefrom-функцию.
	$format = strtolower(substr($size['mime'], strpos($size['mime'], '/')+1));
	$icfunc = "imagecreatefrom" . $format;
	if (!function_exists($icfunc)) {
		FatalError('Can\'t find function '.$icfunc);
	}

	$x_ratio = $width / $size[0];
	$y_ratio = $height / $size[1];

	$ratio       = min($x_ratio, $y_ratio);
	$use_x_ratio = ($x_ratio == $ratio);

	$new_width   = $use_x_ratio  ? $width  : floor($size[0] * $ratio);
	$new_height  = !$use_x_ratio ? $height : floor($size[1] * $ratio);
	$new_left    = $use_x_ratio  ? 0 : floor(($width - $new_width) / 2);
	$new_top     = !$use_x_ratio ? 0 : floor(($height - $new_height) / 2);

	$isrc = $icfunc($src);
	$idest = imagecreatetruecolor($width, $height);

	$r = $rgb>>16;
	$g = $rgb>>8&0xFF;
	$b = $rgb&0xFF;

	imagefill($idest, 0, 0, imagecolorallocate($idest, $r, $g, $b));

	imagecopyresampled($idest, $isrc, $new_left, $new_top, 0, 0,
	$new_width, $new_height, $size[0], $size[1]);

	imagejpeg($idest, $dest, $quality);

	imagedestroy($isrc);
	imagedestroy($idest);

	return true;
}


function MagicResize($im_filename, $x, $y) {
	$oldName = $im_filename;
	if (ereg("\.JPG$", $im_filename)) {
		$im_filename = str_replace(".JPG", ".jpg", $im_filename);
		rename($oldName, $im_filename);
	}


	$convert = FindConvert();
	system($convert." ".$im_filename." -resize ".$x."x".$y." ".$im_filename." > /dev/null 2>/dev/null");

	if ($oldName !== $im_filename) {
		rename($im_filename, $oldName);
	}
}



function NConvertSafeImageResize($im_filename, $x, $y) {
	$oldName = $im_filename;
	if (ereg("\.JPG$", $im_filename)) {
		$im_filename = str_replace(".JPG", ".jpg", $im_filename);
		rename($oldName, $im_filename);
	}

	$im = ImageCreateFromJpeg($im_filename);
	$width  = ImageSX($im);
	$height = ImageSY($im);
	ImageDestroy($im);

	$nconvert = FindNconvert();
	if ($width/$height < $x/$y) {
		system($nconvert." -v -ratio -resize 0 $y ".$im_filename." > /dev/null 2>/dev/null");
	} else {
		system($nconvert." -v -ratio -resize $x 0 ".$im_filename." > /dev/null 2>/dev/null");
	}

	if ($oldName !== $im_filename) {
		rename($im_filename, $oldName);
	}
}

function MakeWaterMark(&$image, $watermark, $coeff=30) {
	$image_width      = ImageSX($image);
	$image_height     = ImageSY($image);
	$watermark_width  = ImageSX($watermark);
	$watermark_height = ImageSY($watermark);
	ImageCopyMerge($image, $watermark, $image_width/2-$watermark_width/2, $image_height/2-$watermark_height/2, 0, 0, $watermark_width, $watermark_height, $coeff);
}

function ReadJFIFbyPerl($filename, $section='caption') {
	$a = `perl -mJPEG::JFIF -e 'print 1'`;
	if ($a != "1") {
		FatalError("Please, install JPEG::JFIF perl library!");
	}
	$perl_string = 'use JPEG::JFIF; use strict; my $jfif = new JPEG::JFIF; $jfif->read("'.$filename.'"); print $jfif->getdata("'.$section.'");';
	$a = `/bin/echo '$perl_string' | perl`;
	return $a;
}

function GetImageProperty($file) {
	$retArray = array();
	$size = getimagesize($file);
	$retArray['width'] = $size[0];
	$retArray['height'] = $size[1];

	switch($size[2]) {
		case '1':
			$type = 'GIF';
			break;
		case '2':
			$type = 'JPG';
			break;
		case '3':
			$type = 'PNG';
			break;
		case '4':
			$type = 'SWF';
			break;
		case '5':
			$type = 'PSD';
			break;
		case '6':
			$type = 'BMP';
			break;
		case '7':
			$type = 'TIFF';
			break;
		case '8':
			$type = 'TIFF';
			break;
		case '9':
			$type = 'JPC';
			break;
		case '10':
			$type = 'JP2';
			break;
		case '11':
			$type = 'JPX';
			break;
		default:
			$type = 'undefined';
			break;
	}
	$retArray['type'] = $type;
	$byteSize = filesize($file);
	$retArray['byteSize'] = $byteSize;
	if ($byteSize < 1024) {
		$retArray['xByteSize'] = $byteSize.' байт';
	} elseif ($byteSize < 1048576) {
		$retArray['xByteSize'] = sprintf("%01.2f", ($byteSize/1024)).' Кбайт';
	} else {
		$retArray['xByteSize'] = sprintf("%01.2f", ($byteSize/1048576)).' Кбайт';
	}
	return $retArray;
}


?>
