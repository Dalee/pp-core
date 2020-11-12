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
		0 => 'Понедельник',
		1 => 'Вторник',
		2 => 'Среда',
		3 => 'Четверг',
		4 => 'Пятница',
		5 => 'Суббота',
		6 => 'Воскресенье',
	);
*/
	$weekdays = [
		1 => 'Понедельник',
		2 => 'Вторник',
		3 => 'Среда',
		4 => 'Четверг',
		5 => 'Пятница',
		6 => 'Суббота',
		0 => 'Воскресенье',
    ];

	$weekdaysShort = [
		1 => 'Пн',
		2 => 'Вт',
		3 => 'Ср',
		4 => 'Чт',
		5 => 'Пт',
		6 => 'Сб',
		0 => 'Вс',
    ];

	$months = [
		'01' => 'Январь',
		'02' => 'Февраль',
		'03' => 'Март',
		'04' => 'Апрель',
		'05' => 'Май',
		'06' => 'Июнь',
		'07' => 'Июль',
		'08' => 'Август',
		'09' => 'Сентябрь',
		'10' => 'Октябрь',
		'11' => 'Ноябрь',
		'12' => 'Декабрь',
    ];

	$monthsE = [
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
    ];

	$months2 = [
		'01' => 'января',
		'02' => 'февраля',
		'03' => 'марта',
		'04' => 'апреля',
		'05' => 'мая',
		'06' => 'июня',
		'07' => 'июля',
		'08' => 'августа',
		'09' => 'сентября',
		'10' => 'октября',
		'11' => 'ноября',
		'12' => 'декабря',
    ];

	$months3 = [
		'01' => 'январе',
		'02' => 'феврале',
		'03' => 'марте',
		'04' => 'апреле',
		'05' => 'мае',
		'06' => 'июне',
		'07' => 'июле',
		'08' => 'августе',
		'09' => 'сентябре',
		'10' => 'октябре',
		'11' => 'ноябре',
		'12' => 'декабре',
    ];

	$monthsShort = [
		'01' => 'Янв',
		'02' => 'Фев',
		'03' => 'Мрт',
		'04' => 'Апр',
		'05' => 'Май',
		'06' => 'Июн',
		'07' => 'Июл',
		'08' => 'Авг',
		'09' => 'Сен',
		'10' => 'Окт',
		'11' => 'Ноя',
		'12' => 'Дек',
    ];


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
