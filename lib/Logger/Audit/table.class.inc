<?php

class PXAdminAuditTable extends PXAdminTableSimple
{

    public function __construct($cols)
    {
        parent::__construct($cols);
        $this->app = PXRegistry::getApp();
        $this->setDict('datatypes', $this->app->types);
    }

    public function _getDatatypeTitle($format)
    {
        return isset($this->dict['datatypes'][$format]) ? $this->dict['datatypes'][$format]->title : $format;
    }

    /**
     * @throws JsonException
     */
    public function cell($row, $col)
    {
        if ($col == 'source') {
            if (mb_strpos((string) $row[$col], '/')) {
                preg_match('|(\w+)/?(\d+)?|' . REGEX_MOD, (string) $row[$col], $matches);

                if (sizeof($matches) == 3) {
                    $row[$col] = $this->__link('popup.phtml?area=objects&format=' . $matches[1] . '&id=' . $matches[2] . '&action=main', $this->_getDatatypeTitle($matches[1]) . '/' . $matches[2]);

                } elseif (sizeof($matches) == 2) {
                    $row[$col] = $this->__link('?area=dicts&sid=' . $matches[1], $this->_getDatatypeTitle($matches[1]));
                }

            } elseif (mb_strlen((string) $row[$col])) {
                $row[$col] = $this->__link('?area=dicts&sid=' . $row[$col], $this->_getDatatypeTitle($row[$col]));
            }
        }

        if ($col == 'user' && $row[$col] !== 'UNKNOWN') {
            $row[$col] = $this->__link('./?area=auditlog&user=' . $row[$col], $row[$col]);
        }

        if ($col == 'diff') {
            if (!($row[$col] ?? false)) {
                $row[$col] = '[]';
            }
            $diff = (array)json_decode((string) $row[$col], true, 512, JSON_THROW_ON_ERROR);
            $diff = array_filter($diff);

            $source = explode('/', (string) $row['source'])[0];

            $typeFields = $this->app->types[$source]->fields ?? [];
            $changedOutput = 'Ничего не изменилось';
            $humanTitles = [];

            foreach ($diff as $code) {
                isset($typeFields[$code]->description) && $humanTitles[] = $typeFields[$code]->description;
            }

            if (!empty($humanTitles)) {
                $changedOutput = implode(', ', $humanTitles);
            }

            $row[$col] = $changedOutput;
        }

        return parent::cell($row, $col);
    }

    public function __link($url, $text)
    {
        return '<a href="' . $url . '" target="_blank">' . $text . '</a>';
    }
}
