<?php
class PXModuleAuth extends PXModule {
	function adminIndex() {
		$user    = $this->user;
		$layout  = $this->layout;
		$request = $this->request;
		
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
				'referer' => $request->getReferer(),
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

	function adminAction() {
		$request = $this->request;

		if ($this->_auth() == 0) {
			$nextLocation = $request->GetReferer();

		} else {
			$nextLocation = $request->GetReferer();
			if(strpos($nextLocation, '?') !== FALSE) {
				if(strpos($nextLocation, '?action=') !== FALSE || strpos($nextLocation, '&action=') !== FALSE) {
					$nextLocation = preg_replace("/(?<=\&|\?)action=[^&]*/", "action=error", $nextLocation);
				} else {
					$nextLocation .= '&action=error';
				}

			} else {
				$nextLocation .= '?action=error';
			}
		}

		return $nextLocation;
	}

	function userAction() {
		$request = $this->request;

		if ($this->_auth() == 0) {
			$nextLocation = ($request->GetVar('onsuccess')) ? $request->GetVar('onsuccess') : $request->GetReferer();
			$nextLocation = removeParamFromUrl($nextLocation, 'login');
		} else {
			$nextLocation = ($request->GetVar('onerror')) ? $request->GetVar('onerror') : $request->GetReferer();
			$nextLocation = appendParamToUrl($nextLocation, 'login', 'bad');
		}
		return $nextLocation;
	}

	function _auth() {
		$app     = $this->app;
		$db      = $this->db;
		$user    = $this->user;
		$request = $this->request;
		$audit   = PXAuditLogger::getLogger($app, $db);

		switch ($request->getVar('action')) {
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