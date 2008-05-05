<?php

class PXModuleObjects extends PXAbstractModule {
	var $formats;

	function PXModuleObjects($area, $settings) {
		$this->PXAbstractModule($area, $settings);
		$this->formats = array();
		if (isset($settings['format'])) {
			if (is_array($settings['format'])) {
				$this->formats = $settings['format'];
			} else {
				$this->formats[] = $settings['format'];
			}
		}
	}


	function adminIndex($app, $request, $user, $db, $layout) {
		$types = array();
		foreach ($app->types as $k=>$v) {
			if (count($this->formats) == 0 || in_array($k, $this->formats)) {
				$types[$k] = $v->title;
			}
		}

		uasort($types, array($this, '__sortTypes'));

		$layout->AssignKeyValueList('INNER.0.0', $types, $request->GetSid());

		if ($request->GetSid() != NULL) {
			$format = $request->GetSid();
			if (!isset($app->types[$format])) {
				return;
			}

			$onPage      = $app->GetProperty('CHILDREN_ON_PAGE', 20);
			$currentPage = $request->GetVar($format.'_page', 1);
			$layout->SetGetVarToSave($format.'_page', $currentPage);

			if ($app->types[$format]->struct == 'tree' && $request->GetVar($format.'_view') != 'plain') {
				$objects = $db->GetObjects($app->types[$format], NULL, DB_SELECT_TREE);
				$layout->AppendTableAsTree('INNER.1.0', $format, $objects, $request->GetCid(), $request->GetLeafStatus());

			} else {
				$db->LoadDirectoriesByType($app->types[$format]);
				$count   = $db->GetObjects($app->types[$format], NULL, DB_SELECT_COUNT);
				$currentPage  = $request->GetVar($format.'_page', 1);
				$currentOrder = $request->GetVar($format.'_order', $app->types[$format]->order);
				$layout->SetGetVarToSave($format.'_order', $currentOrder);

				$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

				$objects = $db->GetObjectsLimited($app->types[$format], NULL, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

				$table = new PXAdminTable($objects, $app->types[$format], $layout->getData);
				$table->setPager($currentPage, $onPage, $count);
				$layout->append('INNER.1.0', $table->getTable());
			}

			$layout->AssignControls('INNER.1.1', 0, array($format));
			$layout->AssignContextMenu('INNER.1.0.CONTEXT', 0, array($format));
		}
	}
	function adminPopup(&$app, &$request, &$user, &$db, &$layout) {
        

		if (!isset($app->types[$request->GetFormat()])) {
			FatalError();
		}

        $requestedFormat = $request->GetFormat();
        $requestedType   = $app->types[$requestedFormat];
		
        $layout->SetGetVarToSave('id',     $request->GetId());
		$layout->SetGetVarToSave('format', $requestedFormat);
		$layout->SetGetVarToSave('action', $request->GetAction());
		$layout->SetOuterForm('action.phtml', 'POST', 'multipart/form-data');

		if ($request->GetId() == 0) {
			$object    = $app->InitContentObject($requestedFormat);
			$reqObject = $request->GetContentObject($requestedType);

			foreach($reqObject as $k=>$v) {
				if(array_key_exists($k, $object) && is_null($object[$k])) {
					$object[$k] = $v;
				}
			}
		} else {
			$object = $db->GetObjectByID($requestedType, $request->GetId());
		}

		if ($user->isAdmin()) {
			if(isset($object['sys_owner'])) {
				$tmp = $db->GetObjectByID($app->types['suser'], $object['sys_owner']);
				$object['ownerlogin'] = $tmp['title'];
			}

			$form      = new PXAdminObjectForm($app->types[$request->GetFormat()], $object);

			// $access    = $form->CheckAccess($request->GetAction(), $user->level);
			$menuItems = $form->GetAllowedForms($user->level);
			$layout->SetMenu($menuItems, $request->GetAction(), 'action');
			if (!$user->can('write', $requestedType, $object)) $form->disable = true;
			$layout->Assign('OUTER.LEFTCONTROLS', $form->LeftControls());
			$layout->Assign('OUTER.RIGHTCONTROLS', $form->RightControls());
			$db->LoadDirectoriesByType($requestedType, $object);

			switch ($request->GetAction()) {
				case 'main':
					$id  = getFromArray($object, 'id');
					$pid = getFromArray($object, 'pid');

					if (($parentFormat = $requestedType->parent) && strstr($requestedType->fields['parent']->displayType, "DROPDOWN")) {
						$tree = $db->GetObjects($app->types[$parentFormat], NULL, DB_SELECT_TREE);
					} else {
						$tree = new NLTree(array());
					}

					if ($requestedType->struct == 'tree') {
						if (isset($requestedType->fields['parent']) && !is_null($object['parent'])) {
							$selfTree = $db->GetObjectsByParent($requestedType, NULL, $object['parent'], DB_SELECT_TREE);
						} else {
							$selfTree = $db->GetObjects($requestedType, NULL, DB_SELECT_TREE);
						}
					} else {
						$selfTree = new NLTree(array());
					}
					$layout->Append('OUTER.CONTENT', $form->MainForm($tree->GetPlainTree($id), $selfTree->GetPlainTree($id)));
					break;
				case 'children':
					$counts = array();
					$possibleFormats = $app->filterTypesByBase($app->GetNextBase($requestedType->base));
					foreach ($app->getAllowedChildsKeys($requestedFormat, $object) as $allowedFormat) {
						$counts[$allowedFormat] = $db->GetObjectsByParent($app->types[$allowedFormat], NULL, $object['id'], DB_SELECT_COUNT);
					}
					$layout->Append('OUTER.CONTENT', $form->ChildrenForm($possibleFormats, $counts));
					break;
				case 'access':
					$roles = GetColFromTableWithIndexs($app->types['suser']->fields['access']->values->values, 'title');
					foreach($roles as $k=>$v) {
						if ($k > $user->level) {
							unset($roles[$k]);
						}
					}

					unset($roles[0]);

					$layout->Append('OUTER.CONTENT', $form->AccessForm($roles));
					break;
				case 'remove':
					$layout->Append('OUTER.CONTENT', $form->RemoveForm());
					break;
				case 'links':
					foreach ($requestedType->references as $k=>$v) {
						$existingLinks = $db->GetLinks($v, $requestedFormat, $object['id']);
						$fullList      = true;

						$onPage      = $app->GetProperty('LINKS_ON_PAGE', 10);

						$currentPage = $request->GetVar($k.'_page', 1);
						$layout->SetGetVarToSave($k.'_page', $currentPage);

						$onlyExistingLinks = (int)$request->GetVar($k.'_exist', false);
						$layout->SetGetVarToSave($k.'_exist', $onlyExistingLinks);


						$where = NULL;

						if(!empty($v->filterFrom) && $v->from == $k) {
							$where = $db->ParseWhereTemplate($v->filterFrom, $object, $requestedType);
						}

						if(!empty($v->filterTo) && $v->to == $k) {
							$where = $db->ParseWhereTemplate($v->filterTo, $object, $requestedType);

						}

						if (!empty($v->restrictBy)) {
							if(strlen($where)) {
								$where .= ' AND ';
							}

							$where .= $k . '.' . $v->restrictBy." = '".$db->EscapeString($object[$v->restrictBy])."'";
						}

						if($onlyExistingLinks) {
							$count = 0;
							$possibleLinksKeys = array();
							foreach($existingLinks as $kk => $vv) {
								if($count >= $onPage * ($currentPage - 1) && $count < $onPage * $currentPage) {
									$possibleLinksKeys[$kk] = $kk;
								}
								$count++;
							}

							$possibleLinks = $db->GetObjectsByIdArray($app->types[$k], NULL, $possibleLinksKeys);
							$count         = $db->GetObjectsByWhere($app->types[$k], NULL, $where, DB_SELECT_COUNT);

						} else {
                            $possibleLinks = $db->GetObjectsByWhereLimited($app->types[$k], NULL, $where, 
                                                                           $onPage, $onPage*($currentPage-1));
							$count         = $db->GetObjectsByWhere($app->types[$k], NULL, $where, DB_SELECT_COUNT);

							if ($requestedFormat == $k) {
								unset($possibleLinks[$object['id']]);
							}
						}

						$formatTo = $v->getOther($requestedFormat);
                        $layout->Append('OUTER.CONTENT', 
                                        $form->LinksForm($v, 
                                                         $app->types[$formatTo],
                                                         $existingLinks, 
                                                         $possibleLinks, 
                                                         $onPage,
                                                         $currentPage, 
                                                         $count, 
                                                         $layout,
                                                         $onlyExistingLinks)); // fixme: 9 params!!!
					}
					break;
			}
		} else {
			$layout->AssignError('OUTER.CONTENT', 'Нет доступа');
		}

		if ($request->GetId() == 0) {
			$title = 'Добавление объекта формата &laquo;'.strip_tags($requestedType->title).'&raquo;';
		} else {
			$title = isset($object) && isset($object['title']) ? '&laquo;' . substr(strip_tags($object['title']), 0, 32) . '&raquo; &#8212; ' : ' ';
            $title.= 'Редактирование объекта формата &laquo;'.strip_tags($requestedType->title).'&raquo;';
		}

		$layout->AssignTitle($title);
	}

	function adminAction(&$app, &$request, &$user, &$db) { /*{{{*/
		if (!isset($app->types[$request->GetFormat()])) {
			FatalError();
		}
		$base = isset($app->types[$request->GetFormat()]->base) ? $app->types[$request->GetFormat()]->base : NULL;
		switch ($request->GetAction()) {
			case 'main':
				$object = $request->GetContentObject($app->types[$request->GetFormat()]);
				if ($object['id']) {
					$db->ModifyContentObject($app->types[$request->GetFormat()], $object, true); // Preserve values with displaytype HIDDEN or STATIC
					$nextLocation = "popup.phtml?area=objects&format=".$request->GetFormat()."&id={$object['id']}&action=main";
				} else {
					$id = $db->AddContentObject($app->types[$request->GetFormat()], $object);
					$nextLocation = "popup.phtml?area=objects&format=".$request->GetFormat()."&id={$id}&action=main";
				}
				break;
			case 'children':
				if (!count($app->types[$request->GetFormat()]->childs)) {
					FatalError("В этом разделе невозможно назначать потомков");
				}
				$object = $request->GetObjectSysVars($app->types[$request->GetFormat()], array(OBJ_FIELD_CHILDREN));
				if ($object['id']) {
					$db->ModifyObjectSysVars($app->types[$request->GetFormat()], $object);
					$nextLocation = "popup.phtml?area=objects&format=".$request->GetFormat()."&id={$object['id']}&action=children";
				}
				break;
			case 'access':
				$object = $request->GetObjectSysVars($app->types[$request->GetFormat()]);
				if ($object['id']) {
					$db->ModifyObjectSysVars($app->types[$request->GetFormat()], $object);
					$nextLocation = "popup.phtml?area=objects&format=".$request->GetFormat()."&id={$object['id']}&action=access";
				}
				break;
			case 'remove':
				if ($request->isSetVar('ack')) {
					$db->DeleteContentObject($app->types[$request->GetFormat()], $request->GetID());
				}
				CloseAndRefresh();
				break;
			case 'directup':
				$db->UpContentObject($app->types[$request->GetFormat()], $request->GetID());
				$nextLocation = $request->GetReferer();
				break;
			case 'directdown':
				$db->DownContentObject($app->types[$request->GetFormat()], $request->GetID());
				$nextLocation = $request->GetReferer();
				break;
			case 'directmove':
				$db->MoveContentObject($app->types[$request->GetFormat()], $request->GetID(), $request->GetVar('shift'));
				$nextLocation = $request->GetReferer();
				break;
			case 'directremove':
				$db->DeleteContentObject($app->types[$request->GetFormat()], $request->GetID());
				$nextLocation = $request->GetReferer();
				break;
			case 'directstatus':
				$object = $db->GetObjectById($app->types[$request->GetFormat()], $request->GetId());
				$object['status'] = ($object['status'] == true) ? false : true;
				$db->ModifyContentObject($app->types[$request->GetFormat()], $object);
				$nextLocation = $request->GetReferer();
				break;
			case 'links':
				foreach ($app->types[$request->GetFormat()]->references as $k=>$v) {
					$db->ModifyLinks($v, $request->GetFormat(), $request->GetId(), $request->GetLinks($v), false);
				}
				$nextLocation = "popup.phtml?area=objects&format=".$request->GetFormat()."&id=".$request->GetId()."&action=links";
				break;
			default:
				FatalError("Неизвестный системе <EM>action</EM>");
				break;
		}
		return $nextLocation;
	} /*}}}*/
	
	private function __sortTypes($a, $b) {
		return strcoll($a, $b);
	}

}

?>
