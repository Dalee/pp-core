<?php
class PXModuleRegister extends PXAbstractModule {
	var $systemName;
	var $systemEmail;
	var $registerPath;
	var $userActivation;
	var $contactEmailField;
	var $captchaField;
	var $defaultAccess;
	
	var $fields;

	function PXModuleRegister($area, $settings) {
		$this->PXAbstractModule($area, $settings);
		$this->systemName        = GetFromArray($settings,'systemname','PX Site');
		$this->systemEmail       = GetFromArray($settings,'systememail', 'noreply@www');
		$this->registerPath      = GetFromArray($settings,'registerpath', '/registration/');
		$this->userActivation    = GetFromArray($settings,'activation');
		$this->contactEmailField = GetFromArray($settings,'useremail');
		$this->captchaField      = GetFromArray($settings,'captcha');
		$this->defaultAccess     = GetFromArray($settings,'defaultaccess', 1024);
		
		$this->fields           = array();
		
		if (is_array($settings['field'])) {
			foreach ($settings['field'] as $f) {
				$this->fields[] = $this->_parseField($f);
			}
		} else {
			$this->fields[] = $this->_parseField($settings['field']);
		}
	}
	
	function __ctor(&$app, &$db, &$user, &$request){
		$this->app  = & $app;
		$this->db   = & $db;
		$this->user = $user;
		$this->req  = & $request;
	}
	
	function _parseField($formatString) {
		@list($title, $min, $max, $validators) = explode('|', $formatString);
		$validators = !empty($validators) ? explode(',', str_replace(' ', '',$validators)) : array();
		return compact('title', 'min', 'max', 'validators');
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, &$currentSid, &$currentCid, &$currentCtype, &$currentSCid, &$currentSCtype, &$rootId, &$pathId, &$urlFile, &$error) {
		$this->__ctor($app, $db, $user, $request);
		
		$errors = & $this->loadModel('session', 'errors');
		$input  = & $this->loadModel('session', 'data');
		$layout->assign('__view', $request->GetVar('view'));

		switch($request->GetVar('view')){
			default:
				if(!$user->isAuthed()){
					$layout->assign('showRegForm', true);
					if($this->captchaField){
						$layout->assign($this->captchaField, $this->loadModel('captcha'));
					}
				}
			break;
			case 'modify':
				$errors = null;
			case 'modifysuccess':
			case 'modifyerror':
				if($user->isAuthed()){
				    $layout->assign('showRegForm', true);
				    $layout->assign('regErrors', $errors);
				    $layout->assign('getInput', $input);
				    @$layout->assign('regInput', reset($this->loadModel('suser', array('id' => $user->id))));
				}
			break;
			case 'success':
			case 'activation':
			case 'activationsuccess':
			case 'remindsuccess':
			break;
			case 'error':
				if(!$user->isAuthed()){
					$layout->assign('showRegForm', true);
					if($this->captchaField){
						$layout->assign($this->captchaField, $this->loadModel('captcha'));
					}
					$layout->assign('regErrors', $errors);
					$layout->assign('regInput', $input);
				}
			break;
			case 'remind':
				$errors = null;
			case 'reminderror':
				if(!$user->isAuthed()){
					$layout->assign('showRemindForm', true);
					$layout->assign('regErrors', $errors);
					if($this->captchaField){
						$layout->assign($this->captchaField, $this->loadModel('captcha'));
					}
				}
			break;
			case 'activationerror':
				if(!$user->isAuthed()){
					$layout->assign('regErrors', $errors);
				}
			break;
		}
		
		$input = $errors = null;
		$this->_saveModel('session');
	}
	
	function userAction(&$app, &$request, &$user, &$db) {
		$this->__ctor($app, $db, $user, $request);
	
		$this->assignController();
		
		return call_user_func($this->_controller);
	}
	
	function _modifyController(){
		if(!$this->user->isAuthed()) return;
		
		$userData =& $this->loadModel('suser', array('id' => $this->user->id, 'passwd' => $this->req->GetPostVar('currentpassword')));
		if(!count($userData)){
			return $this->buildRoute('modifyerror', array('currentpassword' => 'wrong'));
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

	function _remindController(){
		if($this->user->isAuthed()) return;
		
		$errors =& $this->loadModel('session', 'errors');
		$errors = null;
		
		$key    = $this->req->GetVar('code');
		
		switch(isset($key)){
			case false:
				//Send remind key to the account owner
				if(!count($userToRemember = & $this->loadModel('suser', array('title' => $this->req->GetVar('login'), 'status' => true)))){
					$errors['login']['usernotexists'] = true;
				}
				
				if(!empty($this->captchaField)){
					$this->_captchaFieldValidator($this->captchaField, $this->req->GetVar($this->captchaField), $errors);
				}
				
				if(!count($errors)){
					$userToRemember = reset($userToRemember);
					
					if($this->_sendMail($userToRemember, array('route' => 'remind'))){
						$stages = array('mail' => 'sent');
					} else {
						$stages = array('mail' => 'errorsent');
					}
					$this->_saveModel('session');
					$this->_saveModel('suser', $userToRemember);
					return $this->buildRoute('remindsuccess', $stages);
				}
				
				return $this->buildRoute('reminderror');
			
			case true:
				if(!count($existingUser = & $this->loadModel('suser', array('title' => $this->req->GetPostVar('login'), 'remindcode' => $key)))){
					$errors['login']['badloginorkey'] = true;
				}
				$existingUser = max(array(), reset($existingUser));
				$this->_passwordFieldValidator('newpassword', $this->req->GetPostVar('newpassword'), $errors, $existingUser);
				
				if(!count($errors)){
					$existingUser['remindcode'] = '';
					$this->_saveModel('session');
					$this->_saveModel('suser', $existingUser);
					return $this->buildRoute('remindsuccess', array('password' => 'changed'));
				}
				
				$this->_saveModel('session');
				return $this->buildRoute('reminderror', array('login' => $this->req->GetPostVar('login'), 'code' => $key));
		}
	}
	
	function _activationController() {
		if(!$this->userActivation || $this->user->isAuthed()){
			return;
		}
		
		$errors =& $this->loadModel('session', 'errors');
		$errors = null;
		
		if(!count($inactiveUser =& $this->loadModel('suser', array('title' => $this->req->GetVar('login'), 'activationcode' => $this->req->GetVar('code'))))) {
			$errors['badconfirmcode'] = true;
			$this->_saveModel('session');
			return $this->buildRoute('activationerror');
		} else {
			$activateUser = reset($inactiveUser);
			$activateUser['activationcode'] = '';
			$activateUser['status'] = 1;
			$this->_saveModel('suser', $activateUser);
			$this->_saveModel('session');
			return $this->buildRoute('activationsuccess');
		}
	}
	
	function _parseFields(&$input, &$errors){
		$uData = array();
		foreach ($this->fields as $k => $f) {
			$input[$f['title']] = $uData[$f['title']] = $this->req->GetPostVar($f['title']);

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
	
	function _registerController() {
		if($this->user->isAuthed()) return;
		
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
				if($this->_sendMail($newUser, array('route' => 'activation'))){
					$stages = array('mail' => 'sent');
				} else {
					$stages = array('mail' => 'errorsent');
				}
				$newUser['status'] = false;
			} else {
				$stages = array('account' => 'created');
				$newUser['status'] = true;
			}
			
			$this->_saveModel('suser', $newUser);
			$this->_saveModel('session');
			return $this->buildRoute('success', $stages);
		}
		
		$this->_saveModel('session');
		return $this->buildRoute('error');
	}
	
	function _badactionController() {
		return false;
	}
	
	function assignController(){
		$action = $this->req->GetVar('action');
		if(method_exists($this, '_' . $action . 'Controller')){
			$this->_controller = array(&$this, '_' . $action . 'Controller');
		} else {
			$this->_controller = array(&$this, '_badaction' . 'Controller');
		}
	}
	
	function buildRoute($destination, $stages = array()){
		$returnTo = $this->registerPath;
		// common modification of the stages into uri
		$_stages = array();
		foreach($stages as $k => $v){
			$_stages[] = $k.'='.$v;
		}
		$stages = join('&', $_stages);
		
		switch($destination){
			default:
				return $returnTo;
			
			case 'error':
			case 'modify':
			case 'modifysuccess':
			case 'activationerror':
			case 'activationsuccess':
				$returnTo .= '?view=' . $destination;
			break;
			case 'success':
			case 'reminderror':
			case 'remindsuccess':
			case 'modifyerror':
				$returnTo .= '?view=' . $destination . '&';
			break;
			case 'activation':
				$returnTo = 'http://' . $this->req->GetHttpHost() . '/action.phtml?area=register&action='.$destination.'&';
			break;
			case 'remind':
				$returnTo = 'http://' . $this->req->GetHttpHost() . $this->registerPath . '?view='.$destination.'&';
			break;
		}
		return $returnTo . $stages;
	}
	
	function _saveModel($store, $object = array()){
		switch($store){
			case 'session':
				session_commit();
			break;
			case 'suser':
				// Security
				foreach($this->fields as $field){
					if(isset($object[$field['title']]) && is_string($object[$field['title']])){
						$object[$field['title']] = $this->db->EscapeString($object[$field['title']]);
					}
				}
				$object += $this->app->InitContentObject('suser'); //merge with prototype
				
				if($object['id'] > 0){ // modify
					$this->db->ModifyContentObject($this->app->types['suser'], $object);
				} else { //create
					$this->db->AddContentObject($this->app->types['suser'], $object);
				}
			break;
			case 'captcha':
				if(is_a($object, 'NLBlockingNumbers')){
					$object->CreateNew(4, "2345678qweyupasdfkzxcvbnm2345678");
					return $object->key;
				}
			break;
		}
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
		if (!isValidLogin($value, 2, 16)) {
			$errors[$field]['badlogin'] = true;
		}
		$userData['title'] = $value;
		unset($userData[$field]);
	}
	
	function _passwordFieldValidator($field, $value, &$errors, &$userData){
		if (!isset($value[0]) || !isValidPasswd($value[0], 3, 16)) {
			$errors[$field]['badpassword']    = true;
		}
		if (!isset($value[0]) || !isset($value[1]) || $value[0] !== $value[1]) {
			$errors[$field]['notequalpasswd'] = true;
		}
		
		if(!isset($errors[$field])){
    		    $userData['passwd'] = $value[0];
		}
		
		unset($userData[$field]);
	}
	
	function _emailFieldValidator($field, $value, &$errors, &$userData){
		if(!empty($value) && !isValidEmail($value)){
			$errors[$field]['email'] = true;
		}
	}
	
	function _uniqueFieldValidator($field, $value, &$errors, &$userData){
		if(!empty($value)){
			$suser =& $this->loadModel('suser', array($field => $value));
			if(!count($suser)){
				return;
			}
			$errors[$field]['exists'] = true;
		}
	}
	
	function _reqFieldValidator($field, $value, &$errors, &$userData){
		if(is_array($value)){
		    foreach($value as $everyField){
			if(!strlen($everyField)){
			    $errors[$field]['req'] = true;
    			    break;
			}
		    }
		    return;
		}
		
		if(!strlen($value)){
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
		$file = $this->req->GetUploadFile($field);
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
	
	function _sendMail(&$user, $params){
		if(!isset($user[$this->contactEmailField]) || empty($user[$this->contactEmailField])){
			return false;
		}
		$user[$params['route'] . 'code'] = generateRandomString(16);
		$m = new NLMailMessage();
		$m->SetFrom($this->systemName, $this->systemEmail);
		$m->SetTo(NULL, $user[$this->contactEmailField]);
		
		//Apply mail template
		$m->SetTemplate(
			BASEPATH.'local/mail_templates/registration/'.$params['route'].'.tmpl',
			TRUE,
			array(
				'href' => $this->buildRoute($params['route'], array('code' => $user[$params['route'] . 'code'], 'login' => $user['title']))
			)
		);
		
		return $m->Send();
	}
}

?>
