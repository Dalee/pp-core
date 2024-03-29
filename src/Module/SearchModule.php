<?php

namespace PP\Module;

/**
 * Class SearchModule.
 *
 * @package PP\Module
 */
class SearchModule extends AbstractModule
{
    public $word;

    public function adminIndex()
    {
        $this->word = trim((string) $this->request->GetVar('word'));
        $this->searchForm();

        if (mb_strlen($this->word)) {
            $allCount = 0;

            $checkedTypes = $this->request->getVar('d', []);
            foreach ($this->app->types as $datatype) {
                if (!isset($checkedTypes[$datatype->id])) {
                    continue;
                }

                $allCount += $this->find($datatype);
            }

            if (!$allCount) {
                $this->layout->append('INNER.1.0', '<h2 class="error">Ничего не найдено</h2>');
            }
        }
    }

    public function __sortTypes($a, $b)
    {
        return strcmp((string) $a->title, (string) $b->title);
    }

    public function searchForm()
    {
        $datatypesHTML = '<ul>';

        $types = $this->app->types;
        uasort($types, $this->__sortTypes(...));

        $checkedTypes = $this->request->getVar('d', []);

        foreach ($types as $datatype) {
            $checked = !sizeof($checkedTypes) || isset($checkedTypes[$datatype->id]) ? 'checked' : '';
            $datatypesHTML .= <<<HTML

			<li>
				<input type="checkbox" name="d[{$datatype->id}]" id="d_{$datatype->id}" {$checked}>
				<label for="d_{$datatype->id}">{$datatype->title}</label>
			</li>
HTML;
        }

        $datatypesHTML .= '</ul>';

        $hword = quot($this->word);
        $html = <<<HTML
		<style type="text/css">
			fieldset li, fieldset ul {
				padding: 0;
				margin: 0;
				list-style: none;
			}

			fieldset {
				margin: 1em 0;
			}
		</style>

		<form>
			<h2>Поиск</h2>

			<input type="hidden" name="area"      value="{$this->area}">
			<input type="text" style="width:80%;" name="word" value="{$hword}">

			<input type="submit" value="Найти">

			<fieldset>
				<legend>искать по типам данных</legend>
				{$datatypesHTML}

				<button onclick="var inps = document.getElementsByTagName('fieldset')[0].getElementsByTagName('input'); for(i in inps) { inps[i].checked = !inps[i].checked; }; return false;">поменять выделение</button>
			</fieldset>
		</form>
HTML;

        $this->layout->assign('INNER.0.0', $html);
        $this->layout->setGetVarToSave('word', $this->word);

        // build the string for datatypes check
        foreach ($checkedTypes as $k => $v) {
            $this->layout->setGetVarToSave('d[' . $k . ']', 'on');
        }
    }

    public function find(&$datatype)
    {
        $count = $this->db->getObjectsBySearchWord($datatype, null, $this->word, DB_SELECT_COUNT);

        if ($count) {
            $table = new \PXAdminTableObjects($datatype, $this->word, 'BySearchWord');
            $table->addToParent('INNER.1.0');
        }

        return $count;
    }

}
