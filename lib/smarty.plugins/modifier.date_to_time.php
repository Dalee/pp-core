<?php

function smarty_modifier_date_to_time($string)
{
	if ($string == 'today') {
		return mktime(0,0,0);
	} elseif ($string == 'month') {
		return mktime(0,0,0,date('n'),1);
	} elseif ($string != '') {
		preg_match("/^(\d{2})\.(\d{2})\.(\d{4})\s+(\d{2}):(\d{2}):(\d{2})$/si", trim($string), $date);
	    	return mktime($date[4], $date[5], $date[6], $date[2], $date[1], $date[3]);

	} else {
    		return time();
	}
}

?>
