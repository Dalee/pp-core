<?php

/*
 * Smarty plugin
 * -------------------------------------------------------------
 * Type:     modifier
 * Name:     date_format
 * Purpose:  format datestamps via strftime
 * Input:    string: input date string
 *           format: strftime format for output
 *           default_date: default date if $string is empty
 * -------------------------------------------------------------
 */
require_once $smarty->_get_plugin_filepath('shared', 'make_timestamp');

function smarty_modifier_date_format($string, $format="%b %e, %Y", $default_date=null, $default_strftime=null) {
/*
	$weekdays = array(
		0 => '�����������',
		1 => '�������',
		2 => '�����',
		3 => '�������',
		4 => '�������',
		5 => '�������',
		6 => '�����������',
	);
*/
	$weekdays = array(
		1 => '�����������',
		2 => '�������',
		3 => '�����',
		4 => '�������',
		5 => '�������',
		6 => '�������',
		0 => '�����������',
	);

	$weekdaysShort = array(
		1 => '��',
		2 => '��',
		3 => '��',
		4 => '��',
		5 => '��',
		6 => '��',
		0 => '��',
	);

	$months = array(
		'01' => '������',
		'02' => '�������',
		'03' => '����',
		'04' => '������',
		'05' => '���',
		'06' => '����',
		'07' => '����',
		'08' => '������',
		'09' => '��������',
		'10' => '�������',
		'11' => '������',
		'12' => '�������',
	);

	$monthsE = array(
		'01' => 'January',
		'02' => 'February',
		'03' => 'March',
		'04' => 'April',
		'05' => 'May',
		'06' => 'June',
		'07' => 'July',
		'08' => 'August',
		'09' => 'September',
		'10' => 'October',
		'11' => 'November',
		'12' => 'December',
	);

	$months2 = array(
		'01' => '������',
		'02' => '�������',
		'03' => '�����',
		'04' => '������',
		'05' => '���',
		'06' => '����',
		'07' => '����',
		'08' => '�������',
		'09' => '��������',
		'10' => '�������',
		'11' => '������',
		'12' => '�������',
	);

	$months3 = array(
		'01' => '������',
		'02' => '�������',
		'03' => '�����',
		'04' => '������',
		'05' => '���',
		'06' => '����',
		'07' => '����',
		'08' => '�������',
		'09' => '��������',
		'10' => '�������',
		'11' => '������',
		'12' => '�������',
	);

	$monthsShort = array(
		'01' => '���',
		'02' => '���',
		'03' => '���',
		'04' => '���',
		'05' => '���',
		'06' => '���',
		'07' => '���',
		'08' => '���',
		'09' => '���',
		'10' => '���',
		'11' => '���',
		'12' => '���',
	);


	$time = strlen(trim($string)) ? $string : (isset($default_date) ? $default_date : '');
	if (!is_numeric($time)) $time = smarty_make_timestamp($time);

	if (!$default_strftime) {
//		$format = str_replace('%a',  $weekdaysShort[strftime('%u', $time)-1], $format);
//		$format = str_replace('%A',  $weekdays[     strftime('%u', $time)-1], $format);

		$format = str_replace('%a',  $weekdaysShort[strftime('%w', $time)], $format);
		$format = str_replace('%A',  $weekdays[     strftime('%w', $time)], $format);

		$format = str_replace('%b',  $monthsShort[  strftime('%m', $time)], $format);
		$format = str_replace('%B3', $months3[      strftime('%m', $time)], $format);
		$format = str_replace('%B2', $months2[      strftime('%m', $time)], $format);
		$format = str_replace('%BE', $monthsE[      strftime('%m', $time)], $format);
		$format = str_replace('%B',  $months[       strftime('%m', $time)], $format);
		$format = str_replace('%e',  strftime('%d', $time) > 9 ? strftime('%d', $time) : substr(strftime('%d', $time), 1), $format);
		$format = str_replace('%h',  $monthsShort[  strftime('%m', $time)], $format);
	}

	if ($string != '') {
		return strftime($format, smarty_make_timestamp($string));

	} elseif (isset($default_date) && $default_date != '') {
		return strftime($format, smarty_make_timestamp($default_date));

	} else {
		return;
	}
}
?>
