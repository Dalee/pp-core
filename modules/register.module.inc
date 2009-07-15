<?php
class PXModuleRegister extends PXAbstractModule {
	//config vars
	var $systemName;
	var $systemEmail;
	var $registerPath;
	var $userActivation;
	var $userNotify;
	var $contactEmailField;
	var $captchaField;
	var $defaultAccess;
	var $defaultAuth;

	var $fields;

	// need for automatic assign using PXRegistry
	var $response;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->systemName        = GetFromArray($settings,'systemname','PX Site');           //mail From (name)    in the robot emails
		$this->systemEmail       = GetFromArray($settings,'systememail', 'noreply@www');     //mail From (address) in the robot emails
		$this->registerPath      = GetFromArray($settings,'registerpath', '/registration/'); //path to registration page
		$this->userActivation    = GetFromArray($settings,'activation');                     //enable activation mail for registration
		$this->userNotify        = GetFromArray($settings,'notify');                         //notify users about successful registration
		$this->contactEmailField = GetFromArray($settings,'useremail');                      //form field used for user contact email
		$this->captchaField      = GetFromArray($settings,'captcha');                        //form field used for the captcha
		$this->defaultAccess     = GetFromArray($settings,'defaultaccess', 1024);            //default user access after registration
		$this->defaultAuth       = GetFromArray($settings,'authtype', null);                 //auth engine used on this site, for password encryption purposes

		$this->fields            = array();

		if (is_array($settings['field'])) {
			foreach ($settings['field'] as $f) {
				$this->fields[] = $this->_parseField($f);
			}

		} else {
			$this->fields[] = $this->_parseField($settings['field']);
		}
	}

	function _parseField($formatString) {
		@list($title, $min, $max, $validators) = explode('|', $formatString);
		$validators = !empty($validators) ? explode(',', str_replace(' ', '',$validators)) : array();
		return compact('title', 'min', 'max', 'validators');
	}

	//reFUCKtoring complete
	function userIndex() { //&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, &$currentSid, &$currentCid, &$currentCtype, &$currentSCid, &$currentSCtype, &$rootId, &$pathId, &$urlFile, &$error) {
		$this->response->dontCache();

		$errors = & $this->loadModel('session', 'errors');
		$input  = & $this->loadModel('session', 'data');
		$this->layout->assign('__view', $this->request->GetVar('view'));

		switch($this->request->GetVar('view')){
			default:
				if(!$this->user->isAuthed()){
					$this->layout->assign('showRegForm', true);

					if($this->captchaField){
						$this->layout->assign($this->captchaField, $this->loadModel('captcha'));
					}
				}
				break;

			case 'modify':
				$errors = null;
			case 'modifysuccess':
			case 'modifyerror':
				if($this->user->isAuthed()){
				    $this->layout->assign('showRegForm', true);
				    $this->layout->assign('regErrors', $errors);
				    $this->layout->assign('getInput', $input);
				    @$this->layout->assign('regInput', reset($this->loadModel('suser', array('id' => $user->id))));
				}
				break;

			case 'success':
			case 'activation':
			case 'activationsuccess':
			case 'remindsuccess':
				break;

			case 'error':
				if(!$this->user->isAuthed()){
					$this->layout->assign('showRegForm', true);

					if($this->captchaField){
						$this->layout->assign($this->captchaField, $this->loadModel('captcha'));
					}

					$this->layout->assign('regErrors', $errors);
					$this->layout->assign('regInput', $input);
				}
				break;

			case 'remind':
				$errors = null;
			case 'reminderror':
				if(!$this->user->isAuthed()){
					$this->layout->assign('showRemindForm', true);
					$this->layout->assign('regErrors', $errors);

					if($this->captchaField){
						$this->layout->assign($this->captchaField, $this->loadModel('captcha'));
					}
				}
				break;

			case 'activationerror':
				if(!$this->user->isAuthed()){
					$this->layout->assign('regErrors', $errors);
				}
				break;
		}

		$input = $errors = null;
		$this->_saveModel('session');
	}

	function userAction() { //&$app, &$request, &$user, &$db) {
		$this->response->dontCache();
		$this->assignController();

		return call_user_func($this->_controller);
	}

	function assignController(){
		$action = $this->request->GetVar('action');

		if(method_exists($this, $controller = '_' . $action . 'Controller')) {
			$this->_controller = array(&$this, $controller);
		} else {
			$this->_controller = array(&$this, '_badactionController');
		}
	}

	################################# Controllers #################################

	// {{{ Bad Action Controller
	function _badactionController() {
		return false;
	}
	// }}} Bad Action Controller

	// {{{ Modify Controller
	function _modifyController(){
		if(!$this->user->isAuthed()) {
			return;
		}

		$userData = array();
		if(!is_null($route = $this->_modifyControllerLoadDataHandler($userData))){
			return $route;
		}

		$userData = reset($userData);

		$errors   =& $this->loadModel('session', 'errors');
		$input    =& $this->loadModel('session', 'data');

		$errors = $input = null;

		$modifyingFields = $this->_parseFields($input, $errors);
		foreach($userData as $field => $data){
			if(!isset($errors[$field]) && isset($modifyingFields[$field])){
				$userData[$field] = $modifyingFields[$field];
			}
		}

		$userData['title'] = $this->user->data['title']; //protect login from modification

		$this->_saveModel('session');
		$this->_saveModel('suser', $userData);

		return $this->buildRoute('modifysuccess');
	}

	// TODO: For the future inheritance purposes same handlers may be written for other controllers
	function _modifyControllerLoadDataHandler(&$userData){
		if(!count($userData = $this->loadModel('suser', array('id' => $this->user->id, 'passwd' => $this->request->GetPostVar('currentpassword'))))){
			return $this->buildRoute('modifyerror', array('currentpassword' => 'wrong'));
		}
	}
	// }}} Modify Controller

	// {{{ Remind Controller
	function _remindController(){
		if($this->user->isAuthed()) {
			return;
		}

		$errors =& $this->loadModel('session', 'errors');
		$errors = null;

		$key    = $this->request->GetVar('code');

		switch(isset($key)){
			case false:
				//Send remind key to the account owner
				if(!count($userToRemember = & $this->loadModel('suser', array('title' => $this->request->GetVar('login'), 'status' => true)))){
					$errors['login']['usernotexists'] = true;
				}

				if(!empty($this->captchaField)){
					$this->_captchaFieldValidator($this->captchaField, $this->request->GetVar($this->captchaField), $errors);
				}

				if(!count($errors)){
					$userToRemember = reset($userToRemember);

					if($this->_sendMail($userToRemember, array('route' => 'remind'))){
						$stages = array('mail' => 'remindsent');
					} else {
						$stages = array('mail' => 'reminderrorsent');
					}

					$this->_saveModel('session');
					$this->_saveModel('suser', $userToRemember);

					return $this->buildRoute('remindsuccess', $stages);
				}

				return $this->buildRoute('reminderror');

			case true:
				if(!count($existingUser = & $this->loadModel('suser', array('title' => $this->request->GetPostVar('login'), 'remindcode' => $key)))){
					$errors['login']['badloginorkey'] = true;
				}

				$existingUser = max(array(), reset($existingUser));
				$this->_passwordFieldValidator('newpassword', $this->request->GetPostVar('newpassword'), $errors, $existingUser);

				if(!count($errors)){
					$existingUser['remindcode'] = '';
					$this->_saveModel('session');
					$this->_saveModel('suser', $existingUser);
					return $this->buildRoute('remindsuccess', array('password' => 'changed'));
				}

				$this->_saveModel('session');
				return $this->buildRoute('reminderror', array('login' => $this->request->GetPostVar('login'), 'code' => $key));
		}
	}
	// }}} Remind Controller

	// {{{ Activation Controller
	function _activationController() {
		if(!$this->userActivation || $this->user->isAuthed()){
			return;
		}

		$errors =& $this->loadModel('session', 'errors');
		$errors = null;
		$stages = array();

		if(!count($inactiveUser =& $this->loadModel('suser', array('title' => $this->request->GetVar('login'), 'activationcode' => $this->request->GetVar('code'))))) {
			$errors['badconfirmcode'] = true;
			$this->_saveModel('session');
			return $this->buildRoute('activationerror');

		} else {
			$activateUser = reset($inactiveUser);
			$activateUser['activationcode'] = '';
			$activateUser['status'] = 1;

			if($this->userNotify){
				if($this->_sendMail($activateUser, array('route' => 'notify'))){
					$stages = array('mail' => 'notifysent');
				} else {
					$stages = array('mail' => 'notifyerrorsent');
				}

				$activateUser['__plain_passwd__'] = '';
			}

			$this->_saveModel('suser', $activateUser);
			$this->_saveModel('session');

			return $this->buildRoute('activationsuccess', $stages);
		}
	}
	// }}} Activation Controller

	// {{{ Register Controller
	function _registerController() {
		if($this->user->isAuthed()) {
			return;
		}

		$errors =& $this->loadModel('session', 'errors');
		$input  =& $this->loadModel('session', 'data');
		$errors = $input = null; // clear previous errors and data

		// parse fields
		$rq = $this->_parseFields($input, $errors);

		if (!count($errors)) {
			$newUser['access'] = $this->defaultAccess;

			foreach ($rq as $k=>$v) {
				$newUser[$k] = $v;
			}

			$stages = array();
			if($this->userActivation){
				$mail              = array('route' => 'activation');
				$newUser['status'] = false;

			} else {
				if($this->userNotify){
					$mail = array('route' => 'notify');
				}

				$stages            = array('account' => 'created');
				$newUser['status'] = true;
			}

			if(isset($mail)){
				if($this->_sendMail($newUser, $mail)){
					$stages['mail'] = $mail['route'] . 'sent';
				} else {
					$stages['mail'] = $mail['route'] . 'errorsent';
				}
			}

			$this->_saveModel('suser', $newUser);
			$this->_saveModel('session');

			return $this->buildRoute('success', $stages);
		}

		$this->_saveModel('session');
		return $this->buildRoute('error');
	}
	// }}} Register Controller

	################################# END Controllers #################################

	function _parseFields(&$input, &$errors){
		$uData = array();
		foreach ($this->fields as $k => $f) {
			$input[$f['title']] = $uData[$f['title']] = $this->request->GetPostVar($f['title']);

			// check basic fields allowed length (only strings)
			if(!empty($f['min']) || !empty($f['max'])){
				if(!@isValidString($uData[$f['title']], (int)$f['min'], (int)$f['max'])) {
					$errors[$f['title']]['size'] = true;
				}
			}

			// apply validators
			foreach($f['validators'] as $modifier){
				if(method_exists($this, $validator = '_' . $modifier . 'FieldValidator')){
					$this->$validator($f['title'], $uData[$f['title']], $errors, $uData);
				}
			}
		}
		return $uData;
	}

	function buildRoute($destination, $stages = array()) {
		$_stages = array();
		foreach($stages as $k => $v){
			$_stages[] = $k.'='.$v;
		}
		$stages = join('&', $_stages);

		return method_exists($this, $route = '_' . $destination . 'Route') ? $this->$route($stages) : $this->registerPath;
	}

	function _sendMail(&$user, $params){
		if(!isset($user[$this->contactEmailField]) || empty($user[$this->contactEmailField])){
			return false;
		}

		$m = new NLMailMessage();
		$m->SetFrom($this->systemName, $this->systemEmail);
		$m->SetTo(NULL, $user[$this->contactEmailField]);

		//Apply mail template
		$m->SetTemplate(
			BASEPATH.'local/mail_templates/registration/'.$params['route'].'.tmpl',
			TRUE,
			method_exists($this, $mailto = '_'.$params['route'].'MailData') ? $this->$mailto($user, $params['route']) : array()
		);

		return $m->Send();
	}

	function _saveModel($store, $object = array()){
		switch($store){
			case 'session':
				session_commit();
				break;

			case 'suser':
				$object += $this->app->InitContentObject('suser'); //merge with prototype
				
				if($object['id'] > 0){ // modify
					$this->db->ModifyContentObject($this->app->types['suser'], $object);
				} else { //create 
					$object['id'] = $this->db->AddContentObject($this->app->types['suser'], $object);
				}
				break;

			case 'captcha':
				if(is_a($object, 'NLBlockingNumbers')){
					$object->CreateNew(4, "2345678qweyupasdfkzxcvbnm2345678");
					return $object->key;
				}
				break;
		}

		$this->_modelCache[$store] = $object;
	}

	function &loadModel($store, $filter = null){
		$model = null;

		switch($store){
			case 'session':
				static $firstOpen = 0;

				if(!$firstOpen++){
					MakeDirIfNotExists(BASEPATH."var/sessions/");
					session_save_path(BASEPATH."var/sessions/");
					ini_set('session.use_cookies',      1);
					ini_set('session.use_only_cookies', 1);
					ini_set('session.use_trans_sid',    0);
					session_set_cookie_params(0);

					if(isset($_SESSION)){
						if(session_name() != 'registration'){
							session_commit();
							session_name('registration');
							session_start();
						}

					} else {
						session_name('registration');
						session_start();
					}
				}

				$model =& $_SESSION['registration'][$filter];
				break;

			case 'suser':
				$this->_applyModifiers($filter);
				$model = $this->db->GetObjectsByFieldLimited($this->app->types['suser'], null, $filter, null, 1, 0);
				break;

			case 'captcha':
				$bn = new NLBlockingNumbers(BASEPATH.'/var/blocking_nums');

				if(isset($filter)){
					$model = $bn->CheckValueByKey(key($filter), current($filter));
				} else {
					$model = $this->_saveModel($store, $bn);
				}
				break;
		}

		return $model;
	}

	//{{{ Validators block
	function _loginFieldValidator($field, $value, &$errors, &$userData){
		if (!isValidLogin($value, 1, 255)) { //login length must be controlled in the min and max field options
			$errors[$field]['badlogin'] = true;
		}

		$userData['title'] = $value;

		if($field !== 'title') {
			unset($userData[$field]);
		}
	}

	function _passwordFieldValidator($field, $value, &$errors, &$userData){
		if (!isset($value[0]) || !isValidPasswd($value[0], 1, 255)) { //password length must be controlled in the min and max field options
			$errors[$field]['badpassword']    = true;
		}

		if (!isset($value[0]) || !isset($value[1]) || $value[0] !== $value[1]) {
			$errors[$field]['notequalpasswd'] = true;
		}

		if(!isset($errors[$field])){
			$userData['passwd']           = $this->_passwdModifier($value[0]);
			$userData['__plain_passwd__'] = $value[0];
		}

		if($field !== 'passwd') unset($userData[$field]);
	}

	function _emailFieldValidator($field, $value, &$errors, &$userData){
		if(!empty($value) && !isValidEmail($value)){
			$errors[$field]['email'] = true;
		}
	}

	function _uniqueFieldValidator($field, $value, &$errors, &$userData){
		if(strlen(trim($value))){
			$suser =& $this->loadModel('suser', array($field => trim($value)));

			if(!count($suser) or $this->user->id > 0 && key($suser) == $this->user->id){
				return;
			}

			$errors[$field]['exists'] = true;
		}
	}

	function _reqFieldValidator($field, $value, &$errors, &$userData){
		if(is_array($value)){
			foreach($value as $everyField){
				if(!strlen(trim($everyField))){
					$errors[$field]['req'] = true;
					break;
				}
			}

			return;
		}

		if(!strlen(trim($value))){
			$errors[$field]['req'] = true;
		}
	}

	function _dateFieldValidator($field, $value, &$errors, &$userData){
		extract($value, EXTR_SKIP);

		if(empty($day) && empty($month) && empty($year)){
			$userData[$field] = null;

		} elseif(checkdate((int)$month, (int)$day, (int)$year)){
			$userData[$field] = $year.'-'.$month.'-'.$day;

		} else {
			$errors[$field]['wrongdate'] = true;
		}
	}

	function _fileFieldValidator($field, $value, &$errors, &$userData){
		$file = $this->request->GetUploadFile($field);

		if(!empty($file)){
			$userData[$field] = $file;
		}
	}

	function _captchaFieldValidator($field, $value, &$errors){
		if(!empty($this->captchaField) && 
		    empty($value['key'])       or 
		    empty($value['value'])     or 
		    !$this->loadModel('captcha', array($value['key'] => $value['value']))
		){
			$errors[$field]['badcaptcha']  = true;
		}
	}
	// }}} Validators block

	function _applyModifiers(&$object){
		if(!is_array($object) || empty($object)) {
			return;
		}

		foreach($object as $k => $v){
			if(method_exists($this, $modifier = '_' . $k . 'Modifier')){
				$object[$k] = $this->$modifier($v);
			}
		}
	}

	// {{{ Modifiers block
	function _passwdModifier($password){
		$authmethodname = $this->user->getPrimaryAuthMethod($this->defaultAuth);
		return call_user_func(array($authmethodname, 'passwdToDB'), $password);
	}
	// }}} Modifiers block

	// {{{ Routes block
	// common
	function _defaultRouteRedirect($destination, $urlArgs){
		return $this->registerPath .'?view=' . $destination . (strlen($urlArgs) ? '&' . $urlArgs : '');
	}

	// register controller
	function _successRoute($urlArgs){
		return $this->_defaultRouteRedirect('success', $urlArgs);
	}

	function _errorRoute($urlArgs){
		return $this->_defaultRouteRedirect('error', $urlArgs);
	}

	// modify controller
	function _modifyRoute($urlArgs){
		return $this->_defaultRouteRedirect('modify', $urlArgs);
	}

	function _modifysuccessRoute($urlArgs){
		return $this->_defaultRouteRedirect('modifysuccess', $urlArgs);
	}

	function _modifyerrorRoute($urlArgs){
		return $this->_defaultRouteRedirect('modifyerror', $urlArgs);
	}

	// activation controller
	function _activationRoute($urlArgs){
		return 'http://' . $this->request->GetHttpHost() . "/action.phtml?area={$this->area}&action=activation" . (strlen($urlArgs) ? '&' . $urlArgs : '');
	}

	function _activationsuccessRoute($urlArgs){
		return $this->_defaultRouteRedirect('activationsuccess', $urlArgs);
	}

	function _activationerrorRoute($urlArgs){
		return $this->_defaultRouteRedirect('activationerror', $urlArgs);
	}

	// remind controller
	function _remindRoute($urlArgs){
		return 'http://' . $this->request->GetHttpHost() . $this->registerPath . '?view=remind' . (strlen($urlArgs) ? '&' . $urlArgs : '');
	}

	function _remindsuccessRoute($urlArgs){
		return $this->_defaultRouteRedirect('remindsuccess', $urlArgs);
	}

	function _reminderrorRoute($urlArgs){
		return $this->_defaultRouteRedirect('reminderror', $urlArgs);
	}
	// }}} Routes block
	
	// {{{ Mail templates data assigners block
	function _activationMailData(&$user, $route){
		$user[$route . 'code'] = generateRandomString(16);
		return array(
			'href' => $this->buildRoute($route, array('code' => $user[$route . 'code'], 'login' => $user['title']))
		);
	}

	function _remindMailData(&$user, $route){
		return $this->_activationMailData($user, $route);
	}

	function _notifyMailData(&$user, $route){
		return array(
			'login'    => $user['title'],
			'password' => $user['__plain_passwd__']
		);
	}
	// }}} Mail templates data assigners block
}
?>