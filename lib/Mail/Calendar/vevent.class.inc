<?php
class NLMailVEvent extends NLMailVCalendar {
	
	var $_created;        // ����/����� �������� ���������� � ������� �� ����������� (�� ��������������� ��� ������� � ������� refresh)
	var $_attendee;       // �������� �������, �� ���������� - ����������
	var $_organizer;      // ����������� - ������������ ��������, �� ��������� - �����������
	var $_DTStart;        // ����/����� ������ ������� - �� ������������
	var $_DTEnd;          // ����/����� ��������� ������� - �� ������������
	var $_DTStamp;	      // ����/����� �������� ������ ������� ������� - ������������, �� ��������� ������� �����
	var $_location;       // ����� ���������� ������� - �� �����������
	var $_transp;         // �������� ����������, ���������� �� ����������� ����� ��� ������� �� ������ ������� - �� �����������
	var $_sequence;       // ���������� ����� ������� �������, ������� � 0
	var $_uid;            // ���������� ������������� ��� ����� �������, �� ���������� ��� ������ ������� - ������������ 
	var $_description;    // �������� ������� - �� ������������ ��������, ����� ���� null
	var $_summary;        // ������� �������� - ������������ ��������
	var $_priority;	      // ��������� �������. �������� �� 0 �� 9. 1 - ��������� ��������, 9 - ����������, 0 - �� ����������. �� ������������ ��������
	var $_class;	      // ����� - ��� ������������� ������� � ���������� ���������
	var $_geo;            // �������������� ��������� �� ������������ ��������
	var $_lastmod;        // ����/����� ��������� ����������� - �� ������������
	var $_status;         // ������ - �� �����������, ����� ��������� ��������: ������� ������������, ��������, ������� ������
	var $_url;            // �������� ������ url ����� ���������� ����, ��������:  TZURL:http://timezones.r.us.net/tz/US-California-Los_Angeles. �� ������������
	var $_recurrenceId;   // ������ ����� ���������� ������� � ������ uid � ���������� ������� -  sequence, �� �����������
	var $_duration;       // ������������ �������, ����� �������������� ������ DTEND
	var $_attach;         // ����������� ������� �������� (����������� ��� uri) � �������� vCalendar
	var $_comment;        // ����������� - �� ������������
	var $_contact;        // �������������� �������� ��� ����� - �� ������������
	var $_recurrenceDate; // ���������� ����, ����� ������� ������ ����������� - �� ������������
	var $_recurrenceRule; // ���������� ������� ����� ������� ������ ����������� - �� ������������
	var $_exeptionDate;   // ���������� ����, ����� ������� �� ����� ����������� - �� ������������
	var $_exeptioRule;    // ���������� �������-����������, ��������, ���������� �� ������ 10 ��� � ���� ��� � �.�
	var $_requestStatus;  // ���������� ������-���, ������� ������ ������ ���������� ���������� - �� �����������
	var $_related;        // ����������� ����� � ������ ����������� vCalendar �� UID - �� ������������
	var $_categories;     // ��������� �������, �������� BUSINESS,HUMAN RESOURCES,ANNIVERSARY,PERSONAL,SPECIAL OCCASION � �.� - �� ������������
	var $_XImportance;    // �� ����������� �������� ������������ outlook - ��������, �� �����������
	
	var $requiredMethods;
	
	function NLMailVEvent() {
		$this->NLMailVCalendar();
		
		$this->setAttendee($this->toEmail);
		$this->setOrganizer($this->fromEmail);
		$this->setDTStamp(time());
		$this->setSequence(0);
		$this->setSummary('');
		
	}
	
	function createEvent() {
		$required = $this->checkRequired();
		if ($required) {
			$this->setError('�� �� ���������������� �������� ' . $required . ',��������� ��� ������� ���� �������');
			$this->_event = false;
		}
		
		$content = "BEGIN:VEVENT\n";
		
		if (isset($this->_created))        $content .= $this->_created        . "\n";
		if (isset($this->_attendee))       $content .= $this->_attendee       . "\n";
		if (isset($this->_organizer))      $content .= $this->_organizer      . "\n";
		if (isset($this->_DTStart))        $content .= $this->_DTStart        . "\n";
		if (isset($this->_DTEnd))          $content .= $this->_DTEnd          . "\n";
		if (isset($this->_DTStamp))        $content .= $this->_DTStamp        . "\n";
		if (isset($this->_location))       $content .= $this->_location       . "\n";
		if (isset($this->_transp))         $content .= $this->_transp         . "\n";
		if (isset($this->_sequence))       $content .= $this->_sequence       . "\n";
		if (isset($this->_uid))            $content .= $this->_uid            . "\n";
		if (isset($this->_description))    $content .= $this->_description    . "\n";
		if (isset($this->_summary))        $content .= $this->_summary        . "\n";
		if (isset($this->_priority))       $content .= $this->_priority       . "\n";
		if (isset($this->_XImportance))    $content .= $this->_XImportance    . "\n";
		if (isset($this->_class))          $content .= $this->_class          . "\n";
		if (isset($this->_geo))            $content .= $this->_geo            . "\n";
		if (isset($this->_lastmod))        $content .= $this->_lastmod        . "\n";
		if (isset($this->_status))         $content .= $this->_status         . "\n";
		if (isset($this->_url))            $content .= $this->_url            . "\n";
		if (isset($this->_recurrenceId))   $content .= $this->_recurrenceId   . "\n";
		if (isset($this->_duration))       $content .= $this->_duration       . "\n";
		if (isset($this->_attach))         $content .= $this->_attach         . "\n";
		if (isset($this->_comment))        $content .= $this->_comment        . "\n";
		if (isset($this->_contact))        $content .= $this->_contact        . "\n";
		if (isset($this->_recurrenceDate)) $content .= $this->_recurrenceDate . "\n";
		if (isset($this->_recurrenceRule)) $content .= $this->_recurrenceRule . "\n";
		if (isset($this->_exeptionDate))   $content .= $this->_exeptionDate   . "\n";
		if (isset($this->_exeptioRule))    $content .= $this->_exeptioRule    . "\n";
		if (isset($this->_requestStatus))  $content .= $this->_requestStatus  . "\n";
		if (isset($this->_related))        $content .= $this->_related        . "\n";
		if (isset($this->_categories))     $content .= $this->_categories     . "\n";
		
		$content .= "END:VEVENT";
		
		$this->_event = $content;
	}
	
	function checkRequired() {
		$propsArray = get_class_vars(__CLASS__);
		foreach ($this->requiredMethods as $propname) {
			if ($propsArray[$propname] == NULL) {
				return $propname;
			}
		}
		return false;
	}
	
	/**
	 * ������������� �������� �������
	 *
	 * @param int $value
	 */
	function setXImportance($value) {
		$this->_XImportance = 'X-MICROSOFT-CDO-IMPORTANCE:' . $value;
	}
	
	/**
	 * ���������� ��������� �������
	 *
	 * @param string $text
	 */
	function setCategories($text) {
		$this->_categories = 'CATEGORIES:' . $rId;
	}
	
	/**
	 * ���������� ����� � ������ ����������� vCalendar �� UID
	 *
	 * @param string $rId
	 */
	function setRelated($rId) {
		$this->_related = 'RELATED-TO:<' . $rId . '>';
	}

	/**
	 * ���������� ������-���, ������� ������ ������ ���������� ����������.
	 * ��������: REQUEST-STATUS:2.0;Success
	 *
	 * @param string $status
	 */
	function setRequestStatus($status) {
		$this->_requestStatus = 'REQUEST-STATUS:' . $status;
	}

	function setTZurl($url) {
		$this->_url = 'TZURL:' . $url;
	}

	/**
	 * ���������� �������, �������� ���������� �������
	 *
	 * @param string $rule
	 */
	function setRecurrenceRule($rule) {
		$this->_recurrenceRule = 'RRULE:' . $rule;
	}

	/**
	 * ������������� ����, �������� ���������� �������
	 *
	 * @param string $date ���� + ���������
	 */
	function setRecurrenceDate($date) {
		$this->_recurrenceDate = 'RDATE:' . $date;
	}

	/**
	 * ���������� �������, ��������, ���������� �� ������ 10 ��� � ���� ��� � �.�
	 *
	 * @param string $rule
	 */
	function setsetExeptionRule($rule) {
		$this->_exceptionRule = 'EXRULE:' . $rule;
	}

	/**
	 * ������������� ���� - ���������� ��� �������
	 *
	 * @param array of timestamp $datesArray
	 */
	function setExeptionDate($datesArray) {
		$array = array_map(array($this, 'formatDT'), $datesArray);
		$this->_exeptionDate = 'EXDATE:' . implode(",", $array);
	}

	/**
	 * ������������� ���������� ���������� 
	 *
	 * @param string $text
	 */
	function setContact($text) {
		$this->_contact = 'CONTACT:' . $text;
	}
	
	function setComment($text) {
		$this->_comment = 'COMMENT:' . $text;
	}
	
	/**
	 * ������������� ����������� �������� � ������� vCalendar
	 *
	 * @param string $value uri + ���������
	 */
	function setAttach($value) {
		$this->_attach = 'ATTACH' . $value;
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
		$this->_duration = 'P';
		if ($weeks) $this->_duration .= $weeks . 'W';
		if ($days)  $this->_duration .= $days  . 'D';
		
		if ($hours || $minutes || $seconds) $this->_duration .= 'T';
		
		if ($hours)   $this->_duration .= $hours   . 'H';
		if ($minutes) $this->_duration .= $minutes . 'M';
		if ($seconds) $this->_duration .= $seconds . 'S';
	}
	
	function setRecurrenceId($value) {
		$this->_recurrenceId = 'RECURRENCE-ID;' . $value;
	}

	/**
	 * ���������� ���������� ������������� �� ������ ����� �����, � �������  ����� �������
	 *
	 * @param string $hostname
	 */
	function setUID($hostname) {
		$this->_uid = 'UID:' . time() . '@' . $hostname;
	}
	
	/**
	 * ������������� ��������� �������, �� ��������� - ���������� ������
	 *
	 * @param string $attende
	 */
	function setAttendee($attende) {
		$attendee = 'ATTENDEE;ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:' . $attende;
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
			$this->_class = strtoupper($class);
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
			$this->_priority = 'PRIORITY:' . $priority;
		}
	}
	
	/**
	 * ������������� ������� ��������
	 *
	 * @param string $text
	 */
	function setSummary($text) {
		$this->_summary = 'SUMMARY:' . $text;
	}
	
	/**
	 * ������������� �������� �������
	 *
	 * @param string $text
	 */
	function setDescription($text) {
		$this->_description = 'DESCRIPTION:' . $text;
	}

	/**
	 * ������������� ���������� ����� ������� �������
	 *
	 * @param int $number
	 */
	function setSequence($number) {
		$this->_sequence = 'SEQUENCE:' . intval($number);
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
			$this->_transp = 'TRANSP:' . strtoupper($value);
		}
	}

	/**
	 * ������������� ����� �������
	 *
	 * @param string $location
	 */
	function setLocation($location) {
		$this->_location = 'LOCATION:' . $location;
	}
	
	/**
	 * ������������� ����/����� �������� ������� ��������� � �������
	 *
	 * @param timestamp $date
	 */
	function setDTStamp($date) { 
		$this->_DTStamp = 'DTSTAMP:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ����/����� ������ �������. ��� c������ � ������� refresh - �� ������������
	 *
	 * @param timestamp $date
	 */
	function setDTStart($date) {
		$this->_DTStart = 'DTSTART:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ����/����� ��������� �������. ��� c������ � ������� refresh - �� ������������
	 *
	 * @param timestamp $date
	 */
	function setDTEnd($date) {
		$this->_DTEnd = 'DTEND:' . $this->formatDT($date);
	}
	
	/**
	 * ������������� ������������ ������� - ������������ ��������, �� ��������� - �����������
	 *
	 * @param string $organizer
	 */
	function setOrganizer($organizer) {
		$this->_organizer = 'ORGANIZER:MAILTO:'.$organizer;
	}
	
	/**
	 * ������������� ���� �������� ������� 
	 *
	 * @param timestamp $date
	 */
	function setCreated($date) {
		$this->_created = $this->formatDT($date);
	}
	
	/**
	 * ������ �������������� ��������� 
	 *
	 * @param float $lat - ������
	 * @param float $lon - �������
	 */
	function setGeo($lat, $lon) {
		$this->_geo = 'GEO:'.$lat.';'.$lon;
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
			$this->_transp = 'STATUS:'.strtoupper($status);
		}
	}
}

?>