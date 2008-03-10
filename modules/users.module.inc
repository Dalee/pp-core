<?php

class PXModuleUsers extends PXAbstractModule {

	function PXModuleUsers($area, $settings) {
		$this->PXAbstractModule($area, $settings);
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$roles = GetColFromTableWithIndexs($app->types['suser']->fields['access']->values->values, 'title');
		foreach($roles as $k=>$v) {
			if ($k > $user->level) {
				unset($roles[$k]);
			}
		}

		unset($roles[0]);
		$layout->AssignKeyValueList('INNER.0.0', $roles, $request->GetSid());
		if ($request->GetSid() && in_array($request->GetSid(), array_keys($roles))) {
			$onPage    = $app->GetProperty('CHILDREN_ON_PAGE', 20);

			$currentPage  = $request->GetVar('suser_page', 1);
			$currentOrder = $request->GetVar('suser_order', $app->types['suser']->order);
			$layout->SetGetVarToSave('suser_order', $currentOrder);
			$layout->SetGetVarToSave('suser_page', $request->GetVar('suser_page', 1));
			$layout->SetGetVarToSave('suser_view', $request->GetVar('suser_view', 'plain'));

			$count   = $db->GetObjectsByField($app->types['suser'], NULL, 'access', $request->GetSid(), DB_SELECT_COUNT);

			$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

			$objects = $db->GetObjectsByFieldLimited($app->types['suser'], NULL, 'access', $request->GetSid(), $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

			$table = new PXAdminTable($objects, $app->types['suser'], $layout->getData);
			$table->setPager($currentPage, $onPage, $count);
			$table->setSelected($request->GetCid());
			$table->setShowChildObjects(true);
			$layout->append('INNER.1.0', $table->getTable());

			$layout->AssignControls('INNER.1.1', (int)$request->GetSid(), array('suser'));
			$layout->AssignContextMenu('INNER.1.0.CONTEXT', $request->GetSid(), array('suser'));

			/*
			$onPage    = $app->GetProperty('CHILDREN_ON_PAGE', 20);
			$currentPage = $request->GetVar('suser_page', 1);
			$objects     = $db->GetObjectsByFieldLimited($app->types['suser'], NULL, 'access', $request->GetSid(), $onPage, $onPage*($currentPage-1));
			$count       = $db->GetObjectsByField($app->types['suser'], NULL, 'access', $request->GetSid(), DB_SELECT_COUNT);

			$layout->AppendTable('INNER.1.0', 'suser', $objects, $request->GetCid(), 'title', $request->GetVar('suser_page'), $app->GetProperty('CHILDREN_ON_PAGE', 5), $count);
			$layout->AssignControls('INNER.1.1', (int)$request->GetSid(), array('suser'));
			$layout->AssignContextMenu('INNER.1.0.CONTEXT', $request->GetSid(), array('suser'));
			*/

			if ($request->GetCid()) {
				$layout->SetGetVarToSave('cid', $request->GetCid());
				$layout->SetSimpleInnerLayout(array('25%', '40%', '35%'), array('100%', ''));
				if (isset($objects[$request->GetCid()])) {
					$cidObject = $objects[$request->GetCid()];
					$allowedChilds = $app->GetAllowedChildsKeys('suser', $cidObject);
					if (count($allowedChilds)) {
						foreach ($allowedChilds as $childFormat) {
							$objects = $db->GetObjectsByField($app->types[$childFormat], NULL, 'parent', $request->GetCid());
							$layout->AppendTable('INNER.2.0', $app->types[$childFormat]->id, $objects);
						}
						if ($user->level >= $cidObject['sys_accessput']) {
							$layout->AssignContextMenu('INNER.2.0.CONTEXT', $request->GetCid(), $app->types['suser']->childs);
							$layout->AssignControls('INNER.2.1', $request->GetCid(), $app->types['suser']->childs);
						}
					} else {
						$layout->Assign('INNER.2.0', '<H2>Для добавления в раздел информации необходимо указать разрешенные форматы</H2>
							<UL><LI><A href="javascript:EditContent(\'suser\', \''.$request->GetCid().'\', \'children\')">Разрешенные форматы раздела</A></LI></UL>
						');
					}
				} else {
					$layout->AssignError('INNER.2.0', 'Нет такого пользователя');
				}
			}
		} elseif ($request->GetSid()) {
			$layout->AssignError('INNER.1.0', 'Нет доступа');
		} else {
			$layout->Assign('INNER.1.0', '<H2>Выберите категорию пользователей</H2>');
		}
	}

}

?>
