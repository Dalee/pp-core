<?php
class NLMailVEvent {
	
	var $_calScale;        //��� ���������, ���� �� ����������, �� ��������� GREGORIAN
	var $_prodId;		   // �������� ������ - ������������ ��������
	var $_method;
	
	var $_requiredProps;   // ������ ������������ ����� ������� vEvent ����������� ����
	var $_uselessProps;    // ������ �������������� ����� ������� vEvent ����������� ����
	var $eventProps;       // ��� ���� ������� vEvent
	
	function NLMailVEvent() {
		$this->_calScale = null;                    //��� ���������, ���� �� ����������, �� ��������� GREGORIAN
		$this->_prodId   = '-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN'; // �������� ������ - ������������ ��������
		
		$this->eventProps = array();
		$this->setDTStamp(time());
		$this->setSequence(0);
		$this->setSummary('');
	}
	
	function getCalendarContent() {
		$eventContent = $this->_createEvent();
		
		$content = "BEGIN:VCALENDAR\n";
		if ($this->_calScale) $content .= $this->_calScale . "\n";
		$content .= "PRODID:" .$this->_prodId . "\n";
		$content .= "VERSION:2.0\n";
		$content .= "METHOD:" .$this->_method . "\n";
		if ($eventContent) $content .= $eventContent . "\n";
		$content .= "END:VCALENDAR\n";
		
		return iconv("koi8-r", "utf-8", $content);
	}
	
	function _createEvent() {
		$this->setAttendee($this->toEmail);
		$this->setOrganizer($this->fromEmail);
		
		$required = $this->_checkRequired();
		if ($required) {
			$this->setError('�� �� ���������������� �������� ' . $required . ',��������� ��� ������� ���� �������');
			return false;
		}
		
		$useless = $this->_checkUseless();
		if ($useless) {
			$this->setError('C������� ' . $useless . '�� ������ ���� ������������ ��� ������� ���� �������');
			return false;
		}
		$content = "BEGIN:VEVENT\n" . implode("\n", $this->eventProps) . "\nEND:VEVENT";
		return $content;
	}
	
	/**
	 * ��������, ������������ �� ��� ����������� �������� ��� ������� ���� �������
	 */
	function _checkRequired() {
		foreach ($this->_requiredProps as $propname) {
			if (!isset($this->eventProps[$propname])) {
				return $propname;
			}
		}
		return false;
	}
	
	/**
	 * �������� �� ���������� ����������� ������� ��� ������� ���� �������
	 */	
	function _checkUseless() {
		foreach ($this->_uselessProps as $propname) {
			if (isset($this->eventProps[$propname])) {
				return $propname;
			}
		}
		return false;
	}
	
	/**
	 * ������������� ��� ���������, ���� �� ������, �� ��������� GREGORIAN	
	 */
	function setCalScale($scale) {
		$this->_calScale = 'CALSCALE:' . $scale;
	}
	
	/**
	 * ������������� ��������� �������
	 */
	function setProdId($prodId) {
		$this->_prodId = $prodId;
	}
	
	/**
	 * ����������� ����� � ��������� vCalendar
	 *
	 * @param timestamp $date
	 */
	function formatDT($date) {
		return date('Ymd', $date) . 'T' . date('His', $date) . 'Z';
	}
	
	/**
	 * ������������� �����������
	 *
	 * @param string $trigger
	 * @param string $description ��������
	 * @param string $duration �������� ������� ��� �����������
	 * @param string $repeat
	 */
	function setAlarm($trigger, $description = 'Reminder', $duration = null, $repeat = null) {
		$alarm  = "BEGIN:VALARM\n";
		$alarm .= "TRIGGER:" . $trigger . "\n";
		$alarm .= "ACTION:DISPLAY\n";
		$alarm .= "DESCRIPTION:" . $description . "\n";
		if ($duration) $alarm .= 'DURATION:' . $duration . "\n";
		if ($repeat)   $alarm .= 'REPEAT:'   . $repeat   . "\n";  // ����� ����������, ������������ ������ � DURATION
		$alarm .= "END:VALARM";
		
		$this->eventProps['alarm'] = $alarm;
	}
	
	/**
	 * ������������� �������� ������� - �� ����������� �������� ������������ outlook
	 *
	 * @param int $value
	 */
	function setXImportance($value) {
		$this->eventProps['XImportance'] = 'X-MICROSOFT-CDO-IMPORTANCE:' . $value;
	}
	
	/**
	 * ���������� ��������� �������
	 * �������� BUSINESS,HUMAN RESOURCES,ANNIVERSARY,PERSONAL,SPECIAL OCCASION � �.� - �� ������������
	 * 
	 * @param string $text
	 */
	function setCategories($text) {
		$this->eventProps['categories'] = 'CATEGORIES:' . $rId;
	}
	
	/**
	 * ���������� ����� � ������ ����������� vCalendar �� UID
	 *
	 * @param string $rId
	 */
	function setRelated($rId) {
		$this->eventProps['related'] = 'RELATED-TO:<' . $rId . '>'; 
	}

	/**
	 * ���������� ������-���, ������� ������ ������ ���������� ����������.
	 * ��������: REQUEST-STATUS:2.0;Success
	 *
	 * @param string $status
	 */
	function setRequestStatus($status) {
		$this->eventProps['requestStatus'] = 'REQUEST-STATUS:' . $status;
	}

	/**
	 * ������ url ����� ���������� ����
	 * ��������:  TZURL:http://timezones.r.us.net/tz/US-California-Los_Angeles. �� ������������
	 *
	 * @param string $url
	 */
	function setTZurl($url) {
		$this->eventProps['url'] = 'TZURL:' . $url;
	}

	/**
	 * ���������� �������, �������� ���������� �������
	 *
	 * @param string $rule
	 */
	function setRecurrenceRule($rule) {
		$this->eventProps['recurrenceRule'] = 'RRULE:' . $rule;
	}

	/**
	 * ������������� ����, �������� ���������� �������
	 *
	 * @param string $date ���� + ���������
	 */
	function setRecurrenceDate($date) {
		$this->eventProps['recurrenceDate'] = 'RDATE:' . $date;
	}

	/**
	 * ���������� �������, ��������, ���������� �� ������ 10 ��� � ���� � �.�
	 *
	 * @param string $rule
	 */
	function setsetExeptionRule($rule) {
		$this->eventProps['exceptionRule'] = 'EXRULE:' . $rule;
	}

	/**
	 * ������������� ����-���������� ��� �������
	 *
	 * @param array of timestamp $datesArray
	 */
	function setExeptionDate($datesArray) {
		$array = array_map(array($this, 'formatDT'), $datesArray);
		$this->eventProps['exeptionDate'] = 'EXDATE:' . implode(",", $array);
	}

	/**
	 * ������������� �������������� ��� �������������� ���������� ���������� 
	 *
	 * @param string $text
	 */
	function setContact($text) {
		$this->eventProps['contact'] = 'CONTACT:' . $text;
	}
	
	/**
	 * ������������� ����������� � �������
	 *
	 * @param string $text
	 */
	function setComment($text) {
		$this->eventProps['comment'] = 'COMMENT:' . $text;
	}
	
	/**
	 * ������������� ����������� �������� � ������� vCalendar
	 *
	 * @param string $value uri + ���������
	 */
	function setAttach($value) {
		$this->eventProps['attach'] = 'ATTACH:' . $value;
	}

	/**
	 * ������������� ������������ �������, ����� �������������� ������ DTEND
	 *
	 * @param int $weeks
	 * @param int $days
	 * @param int $hours
	 * @param int $minutes
	 * @param int $seconds
	 */
	function setDuration($weeks, $days, $hours, $minutes, $seconds) {
		$duration = 'P';
		if ($weeks) $duration .= $weeks . 'W';
		if ($days)  $duration .= $days  . 'D';
		
		if ($hours || $minutes || $seconds) $duration .= 'T';
		
		if ($hours)   $duration .= $hours   . 'H';
		if ($minutes) $duration .= $minutes . 'M';
		if ($seconds) $duration .= $seconds . 'S';
		
		$this->eventProps['duration'] = 'DURATION:' . $duration;
	}
	
	function setRecurrenceId($value) {
		$this->eventProps['recurrenceId'] = 'RECURRENCE-ID;' . $value;
	}

	/**
	 * ���������� ���������� ������������� �� ������ ����� �����, � �������  ����� �������
	 *
	 * @param string $hostname
	 */
	function setUID($hostname) {
		$this->eventProps['uid'] = 'UID:' . time() . '@' . $hostname;
	}
	
	/**
	 * ������������� ��������� �������, �� ��������� - ���������� ������
	 *
	 * @param string $attende
	 */
	function setAttendee($attende) {
		$this->eventProps['attendee'] = 'ATTENDEE;ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:' . $attende;
	}

	/**
	 * ������������� ����� - ������������� ������� � ���������� ���������
	 *
	 * @param string $class
	 */
	function setClass($class) {
		$validValues = array(
			'private',
			'public',
			'confidential'
		);
		if (in_array($class, $validValues)) {
			$this->eventProps['class'] = 'CLASS:' . strtoupper($class);
		}
	}

	/**
	 * ������������� ��������� �������
	 * ��������� ��������� �������� [0..9]
	 * 1 - ��������� ���������, 9 - ����������
	 * 0 - ��������� �������� (�� ����������)
	 *
	 * @param int $priority
	 */
	function setPriority($priority) {
		$priority = intval($priority);
		if ($priority >= 0 && $priority <= 9) {
			$this->eventProps['priority'] = 'PRIORITY:' . $priority;
		}
	}
	
	/**
	 * ������������� ������� ��������
	 *
	 * @param string $text
	 */
	function setSummary($text) {
		$this->eventProps['summary'] = 'SUMMARY:' . $text;
	}
	
	/**
	 * ������������� �������� �������
	 *
	 * @param string $text
	 */
	function setDescription($text) {
		$this->eventProps['description'] = 'DESCRIPTION:' . $text;
	}

	/**
	 * ������������� ���������� ����� ������� �������
	 *
	 * @param int $number
	 */
	function setSequence($number) {
		$this->eventProps['sequence'] = 'SEQUENCE:' . intval($number);
		
	}
	
 	/** ������������� ������������ �������:
  	 * ���������� �� ����������� ����� ��� ������� �� ������ �������
	 * 
	 *
	 * @param string $value
	 */
	function setTransp($value) {
		$validValues = array(
			'opaque',           // ���������� ����������� ����� ��� ������� �� �������
			'transparent'       // ��� ������������� ����������� ����� ��� ������� �� �������
		);
		if (in_array($value, $validValues)) {
			$this->eventProps['transp'] = 'TRANSP:' . strtoupper($value);
		}
	}

	/**
	 * ������������� ����� ������� (�����)
	 *
	 * @param string $location
	 */
	function setLocation($location) {
		$this->eventProps['location'] = 'LOCATION:' . $location;
	}
	
	/**
	 * ������������� ����/����� �������� ������� ��������� � �������
	 *
	 * @param timestamp $date
	 */
	function setDTStamp($date) { 
		$this->eventProps['DTStamp'] = 'DTSTAMP:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ����/����� ������ �������. ��� c������ � ������� refresh - �� ������������
	 *
	 * @param timestamp $date
	 */
	function setDTStart($date) {
		$this->eventProps['DTStart'] = 'DTSTART:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ����/����� ��������� �������. ��� c������ � ������� refresh - �� ������������
	 *
	 * @param timestamp $date
	 */
	function setDTEnd($date) {
		$this->eventProps['DTEnd'] = 'DTEND:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ������������ ������� - ������������ ��������, �� ��������� - �����������
	 *
	 * @param string $organizer
	 */
	function setOrganizer($organizer) {
		$this->eventProps['organizer'] = 'ORGANIZER:MAILTO:'.$organizer;
	}
	
	/**
	 * ������������� ���� �������� ������� 
	 *
	 * @param timestamp $date
	 */
	function setCreated($date) {
		$this->eventProps['created'] = 'CREATED:' . $this->formatDT($date);
	}
	
	/**
	 * ������ �������������� ���������, ����������
	 *
	 * @param float $lat - ������
	 * @param float $lon - �������
	 */
	function setGeo($lat, $lon) {
		$this->eventProps['geo'] = 'GEO:'.$lat.';'.$lon;
	}
	
	/**
	 * ������������� ������ ������� 
	 *
	 * @param string $status
	 */
	function setStatus($status) {
		$validValues = array(
			'tentative',        // �������
			'confirmed',        // ������������ 
			'cancelled'         // ��������
		);
		if (in_array($status, $validValues)) {
			$this->eventProps['status'] = 'STATUS:'.strtoupper($status);
		}
	}
}

?>