<?
class PXModuleAuth extends PXAbstractModule {
	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$layout->setLoginForm(
			'action.phtml',
			'POST',

			array(
				'login'   => 'login',
				'passwd'  => 'passwd',
				'referer' => 'referer',
				'area'    => 'area'
			),

			array(
				'login'   => $user->login,
				'passwd'  => NULL,
				'referer' => $request->GetReferer(),
				'area'    => 'auth'
			)
		);

		if ($user->IsAuthed()) {
			$layout->assign('LOGIN.ERROR', '<strong class="error">Нет доступа.</strong>');
			$user->unAuth();

		} else if ($request->GetAction() == 'error') {
			$layout->assign('LOGIN.ERROR', '<strong class="error">Введенная комбинация логина/пароля неверна.</strong> ');
		}
	}

	function adminAction(&$app, &$request, &$user, &$db) {
		if ($this->_auth($app, $request, $user, $db) == 0) {
			$nextLocation = $request->GetReferer();

		} else {
			$nextLocation = $request->GetReferer();
			if(strpos($nextLocation, '?') !== FALSE) {
				if(strpos($nextLocation, 'action') !== FALSE) {
					$nextLocation = preg_replace("/action=.*?\&?(.*)?$/", "action=error", $nextLocation);
				} else {
					$nextLocation .= '&action=error';
				}
			} else {
				$nextLocation .= '?action=error';
			}
		}
		return $nextLocation;
	}

	function userAction(&$app, &$request, &$user, &$db) {
		if ($this->_auth($app, $request, $user, $db) == 0) {
			$nextLocation = ($request->GetVar('onsuccess')) ? $request->GetVar('onsuccess') : $request->GetReferer();
			$nextLocation = removeParamFromUrl($nextLocation, 'login');
		} else {
			$nextLocation = ($request->GetVar('onerror')) ? $request->GetVar('onerror') : $request->GetReferer();
			$nextLocation = appendParamToUrl($nextLocation, 'login', 'bad');
		}
		return $nextLocation;
	}

	function _auth($app, $request, &$user, $db) {
		$audit = PXAuditLogger::getLogger($app, $db);

		switch ($request->GetVar('action')) {
			case 'exit':
				$audit->info('Выход из системы', 'suser'.'/'. $user->id);
				$user->UnAuth();
				return 0;
				break;

			default:
				if ($user->IsAuthed()) {
					$audit->info('Вход в систему', 'suser'.'/'. $user->id);
					$user->Auth();
					return 0;

				} else {
					$audit->error('Неудачный вход в систему', 'suser' .'/', 'ERROR');
					return 1;
				}
				break;
		}
	}

}
?>
