<?php

class PXModuleSearch extends PXAbstractModule {

	function PXModuleSearch($area, $settings) {
		$this->PXAbstractModule($area, $settings);
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$html = '<TABLE width="100%" border="0" cellpadding="3" cellspacing="1" style="border: 1px solid #C9CED6;">';
		$html .= '<FORM>';
		$html .= '<INPUT type="hidden" name="area" value="'.$request->GetArea('search').'">';
		$html .= '<INPUT type="hidden" name="charcheck" value="йцукен">';
		$html .= '<COLGROUP>
			<COL width="100%">
			<COL>
		</COLGROUP>';
		$html .= '<TR><TH style="text-align:left; background-color:#385A94; color:#FFFFFF;" colspan="2">Поиск</TH></TR>';
		$html .= '<TR style="background-color: #E7E9ED;"><TD><INPUT type="text" style="width:100%;" name="word" value="'.$request->GetVar('word').'"></TD>';
		$html .= '<TD align="right"><INPUT type="submit" value="Найти" class="button-light"></TD></TR>';
		$html .= '</FORM>';
		$html .= '</TABLE>';

		$layout->Assign('INNER.0.0', $html);

		if ($word = $request->GetVar('word')) {
			$layout->SetGetVarToSave('word', $request->GetVar('word'));
			$layout->SetGetVarToSave('charcheck', $request->GetVar('charcheck'));

			$cidType   = NULL;
			$cidObject = NULL;
			$onPage    = $app->GetProperty('CHILDREN_ON_PAGE', 5);

			$allCount = 0;

			foreach($app->types as $format=>$type) {
				$count   = $db->GetObjectsBySearchWord($app->types[$format], NULL, $word, DB_SELECT_COUNT);

				$allCount += $count;

				$currentPage  = $request->GetVar($format.'_page', 1);
				$currentOrder = $request->GetVar($format.'_order', $app->types[$format]->order);
				$layout->SetGetVarToSave($format.'_order', $currentOrder);

				$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

				if($count) {
					$objects = $db->GetObjectsBySearchWordLimited($app->types[$format], NULL, $word, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

					$table = new PXAdminTable($objects, $app->types[$format], $layout->getData);
					$table->setPager($currentPage, $onPage, $count);
					$layout->append('INNER.1.0', $table->getTable());
				}
			}

			if(!$allCount) {
				$layout->append('INNER.1.0', '<h2 class="error">Ничего не найдено</h2>');
			}
		}
	}

}
?>
