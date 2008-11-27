<?
class PXModuleAuth extends PXAbstractModule {
	function adminIndex() {
		$this->layout->setLoginForm(
			'action.phtml',
			'POST',

			array(
				'login'   => 'login',
				'passwd'  => 'passwd',
				'referer' => 'referer',
				'area'    => 'area'
			),

			array(
				'login'   => $this->user->login,
				'passwd'  => NULL,
				'referer' => $this->request->getReferer(),
				'area'    => 'auth'
			)
		);

		if ($this->user->IsAuthed()) {
			$this->layout->assign('LOGIN.ERROR', '<strong class="error">Нет доступа.</strong>');
			$this->user->unAuth();

		} else if ($this->request->GetAction() == 'error') {
			$this->layout->assign('LOGIN.ERROR', '<strong class="error">Введенная комбинация логина/пароля неверна.</strong> ');
		}
	}

	function adminAction() {
		if ($this->_auth() == 0) {
			$nextLocation = $this->request->GetReferer();

		} else {
			$nextLocation = $this->request->GetReferer();
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

	function userAction() {
		if ($this->_auth() == 0) {
			$nextLocation = ($this->request->GetVar('onsuccess')) ? $this->request->GetVar('onsuccess') : $this->request->GetReferer();
			$nextLocation = removeParamFromUrl($nextLocation, 'login');
		} else {
			$nextLocation = ($this->request->GetVar('onerror')) ? $this->request->GetVar('onerror') : $this->request->GetReferer();
			$nextLocation = appendParamToUrl($nextLocation, 'login', 'bad');
		}
		return $nextLocation;
	}

	function _auth() {
		$audit = PXAuditLogger::getLogger($this->app, $this->db);

		switch ($this->request->getVar('action')) {
			case 'exit':
				$audit->info('Выход из системы', 'suser'.'/'. $this->user->id);
				$this->user->UnAuth();
				return 0;
				break;

			default:
				if ($this->user->IsAuthed()) {
					$audit->info('Вход в систему', 'suser'.'/'. $this->user->id);
					$this->user->Auth();
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
