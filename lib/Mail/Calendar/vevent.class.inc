<?php
class NLMailVEvent extends NLMailVCalendar {
	
	var $_created;        // дата/время создания информации о событии не обязательно (Не устанавливается для события с методом refresh)
	var $_attendee;       // участник события, по умолчанияю - получатель
	var $_organizer;      // организатор - обязательное свойство, по умолчанию - отправитель
	var $_DTStart;        // дата/время начала события - не обязательное
	var $_DTEnd;          // дата/время окончания события - не обязательное
	var $_DTStamp;	      // дата/время создания данной ревизии события - обязательное, по умолчанию текущее время
	var $_location;       // место проведения события - не обязательно
	var $_transp;         // свойство определяет, необходимо ли затрачивать время или ресурсы на данное событие - не обязательно
	var $_sequence;       // порядковый номер ревизии события, начиная с 0
	var $_uid;            // уникальный идентификатор для всего события, не изменяется для разных ревизий - обязательное 
	var $_description;    // Описание события - не обязательное свойство, может быть null
	var $_summary;        // Краткое описание - обязательное свойство
	var $_priority;	      // Приоритет события. Значения от 0 до 9. 1 - наивысший проритет, 9 - наименьший, 0 - не установлен. Не обязательное свойство
	var $_class;	      // класс - для классификации доступа к компоненту календарь
	var $_geo;            // географическое положение не обязательное свойство
	var $_lastmod;        // дата/время последней модификации - не обязательное
	var $_status;         // статус - не обязательно, может принимать значения: событие подтверждено, отменено, пробный запрос
	var $_url;            // свойство задает url места нахождения сети, например:  TZURL:http://timezones.r.us.net/tz/US-California-Los_Angeles. Не обязательное
	var $_recurrenceId;   // задает время повторения события с данным uid и порядковым номером -  sequence, не обязательно
	var $_duration;       // длительность события, может использоваться вместо DTEND
	var $_attach;         // возможность связать документ (указывается его uri) с объектом vCalendar
	var $_comment;        // комментарий - не обязательное
	var $_contact;        // альтернативные контакты для связи - не обязательное
	var $_recurrenceDate; // определяет даты, когда событие должно происходить - не обязательное
	var $_recurrenceRule; // определяет правила когда событие должно происходить - не обязательное
	var $_exeptionDate;   // определяет даты, когда событие не может происходить - не обязательное
	var $_exeptioRule;    // определяет правила-исключения, например, повторение не больше 10 раз в день для и т.п
	var $_requestStatus;  // определяет статус-код, который вернул запрос выполнения расписания - не обязательно
	var $_related;        // определяеть связь с другим компонентом vCalendar по UID - не обязательное
	var $_categories;     // категории события, например BUSINESS,HUMAN RESOURCES,ANNIVERSARY,PERSONAL,SPECIAL OCCASION и т.п - не обязательный
	var $_XImportance;    // не стандартное свойство планировщика outlook - важность, не обязательно
	
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
			$this->setError('Вы не инициализировали свойство ' . $required . ',требуемое для данного типа события');
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
	 * Устанавливает важность события
	 *
	 * @param int $value
	 */
	function setXImportance($value) {
		$this->_XImportance = 'X-MICROSOFT-CDO-IMPORTANCE:' . $value;
	}
	
	/**
	 * Определяет категории события
	 *
	 * @param string $text
	 */
	function setCategories($text) {
		$this->_categories = 'CATEGORIES:' . $rId;
	}
	
	/**
	 * Определяет связь с другим компонентом vCalendar по UID
	 *
	 * @param string $rId
	 */
	function setRelated($rId) {
		$this->_related = 'RELATED-TO:<' . $rId . '>';
	}

	/**
	 * Определяет статус-код, который вернул запрос выполнения расписания.
	 * Например: REQUEST-STATUS:2.0;Success
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
	 * Определяет правила, например повторений события
	 *
	 * @param string $rule
	 */
	function setRecurrenceRule($rule) {
		$this->_recurrenceRule = 'RRULE:' . $rule;
	}

	/**
	 * Устанавливает даты, например повторений события
	 *
	 * @param string $date дата + параметры
	 */
	function setRecurrenceDate($date) {
		$this->_recurrenceDate = 'RDATE:' . $date;
	}

	/**
	 * Определяет правила, например, повторение не больше 10 раз в день для и т.п
	 *
	 * @param string $rule
	 */
	function setsetExeptionRule($rule) {
		$this->_exceptionRule = 'EXRULE:' . $rule;
	}

	/**
	 * Устанавливает даты - исключения для события
	 *
	 * @param array of timestamp $datesArray
	 */
	function setExeptionDate($datesArray) {
		$array = array_map(array($this, 'formatDT'), $datesArray);
		$this->_exeptionDate = 'EXDATE:' . implode(",", $array);
	}

	/**
	 * Устанавливает контактную информацию 
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
	 * Устанавливает привязанный документ к объекту vCalendar
	 *
	 * @param string $value uri + параметры
	 */
	function setAttach($value) {
		$this->_attach = 'ATTACH' . $value;
	}

	/**
	 * устанавливает длительность события, может использоваться вместо DTEND
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
	 * Генерирует уникальный идентификатор на основе имени хоста, и текущей  метки времени
	 *
	 * @param string $hostname
	 */
	function setUID($hostname) {
		$this->_uid = 'UID:' . time() . '@' . $hostname;
	}
	
	/**
	 * Устанавливает участника события, по умолчанию - получатель письма
	 *
	 * @param string $attende
	 */
	function setAttendee($attende) {
		$attendee = 'ATTENDEE;ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:' . $attende;
	}

	/**
	 * Устанавливает класс - классификация доступа к компоненту календарь
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
	 * Устанавливает приоритет события
	 * приоритет принимает значения [0..9]
	 * 1 - наивысший проиритет, 9 - наименьший
	 * 0 - дефолтное значение (не установлен)
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
	 * Устанавливает краткое описание
	 *
	 * @param string $text
	 */
	function setSummary($text) {
		$this->_summary = 'SUMMARY:' . $text;
	}
	
	/**
	 * Устанавливает описание события
	 *
	 * @param string $text
	 */
	function setDescription($text) {
		$this->_description = 'DESCRIPTION:' . $text;
	}

	/**
	 * Устанавливает порядковый номер ревизии события
	 *
	 * @param int $number
	 */
	function setSequence($number) {
		$this->_sequence = 'SEQUENCE:' . intval($number);
	}
	
 	/** Устанавливает прозрачность события:
  	 * необходимо ли затрачивать время или ресурсы на данное событие
	 * 
	 *
	 * @param string $value
	 */
	function setTransp($value) {
		$validValues = array(
			'opaque',           // необходимо затрачивать время или ресурсы на событие
			'transparent'       // нет необходимости затрачивать время или ресурсы на событие
		);
		if (in_array($value, $validValues)) {
			$this->_transp = 'TRANSP:' . strtoupper($value);
		}
	}

	/**
	 * Устанавливает место события
	 *
	 * @param string $location
	 */
	function setLocation($location) {
		$this->_location = 'LOCATION:' . $location;
	}
	
	/**
	 * Устанавливает дату/время создания данного сообщения о событии
	 *
	 * @param timestamp $date
	 */
	function setDTStamp($date) { 
		$this->_DTStamp = 'DTSTAMP:' . $this->formatDT($date);
	}
	
	/**
	 * Устанавливает дату/время начала события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	function setDTStart($date) {
		$this->_DTStart = 'DTSTART:' . $this->formatDT($date);
	}
	
	/**
	 * Устанавливает дату/время окончания события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	function setDTEnd($date) {
		$this->_DTEnd = 'DTEND:' . $this->formatDT($date);
	}
	
	/**
	 * Устанавливает организатора события - обязательное свойство, по умолчанию - отправитель
	 *
	 * @param string $organizer
	 */
	function setOrganizer($organizer) {
		$this->_organizer = 'ORGANIZER:MAILTO:'.$organizer;
	}
	
	/**
	 * Устанавливает дату создания события 
	 *
	 * @param timestamp $date
	 */
	function setCreated($date) {
		$this->_created = $this->formatDT($date);
	}
	
	/**
	 * Задает географическое положение 
	 *
	 * @param float $lat - широта
	 * @param float $lon - долгота
	 */
	function setGeo($lat, $lon) {
		$this->_geo = 'GEO:'.$lat.';'.$lon;
	}
	
	/**
	 * Устанавливает статус события 
	 *
	 * @param string $status
	 */
	function setStatus($status) {
		$validValues = array(
			'tentative',        // пробное
			'confirmed',        // подтверждено 
			'cancelled'         // отменено
		);
		if (in_array($status, $validValues)) {
			$this->_transp = 'STATUS:'.strtoupper($status);
		}
	}
}

?>