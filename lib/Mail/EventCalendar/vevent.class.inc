<?php
class NLMailVEvent {

	var $_calScale;        //тип календаря, если не установлен, по умолчанию GREGORIAN
	var $_prodId;		   // почтовый клиент - обязательное свойство
	var $_method;

	var $_requiredProps;   // массив обязательных полей объекта vEvent конкретного типа
	var $_uselessProps;    // массив неиспользуемых полей объекта vEvent конкретного типа
	var $eventProps;       // все поля объекта vEvent

	function NLMailVEvent() {
		$this->_calScale = null;                    //тип календаря, если не установлен, по умолчанию GREGORIAN
		$this->_prodId   = '-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN'; // почтовый клиент - обязательное свойство

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

		return $content;
	}

	function _createEvent() {
		$this->setAttendee($this->toEmail);
		$this->setOrganizer($this->fromEmail);

		$required = $this->_checkRequired();
		if ($required) {
			$this->setError('Вы не инициализировали свойство ' . $required . ',требуемое для данного типа события');
			return false;
		}

		$useless = $this->_checkUseless();
		if ($useless) {
			$this->setError('Cвойство ' . $useless . 'не должно быть инициировано для данного типа события');
			return false;
		}
		$content = "BEGIN:VEVENT\n" . implode("\n", $this->eventProps) . "\nEND:VEVENT";
		return $content;
	}

	/**
	 * Проверка, инициированы ли все необходимые свойства для данного типа события
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
	 * Проверка на отсутствие запрещенных свойств для данного типа события
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
	 * Устанавливает тип календаря, если не указан, по умолчанию GREGORIAN
	 */
	function setCalScale($scale) {
		$this->_calScale = 'CALSCALE:' . $scale;
	}

	/**
	 * Идентификатор почтового клиента
	 */
	function setProdId($prodId) {
		$this->_prodId = $prodId;
	}

	/**
	 * Форматирует время в стандарте vCalendar
	 *
	 * @param timestamp $date
	 */
	function formatDT($date) {
		return date('Ymd', $date) . 'T' . date('His', $date) . 'Z';
	}

	/**
	 * Устанавливает напоминания
	 *
	 * @param string $trigger
	 * @param string $description описание
	 * @param string $duration интервал времени для напоминаний
	 * @param string $repeat
	 */
	function setAlarm($trigger, $description = 'Reminder', $duration = null, $repeat = null) {
		$alarm  = "BEGIN:VALARM\n";
		$alarm .= "TRIGGER:" . $trigger . "\n";
		$alarm .= "ACTION:DISPLAY\n";
		$alarm .= "DESCRIPTION:" . $description . "\n";
		if ($duration) $alarm .= 'DURATION:' . $duration . "\n";
		if ($repeat)   $alarm .= 'REPEAT:'   . $repeat   . "\n";  // число повторений, используется вместе с DURATION
		$alarm .= "END:VALARM";

		$this->eventProps['alarm'] = $alarm;
	}

	/**
	 * Устанавливает важность события - не стандартное свойство планировщика outlook
	 *
	 * @param int $value
	 */
	function setXImportance($value) {
		$this->eventProps['XImportance'] = 'X-MICROSOFT-CDO-IMPORTANCE:' . $value;
	}

	/**
	 * Определяет категории события
	 * например BUSINESS,HUMAN RESOURCES,ANNIVERSARY,PERSONAL,SPECIAL OCCASION и т.п - не обязательный
	 *
	 * @param string $text
	 */
	function setCategories($text) {
		$this->eventProps['categories'] = 'CATEGORIES:' . $rId;
	}

	/**
	 * Определяет связь с другим компонентом vCalendar по UID
	 *
	 * @param string $rId
	 */
	function setRelated($rId) {
		$this->eventProps['related'] = 'RELATED-TO:<' . $rId . '>';
	}

	/**
	 * Определяет статус-код, который вернул запрос выполнения расписания.
	 * Например: REQUEST-STATUS:2.0;Success
	 *
	 * @param string $status
	 */
	function setRequestStatus($status) {
		$this->eventProps['requestStatus'] = 'REQUEST-STATUS:' . $status;
	}

	/**
	 * Задает url места нахождения сети
	 * например:  TZURL:http://timezones.r.us.net/tz/US-California-Los_Angeles. Не обязательное
	 *
	 * @param string $url
	 */
	function setTZurl($url) {
		$this->eventProps['url'] = 'TZURL:' . $url;
	}

	/**
	 * Определяет правила, например повторений события
	 *
	 * @param string $rule
	 */
	function setRecurrenceRule($rule) {
		$this->eventProps['recurrenceRule'] = 'RRULE:' . $rule;
	}

	/**
	 * Устанавливает даты, например повторений события
	 *
	 * @param string $date дата + параметры
	 */
	function setRecurrenceDate($date) {
		$this->eventProps['recurrenceDate'] = 'RDATE:' . $date;
	}

	/**
	 * Определяет правила, например, повторение не больше 10 раз в день и т.п
	 *
	 * @param string $rule
	 */
	function setsetExeptionRule($rule) {
		$this->eventProps['exceptionRule'] = 'EXRULE:' . $rule;
	}

	/**
	 * Устанавливает даты-исключения для события
	 *
	 * @param array of timestamp $datesArray
	 */
	function setExeptionDate($datesArray) {
		$array = array_map(array($this, 'formatDT'), $datesArray);
		$this->eventProps['exeptionDate'] = 'EXDATE:' . implode(",", $array);
	}

	/**
	 * Устанавливает дополнительную или альтернативную контактную информацию
	 *
	 * @param string $text
	 */
	function setContact($text) {
		$this->eventProps['contact'] = 'CONTACT:' . $text;
	}

	/**
	 * Устанавливает комментарий к событию
	 *
	 * @param string $text
	 */
	function setComment($text) {
		$this->eventProps['comment'] = 'COMMENT:' . $text;
	}

	/**
	 * Устанавливает привязанный документ к объекту vCalendar
	 *
	 * @param string $value uri + параметры
	 */
	function setAttach($value) {
		$this->eventProps['attach'] = 'ATTACH:' . $value;
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
	 * Генерирует уникальный идентификатор на основе имени хоста, и текущей  метки времени
	 *
	 * @param string $hostname
	 */
	function setUID($hostname) {
		$this->eventProps['uid'] = 'UID:' . time() . '@' . $hostname;
	}

	/**
	 * Устанавливает участника события, по умолчанию - получатель письма
	 *
	 * @param string $attende
	 */
	function setAttendee($attende) {
		$this->eventProps['attendee'] = 'ATTENDEE;ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:' . $attende;
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
			$this->eventProps['class'] = 'CLASS:' . strtoupper($class);
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
			$this->eventProps['priority'] = 'PRIORITY:' . $priority;
		}
	}

	/**
	 * Устанавливает краткое описание
	 *
	 * @param string $text
	 */
	function setSummary($text) {
		$this->eventProps['summary'] = 'SUMMARY:' . $text;
	}

	/**
	 * Устанавливает описание события
	 *
	 * @param string $text
	 */
	function setDescription($text) {
		$this->eventProps['description'] = 'DESCRIPTION:' . $text;
	}

	/**
	 * Устанавливает порядковый номер ревизии события
	 *
	 * @param int $number
	 */
	function setSequence($number) {
		$this->eventProps['sequence'] = 'SEQUENCE:' . intval($number);

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
			$this->eventProps['transp'] = 'TRANSP:' . strtoupper($value);
		}
	}

	/**
	 * Устанавливает место события (адрес)
	 *
	 * @param string $location
	 */
	function setLocation($location) {
		$this->eventProps['location'] = 'LOCATION:' . $location;
	}

	/**
	 * Устанавливает дату/время создания данного сообщения о событии
	 *
	 * @param timestamp $date
	 */
	function setDTStamp($date) {
		$this->eventProps['DTStamp'] = 'DTSTAMP:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает дату/время начала события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	function setDTStart($date) {
		$this->eventProps['DTStart'] = 'DTSTART:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает дату/время окончания события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	function setDTEnd($date) {
		$this->eventProps['DTEnd'] = 'DTEND:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает организатора события - обязательное свойство, по умолчанию - отправитель
	 *
	 * @param string $organizer
	 */
	function setOrganizer($organizer) {
		$this->eventProps['organizer'] = 'ORGANIZER:MAILTO:'.$organizer;
	}

	/**
	 * Устанавливает дату создания события
	 *
	 * @param timestamp $date
	 */
	function setCreated($date) {
		$this->eventProps['created'] = 'CREATED:' . $this->formatDT($date);
	}

	/**
	 * Задает географическое положение, координаты
	 *
	 * @param float $lat - широта
	 * @param float $lon - долгота
	 */
	function setGeo($lat, $lon) {
		$this->eventProps['geo'] = 'GEO:'.$lat.';'.$lon;
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
			$this->eventProps['status'] = 'STATUS:'.strtoupper($status);
		}
	}
}

?>
