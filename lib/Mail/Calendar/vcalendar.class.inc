<?php
class NLMailVCalendar extends NLMailMessage {
	
	var $_calScale;
	var $_prodId;
	var $_version;
	var $_method;
	var $_event;
	var $_alarm;
	var $_attendee;
	
	function NLMailVCalendar() {
		$this->NLMailMessage();
		
		$this->_calScale = null;                    //��� ���������, ���� �� ����������, �� ��������� GREGORIAN
		$this->_prodId   = '-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN'; // �������� ������ - ������������ ��������
		$this->_alarm    = null;                    // ����������� ��� ������� - �� �����������
		$this->_event    = null; 
	}
	
	function createCalendar() {
		$content = "BEGIN:VCALENDAR\n";
		if ($this->_calScale) $content .= $this->_calScale . "\n";
		$content .= "PRODID:" .$this->_prodId . "\n";
		$content .= "VERSION:2.0\n";
		$content .= "METHOD:" .$this->_method . "\n";
		if ($this->_event) $content .= $this->_event . "\n";
		$content .= "END:VCALENDAR\n";
		
		return $content;
	}
	
	// ������������� ��������� �������
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
	
	function createSmtpMail() {
		if (!$this->replyEmail) $this->replyEmail = $this->fromEmail;

		$this->_strip();

				$this->addHeaders = $this->_writeSmtpHeaders();

		$contentType = 'text/calendar';

		$this->addHeaders .= "Content-Type: ".$contentType."; method=".$this->_method."; charset=".$this->charset."\n";
		$this->addHeaders .= "Content-Transfer-Encoding: 8bit";
		
		$this->body = $this->createCalendar();

		$_subject = $this->_getSubject();

		return $_subject;
	}
}
?>