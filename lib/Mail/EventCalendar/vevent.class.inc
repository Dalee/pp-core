<?php

class NLMailVEvent
{

	public $_calScale;        //тип календаря, если не установлен, по умолчанию GREGORIAN
	public $_prodId;           // почтовый клиент - обязательное свойство
	public $_method;

	public $_requiredProps;   // массив обязательных полей объекта vEvent конкретного типа
	public $_uselessProps;    // массив неиспользуемых полей объекта vEvent конкретного типа
	public $eventProps;       // все поля объекта vEvent

	public function __construct()
	{
		$this->_calScale = null;                    //тип календаря, если не установлен, по умолчанию GREGORIAN
		$this->_prodId = '-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN'; // почтовый клиент - обязательное свойство

		$this->eventProps = [];
		$this->setDTStamp(time());
		$this->setSequence(0);
		$this->setSummary('');
	}

	public function getCalendarContent()
	{
		$eventContent = $this->_createEvent();

		$content = "BEGIN:VCALENDAR\n";
		if ($this->_calScale) $content .= $this->_calScale . "\n";
		$content .= "PRODID:" . $this->_prodId . "\n";
		$content .= "VERSION:2.0\n";
		$content .= "METHOD:" . $this->_method . "\n";
		if ($eventContent) $content .= $eventContent . "\n";
		$content .= "END:VCALENDAR\n";

		return $content;
	}

	public function _createEvent()
	{
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
		return "BEGIN:VEVENT\n" . implode("\n", $this->eventProps) . "\nEND:VEVENT";
	}

	/**
	 * Проверка, инициированы ли все необходимые свойства для данного типа события
	 */
	public function _checkRequired()
	{
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
	public function _checkUseless()
	{
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
	public function setCalScale($scale)
	{
		$this->_calScale = 'CALSCALE:' . $scale;
	}

	/**
	 * Идентификатор почтового клиента
	 */
	public function setProdId($prodId)
	{
		$this->_prodId = $prodId;
	}

	/**
	 * Форматирует время в стандарте vCalendar
	 *
	 * @param timestamp $date
	 */
	public function formatDT($date)
	{
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
	public function setAlarm($trigger, $description = 'Reminder', $duration = null, $repeat = null)
	{
		$alarm = "BEGIN:VALARM\n";
		$alarm .= "TRIGGER:" . $trigger . "\n";
		$alarm .= "ACTION:DISPLAY\n";
		$alarm .= "DESCRIPTION:" . $description . "\n";
		if ($duration) $alarm .= 'DURATION:' . $duration . "\n";
		if ($repeat) $alarm .= 'REPEAT:' . $repeat . "\n";  // число повторений, используется вместе с DURATION
		$alarm .= "END:VALARM";

		$this->eventProps['alarm'] = $alarm;
	}

	/**
	 * Устанавливает важность события - не стандартное свойство планировщика outlook
	 *
	 * @param int $value
	 */
	public function setXImportance($value)
	{
		$this->eventProps['XImportance'] = 'X-MICROSOFT-CDO-IMPORTANCE:' . $value;
	}

	/**
	 * Определяет категории события
	 * например BUSINESS,HUMAN RESOURCES,ANNIVERSARY,PERSONAL,SPECIAL OCCASION и т.п - не обязательный
	 *
	 * @param string $text
	 */
	public function setCategories($text)
	{
		$this->eventProps['categories'] = 'CATEGORIES:' . $text;
	}

	/**
	 * Определяет связь с другим компонентом vCalendar по UID
	 *
	 * @param string $rId
	 */
	public function setRelated($rId)
	{
		$this->eventProps['related'] = 'RELATED-TO:<' . $rId . '>';
	}

	/**
	 * Определяет статус-код, который вернул запрос выполнения расписания.
	 * Например: REQUEST-STATUS:2.0;Success
	 *
	 * @param string $status
	 */
	public function setRequestStatus($status)
	{
		$this->eventProps['requestStatus'] = 'REQUEST-STATUS:' . $status;
	}

	/**
	 * Задает url места нахождения сети
	 * например:  TZURL:http://timezones.r.us.net/tz/US-California-Los_Angeles. Не обязательное
	 *
	 * @param string $url
	 */
	public function setTZurl($url)
	{
		$this->eventProps['url'] = 'TZURL:' . $url;
	}

	/**
	 * Определяет правила, например повторений события
	 *
	 * @param string $rule
	 */
	public function setRecurrenceRule($rule)
	{
		$this->eventProps['recurrenceRule'] = 'RRULE:' . $rule;
	}

	/**
	 * Устанавливает даты, например повторений события
	 *
	 * @param string $date дата + параметры
	 */
	public function setRecurrenceDate($date)
	{
		$this->eventProps['recurrenceDate'] = 'RDATE:' . $date;
	}

	/**
	 * Определяет правила, например, повторение не больше 10 раз в день и т.п
	 *
	 * @param string $rule
	 */
	public function setsetExeptionRule($rule)
	{
		$this->eventProps['exceptionRule'] = 'EXRULE:' . $rule;
	}

	/**
	 * Устанавливает даты-исключения для события
	 *
	 * @param array of timestamp $datesArray
	 */
	public function setExeptionDate($datesArray)
	{
		$array = array_map($this->formatDT(...), $datesArray);
		$this->eventProps['exeptionDate'] = 'EXDATE:' . implode(",", $array);
	}

	/**
	 * Устанавливает дополнительную или альтернативную контактную информацию
	 *
	 * @param string $text
	 */
	public function setContact($text)
	{
		$this->eventProps['contact'] = 'CONTACT:' . $text;
	}

	/**
	 * Устанавливает комментарий к событию
	 *
	 * @param string $text
	 */
	public function setComment($text)
	{
		$this->eventProps['comment'] = 'COMMENT:' . $text;
	}

	/**
	 * Устанавливает привязанный документ к объекту vCalendar
	 *
	 * @param string $value uri + параметры
	 */
	public function setAttach($value)
	{
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
	public function setDuration($weeks, $days, $hours, $minutes, $seconds)
	{
		$duration = 'P';
		if ($weeks) $duration .= $weeks . 'W';
		if ($days) $duration .= $days . 'D';

		if ($hours || $minutes || $seconds) $duration .= 'T';

		if ($hours) $duration .= $hours . 'H';
		if ($minutes) $duration .= $minutes . 'M';
		if ($seconds) $duration .= $seconds . 'S';

		$this->eventProps['duration'] = 'DURATION:' . $duration;
	}

	public function setRecurrenceId($value)
	{
		$this->eventProps['recurrenceId'] = 'RECURRENCE-ID;' . $value;
	}

	/**
	 * Генерирует уникальный идентификатор на основе имени хоста, и текущей  метки времени
	 *
	 * @param string $hostname
	 */
	public function setUID($hostname)
	{
		$this->eventProps['uid'] = 'UID:' . time() . '@' . $hostname;
	}

	/**
	 * Устанавливает участника события, по умолчанию - получатель письма
	 *
	 * @param string $attende
	 */
	public function setAttendee($attende)
	{
		$this->eventProps['attendee'] = 'ATTENDEE;ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:' . $attende;
	}

	/**
	 * Устанавливает класс - классификация доступа к компоненту календарь
	 *
	 * @param string $class
	 */
	public function setClass($class)
	{
		$validValues = [
			'private',
			'public',
			'confidential',
		];
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
	public function setPriority($priority)
	{
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
	public function setSummary($text)
	{
		$this->eventProps['summary'] = 'SUMMARY:' . $text;
	}

	/**
	 * Устанавливает описание события
	 *
	 * @param string $text
	 */
	public function setDescription($text)
	{
		$this->eventProps['description'] = 'DESCRIPTION:' . $text;
	}

	/**
	 * Устанавливает порядковый номер ревизии события
	 *
	 * @param int $number
	 */
	public function setSequence($number)
	{
		$this->eventProps['sequence'] = 'SEQUENCE:' . intval($number);

	}

	/** Устанавливает прозрачность события:
	 * необходимо ли затрачивать время или ресурсы на данное событие
	 *
	 *
	 * @param string $value
	 */
	public function setTransp($value)
	{
		$validValues = [
			'opaque',           // необходимо затрачивать время или ресурсы на событие
			'transparent'       // нет необходимости затрачивать время или ресурсы на событие
		];
		if (in_array($value, $validValues)) {
			$this->eventProps['transp'] = 'TRANSP:' . strtoupper($value);
		}
	}

	/**
	 * Устанавливает место события (адрес)
	 *
	 * @param string $location
	 */
	public function setLocation($location)
	{
		$this->eventProps['location'] = 'LOCATION:' . $location;
	}

	/**
	 * Устанавливает дату/время создания данного сообщения о событии
	 *
	 * @param timestamp $date
	 */
	public function setDTStamp($date)
	{
		$this->eventProps['DTStamp'] = 'DTSTAMP:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает дату/время начала события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	public function setDTStart($date)
	{
		$this->eventProps['DTStart'] = 'DTSTART:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает дату/время окончания события. Для cобытия с методом refresh - не используется
	 *
	 * @param timestamp $date
	 */
	public function setDTEnd($date)
	{
		$this->eventProps['DTEnd'] = 'DTEND:' . $this->formatDT($date);
	}

	/**
	 * Устанавливает организатора события - обязательное свойство, по умолчанию - отправитель
	 *
	 * @param string $organizer
	 */
	public function setOrganizer($organizer)
	{
		$this->eventProps['organizer'] = 'ORGANIZER:MAILTO:' . $organizer;
	}

	/**
	 * Устанавливает дату создания события
	 *
	 * @param timestamp $date
	 */
	public function setCreated($date)
	{
		$this->eventProps['created'] = 'CREATED:' . $this->formatDT($date);
	}

	/**
	 * Задает географическое положение, координаты
	 *
	 * @param float $lat - широта
	 * @param float $lon - долгота
	 */
	public function setGeo($lat, $lon)
	{
		$this->eventProps['geo'] = 'GEO:' . $lat . ';' . $lon;
	}

	/**
	 * Устанавливает статус события
	 *
	 * @param string $status
	 */
	public function setStatus($status)
	{
		$validValues = [
			'tentative',        // пробное
			'confirmed',        // подтверждено
			'cancelled'         // отменено
		];
		if (in_array($status, $validValues)) {
			$this->eventProps['status'] = 'STATUS:' . strtoupper($status);
		}
	}
}
