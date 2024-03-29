<?php
// Abstract
class PXInputDateTime extends PXAdminWidget
{
    public $date;   // array(year, month, day, hour, minute, second);

    public function __construct(public $name, $value, $notNull = true)
    {
        $this->notNull = $notNull;

        $this->date = (is_array($value)) ? $value : $this->_parseDate($value, $notNull);
    }

    public function _parseDate($value, $notNull)
    {
        $date = [
            'year' => null,
            'month' => null,
            'day' => null,

            'hour' => null,
            'minute' => null,
            'second' => null,
        ];

        if (!is_null($value)) {
            $matches = [];

            $value = trim((string) $value);
            $value = mb_substr($value, 0, 19);

            preg_match("/^((\d{1,2}).(\d{1,2}).(\d{4}))?\s?((\d{1,2})(:(\d{1,2})(:(\d{1,2}))?)?)?/" . REGEX_MOD, trim($value), $matches);

            if (isset($matches[1]) && mb_strlen($matches[1])) {
                $date['day'] = $matches[2];
                $date['month'] = $matches[3];
                $date['year'] = $matches[4];

            } else {
                $date['year'] = date('Y');
                $date['month'] = date('n');
                $date['day'] = date('j');
            }

            if (isset($matches[5])) {
                $date['hour'] = $matches[6];
                $date['minute'] = $matches[8];
                $date['second'] = $matches[10];

            } else {
                $date['hour'] = 0;
                $date['minute'] = 0;
                $date['second'] = 0;
            }

        } elseif ($notNull) {
            $date['year'] = date('Y');
            $date['month'] = date('n');
            $date['day'] = date('j');

            $date['hour'] = date('G');
            $date['minute'] = date('i') * 1;
            $date['second'] = date('s') * 1;
        }

        return $date;
    }

    public function html()
    {
        FatalError("It's abstract class, use other date or time inputs class");
    }
}

?>
