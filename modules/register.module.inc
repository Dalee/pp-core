<?php
class PXModuleRegister extends PXAbstractModule {
	var $systemName;
	var $systemEmail;
	var $defaultAccess;
	var $fields;
	var $successUrl;
	var $errorUrl;

	function PXModuleRegister($area, $settings) {
		$this->PXAbstractModule($area, $settings);
		$this->systemName    = $settings['systemname'];
		$this->systemEmail   = $settings['systememail'];
		$this->defaultAccess = $settings['defaultaccess'];
		$this->fields        = array();
		if (is_array($settings['fields'])) {
			foreach ($settings['fields'] as $f) {
				$this->fields[] = $this->_parseField($f);
			}
		} else {
			$this->fields[] = $this->_parseField($settings['fields']);
		}
	}

	function _parseField($string) {
		$tmp = explode('-', $string);
		$ret = array();
		if (isset($tmp[2])) $ret['max'] = $tmp[2];
		if (isset($tmp[1])) $ret['min'] = $tmp[1];
		$ret['title'] = $tmp[0];
		return $ret;
	}

	function userAction(&$app, &$request, &$user, &$db) {
		$tmp = $request->GetVar('onsuccess', $request->GetReferer());
		$tmp = removeParamFromUrl(removeParamFromUrl($tmp, 'result'),'error');
		$this->successUrl = str_replace("http://".$request->GetHttpHost(), '', $tmp);

		$tmp = $request->GetVar('onerror', $request->GetReferer());
		$tmp = removeParamFromUrl(removeParamFromUrl($tmp, 'result'),'error');
		$this->errorUrl = str_replace("http://".$request->GetHttpHost(), '', $tmp);

		$errors = array();
		$action = $request->GetVar('action','badaction');
		session_start();
		unset($_SESSION[$this->area . 'Data']);
		unset($_SESSION[$this->area . 'Errors']);
		switch ($action) {
			case 'create':
				$errors = $this->_createUser($app, $request, $user, $db);
				break;

			case 'confirm':
				$errors = $this->_confirmCreateUser($app, $request, $user, $db);
				break;

			case 'modify':
				$errors = $this->_modifyUser($app, $request, $user, $db);
				break;

			case 'auth':
			case 'exit':
				FatalError("Using of obsoleted function");
				break;

			default:
				$errors['badaction'] = true;
				break;
		}

		if (count($errors)) {
			$nextLocation = appendParamToUrl($this->errorUrl, 'result', "error_$action");
			$_SESSION[$this->area . 'Errors'] = $errors;
		} else {
			$nextLocation = appendParamToUrl($this->successUrl, 'result', "ok_$action");
		}

		return $nextLocation;
	}

	function _modifyUser(&$app, &$request, &$user, &$db) {
		$errors = array();
		// FIXME!!!!
		if ($request->GetVar('passwd') != $user->data['passwd']) {
			$errors['badpasswd'] = true;
		} else {
			if (!count($tmp = $db->GetObjectsByField($app->types['suser'], NULL, 'title', $user->login))) {
				$errors['baduser'] = true;
			} else {
				$newUser = current($tmp);
				$rqNewPasswd  = $request->GetVar('newpasswd');
				$rqNewPasswd2 = $request->GetVar('newpasswd2');
				$rqEmail      = $request->GetVar('email');
				if (!empty($rqNewPasswd)) {
					if (!isValidPasswd($rqNewPasswd, 3, 16)) {
						$errors['badnewpasswd'] = true;
					}
					if ($rqNewPasswd != $rqNewPasswd2) {
						$errors['notequalpasswd'] = true;
					}
				}
				if (!isValidEmail($rqEmail)) {
					$errors['bademail'] = true;
				}
				$_SESSION[$this->area . 'Data']['email'] = $rqEmail;
				$rq = array();
				foreach ($this->fields as $f) {
					$rq[$f['title']] = $request->GetVar($f['title']);
					if (!isValidString($rq[$f['title']], $f['min'], $f['max'])) {
						$errors['bad'.$f['title']] = true;
					}
					$_SESSION[$this->area . 'Data'][$f['title']] = $rq[$f['title']];
				}
				if (!count($errors)) {
					if (!empty($rqNewPasswd)) {
						$newUser['passwd'] = $rqNewPasswd;
					}
					$newUser['email'] = $rqEmail;
					foreach ($rq as $k=>$v) {
						$newUser[$k] = $v;
					}
					$db->ModifyContentObject($app->types['suser'], $newUser);
					// FIXME!!!
					// $user = new PXUser($newUser);
					// $user->id    = $newUser['id'];
					// $user->level = $newUser['access'];
					// $user->Auth();
				}
			}
		}
		return $errors;
	}

	function _confirmCreateUser(&$app, &$request, &$user, &$db) {
		$errors = array();
		$rqLogin       = $request->GetVar('login');
		$rqConfirmCode = $request->GetVar('confirmcode');
		if (!count($tmp = $db->GetObjectsByField($app->types['suser'], NULL, 'title', $rqLogin))) {
			$errors['badconfimcode'] = true;
		} else {
			$newUser = current($tmp);
			$tmp = @unserialize($newUser['passwd']);
			if (!isset($tmp[1]) || $rqConfirmCode != $tmp[1]) {
				$errors['badconfimcode'] = true;
			} else {
				$newUser['passwd'] = md5($tmp[0]);
				$newUser['status'] = 1;
				$db->ModifyContentObject($app->types['suser'], $newUser);
				$user = new PXUser();
				$user->setDb($db);
				$user->setApp($app);
				$user->setRequest($request);
				$user->checkAuth();
			}
		}

		return $errors;
	}

	function _createUser(&$app, &$request, &$user, &$db) {
		$errors = array();
		$rqLogin    = $request->GetVar('login');
		$rqPasswd   = $request->GetVar('passwd');
		$rqPasswd2  = $request->GetVar('passwd2');
		$rqEmail    = $request->GetVar('email');
		$_SESSION[$this->area . 'Data']['login'] = $rqLogin;
		$_SESSION[$this->area . 'Data']['email'] = $rqEmail;
		if (!isValidLogin($rqLogin, 2, 16)) {
			$errors['badlogin'] = true;
		}
		if (!isValidPasswd($rqPasswd, 3, 16)) {
			$errors['badpasswd'] = true;
		}
		if ($rqPasswd != $rqPasswd2) {
			$errors['notequalpasswd'] = true;
		}
		if (!isValidEmail($rqEmail)) {
			$errors['bademail'] = true;
		}
		if (count($db->GetObjectsByField($app->types['suser'], NULL, 'title', $rqLogin))) {
			$errors['loginexists'] = true;
		}
		$rq = array();
		foreach ($this->fields as $f) {
			$rq[$f['title']] = $request->GetVar($f['title']);
			if (!isValidString($rq[$f['title']], $f['min'], $f['max'])) {
				$errors['bad'.$f['title']] = true;
			}
			$_SESSION[$this->area . 'Data'][$f['title']] = $rq[$f['title']];
		}
		if (!count($errors)) {
			$confirmCode = generateRandomString(16);
			$newUser = $app->InitContentObject('suser');
			$newUser['title']  = $rqLogin;
			$newUser['passwd'] = serialize(array($rqPasswd, $confirmCode));
			$newUser['email']  = $rqEmail;
			$newUser['access'] = 1024;
			foreach ($rq as $k=>$v) {
				$newUser[$k] = $v;
			}
			$db->AddContentObject($app->types['suser'], $newUser);
			$m = new NLMailMessage();
			$m->SetFrom($this->systemName, $this->systemEmail);
			$m->SetTo(NULL, $rqEmail);
			$m->SetSubject("Регистрация на сайте");
			$message  = "Здравствуйте!\n\n";
			$message .= "Для подтверждения вашей регистрации продите по ссылке:\n";
			$message .= "http://".$request->GetHttpHost()."/action.phtml?area=register&action=confirm&login=".$rqLogin."&confirmcode=".$confirmCode."&onsuccess=".$this->successUrl."&onerror=".$this->errorUrl;
			$m->SetBody($message);
			$m->Send();
		}

		return $errors;
	}

}

?>
