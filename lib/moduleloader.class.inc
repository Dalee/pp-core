<?
class PXModuleLoader {
	function ActivateModules($methodType, &$app, &$db, &$request, &$user, &$layout, &$container) {
		$modules = array();

		switch($methodType) {
			case 'userindex':
				$modules[] = array('type' => 'main', 'module' => 'main');

				foreach($app->modBindings as $m) {
					if($m['type'] == 'main') {
						array_shift($modules);
					}
					$modules[] = $m;
				}

				if(sizeof($request->GetHostAndDir()) <= 1) {
					$modules[] = array('type' => 'request', 'module' => $request->GetVar('area'));
				}
				break;

			default:
				$modules[] = array('type' => 'request', 'module' => $request->GetVar('area'));
				break;
		}

		uasort($modules, 'SortModulesCalls');

		foreach ($modules as $k=>$m) {
			switch($methodType) {
				case 'userindex':
					switch($m['type']) {
						case 'types':
							$format = $m['value'];
							if(!isset($app->types[$format])) {
								continue(3);
							}

							switch($app->types[$format]->base) {
								case 'struct':
									if($container['currentSid'] == -1) {
										continue(4);
									}
									break;

								case 'content':
									if($container['currentSid'] == -1) {
										continue(4);
									}

									if(!isset($container['objects'][$format]) || !sizeof($container['objects'][$format])) {
										continue(4);
									}
									break;

								case 'subcontent':
									if($container['currentCid'] == -1) {
										continue(4);
									}

									if(!isset($container['subObjects'][$format]) || !sizeof($container['subObjects'][$format])) {
										continue(4);
									}
									break;
							}
							break;

						case 'struct':
							if($container['currentSid'] == -1) {
								continue(3);
							}

							if(!$this->IsBindingsModule($container['tree']->leafs[$container['currentSid']]->content, $m['var'], $m['value'])) {
								continue(3);
							}
							break;

						case 'content':
							if($container['currentCid'] == -1) {
								continue(3);
							}

							if(!$this->IsBindingsModule($container['objects'][$container['currentCtype']][$container['currentCid']], $m['var'], $m['value'])) {
								continue(3);
							}
							break;

						case 'subcontent':
							if($container['currentSCid'] == -1) {
								continue(3);
							}

							if(!$this->IsBindingsModule($container['subObjects'][$container['currentSCtype']][$container['currentSCid']], $m['var'], $m['value'])) {
								continue(3);
							}
							break;

						case 'request':
						default:
							break;
					}
					break;

				case 'useraction':
				case 'userjson':
					if($m['type'] != 'request') {
						continue(2);
					}

					if(!strlen($m['module'])) {
//						FatalError('Попытка обращения к безымянному модулю');
						Finalize('/');
					}
					break;
			}

			if(!isset($app->modules[$m['module']])) {
				continue;
			}

			$moduleDescription = $app->modules[$m['module']];
			$module = $moduleDescription->getModule();

			$debugKey = 'MODULE: '.$moduleDescription->class;
			NLProfiler($debugKey, 'BEGIN');

			switch($methodType) {
				case 'userindex':
					$module->userIndex(
						$app,
						$request,
						$user,
						$db,

						$layout,

						$container['tree'],
						$container['objects'],
						$container['subObjects'],
						$container['references'],
						$container['heap'],
						$container['currentSid'],
						$container['currentCid'],
						$container['currentCtype'],
						$container['currentSCid'],
						$container['currentSCtype'],
						$container['rootId'],
						$container['pathId'],

						$container['urlFile'],
						$container['error']
					);
					break;

				case 'useraction':
					return $module->userAction(
						$app,
						$request,
						$user,
						$db
					);

				case 'userjson':
					return $module->userJson(
						$app,
						$request,
						$user,
						$db
					);
			}

			NLProfiler($debugKey, 'END');
		}
	}

	function IsBindingsModule($object, $varName, $value) {
		if(is_object($object)) {
			return (isset($object->{$varName}) && ($object->{$varName} == $value));
		}

		if(is_array($object)) {
			return (isset($object[$varName]) && ($object[$varName] == $value));
		}

		return false;
	}
}

function SortModulesCalls($a, $b) {
	if(!isset($a['order'])) {
		$a['order'] = 0;
	}

	if(!isset($b['order'])) {
		$b['order'] = 0;
	}

	if(!isset($a['pOrder'])) {
		$a['pOrder'] = 0;
	}

	if(!isset($b['pOrder'])) {
		$b['pOrder'] = 0;
	}

	if($a['order'] > $b['order']) {
		return  1;
	}

	if($a['order'] < $b['order']) {
		return -1;
	}

	if($a['pOrder'] > $b['pOrder']) {
		return  1;
	}

	if($a['pOrder'] < $b['pOrder']) {
		return -1;
	}

	if($a['type'] == 'main') {
		return -1;
	}

	if($b['type'] == 'main') {
		return 1;
	}

	return 0;
}

?>
