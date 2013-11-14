<?php
/*
TODO:
 - подсветка новых сообщений
 - модерирование
*/

class PXModuleForum extends PXModule {
	var $dtForums;
	var $dtTopics;
	var $dtMessages;
	var $dtBlockings;
	var $structType;
	var $userTitle;
	var $userData;
	var $quoteTag;
	
	var $forumSettings;
	var $forumBaseId;
	var $forumTitle;
	var $rqCase;
	var $rqFile;
	var $moduleError;
	
	var $dtCases = array(
		'SETTINGS'    => 'settings',
		'PROFILE'     => 'profile',
		'REGISTER'    => 'register',

		'VIEWUSER'    => 'viewuser',
		'VIEWTOPIC'   => 'viewtopic',
		'VIEWFORUM'   => 'viewforum',

		'NEWMESSAGE'  => 'newmessage',
		'EDITMESSAGE' => 'editmessage',
	);

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->dtForums    = 'struct';
		$this->dtTopics    = isset($settings['topics'])     ? $settings['topics']     : 'forumtopic';
		$this->dtMessages  = isset($settings['messages'])   ? $settings['messages']   : 'forummessage';
		$this->dtBlockings = isset($settings['blockings'])  ? $settings['blockings']  : 'forumblocking';
		$this->structType  = isset($settings['structtype']) ? $settings['structtype'] : 'type';
		$this->userTitle   = isset($settings['usertitle'])  ? $settings['usertitle']  : 'title';
		$this->userData    = isset($settings['userdata'])   ? $settings['userdata']   : 'data';

		$bbTags = array();

		if (isset($settings['bbtag'])) {
			foreach (array_map(create_function('$a','return explode("|", $a);'), is_array($settings['bbtag']) ? $settings['bbtag'] : array($settings['bbtag'])) as $v) {
				$bbTags[$v[0]] = array('o' => $v[1], 'c' => $v[2], 'url' => $v[3], 'strip' => $v['4']);
			}
		}

		if (isset($settings['quotetag'])) {
			if (is_array($settings['quotetag'])) {
				$settings['quotetag'] = reset($settings['quotetag']);
			}

			foreach (array_map(create_function('$a','return explode("|", $a);'), array($settings['quotetag'])) as $v) {
				$bbTags[$v[0]] = array('o' => $v[1], 'c' => $v[2], 'url' => $v[3], 'strip' => $v['4']);
			}

			list($this->quoteTag['tag'],,,,,$this->quoteTag['field']) = explode('|',$settings['quotetag']);
			$bbTags[$this->quoteTag['tag']]['isquote'] = true;
		}

		$bbTagsJS = array();

		if (isset($settings['bbtagjs'])) {
			foreach (array_map(create_function('$a','return explode("|", $a);'), is_array($settings['bbtagjs']) ? $settings['bbtagjs'] : array($settings['bbtagjs'])) as $v) {
				$bbTagsJS[$v[0]] = array('id' => $v[0], 'title' => $v[1], 'type' => (int)$v[2], 'accesskey' => $v['3'], 'start' => $v['4'], 'end' => $v[5]);
			}
		}

		$this->forumSettings = array(
			'view'       => $settings['view'],
			'perpage'    => $settings['perpage'],
			'topicpp'    => $settings['topicpp'],
			'noretitle'  => false,
			'timetoedit' => $settings['timetoedit'],
			'pager'      => $settings['pager'],
			'bbTagsJS'   => $bbTagsJS,
			'bbTags'     => $bbTags,
		);

		foreach(array_map(create_function('$a','return explode("|", $a);'),(array)$settings['views']) as $v) {
			$this->forumSettings['views'][$v[0]] = $v[1];
		}

		@session_start();
	}

	function adminIndex() {
	}

	function _checkCorrectQuery() {
		if ($this->rqFile != 'index.html' || $this->forumBaseId != $this->tree->current->id) {
			$this->moduleError = 'Некорректный запрос.';
			return false;
		}

		return true;
	}

	function indexSettings() {
		$this->forumTitle = 'Настройки интерфейса';
		$this->_checkCorrectQuery();
	}

	function indexProfile() {
		$this->forumTitle = 'Персональные данные';
		if (!$this->_checkCorrectQuery()) {
			return;
		}

		if (!$this->user->IsAuthed()) {
			$this->moduleError = 'Редактирование персональных данных возможно только для авторизовавшегося пользователя';
		}
	}

	function indexRegister() {
		$this->forumTitle = 'Регистрация';
		$this->_checkCorrectQuery();
	}

	function indexViewuser() {
		if (!$this->_checkCorrectQuery()) {
			return;
		}

		$u = $this->db->getObjectsByFieldLimited($this->app->types['suser'], "TRUE", 'title', $this->request->GetVar('id'), 1, 0);

		if (!count($u)) {
			$this->moduleError = 'Нет такого пользователя.';
			return;
		}

		$forumUser               = current($u);
		$forumUser['__unix']     = DateGermanToUnix($forumUser['sys_created']);
		$forumUser['__messages'] = $this->db->GetObjectsByField($this->app->types[$this->dtMessages], "TRUE", 'sys_owner', $forumUser['id'], DB_SELECT_COUNT);

		$this->forumTitle = 'Информация о пользователе '.$forumUser[$this->userTitle];
		$this->layout->assign('forumUser', $forumUser);
	}

	function indexNewmessage() { 
		$rqMid = $this->request->GetVar('mid');
		if (!$rqMid) {
			$this->forumTitle = 'Создание новой темы';
			return;
		}

		$this->forumTitle = 'Ответ на сообщение';

		if (!$message = $this->_getMessage($rqMid)) {
			return;
		}

		if (!$topic = $this->_getTopic($message['parent'])) {
			return;
		}

		$message = array($message);

		$u = $this->_getForumUsers($this->dtMessages, $message);
		$this->layout->assign('forumUsers', $u);

		$message = reset($message);

		$author = array_key_exists($message['sys_owner'], $u) ? $u[$message['sys_owner']]: null;
		$message['__retitle'] = $this->_makeReplyTitle($message['title']);
		$message['__rebody']  = $this->_makeReplyBody($message['body'], $author, $this->request->GetVar('mode', 'nocite'));

		$this->layout->assign('forumReMessage', $message);
	}

	function indexEditmessage() {
		if (!$rqMid = $this->request->GetVar('mid')) {
			$this->moduleError = 'Нет такого сообщения';
			return;
		}

		if (!$message = $this->_getMessage($rqMid)) {
			return;
		}

		$message = array($message);
		$this->_normalizeObjects($this->dtMessages, $message);
		$message = reset($message);

		if ($message['sys_owner'] != $this->user->id) {
			$this->moduleError = 'Вы не можете редактировать чужие сообщения';
			return;
		}

		if (!$topic = $this->_getTopic($message['parent'])) {
			return;
		}

		if (time() - $this->forumSettings['timetoedit'] > $message['__created_unix'] and $this->request->GetVar('result') !== 'ok') {
			$this->moduleError = 'Срок редактирования сообщения закончился';
			return;
		}

		$this->forumTitle = 'Редактирование сообщения';
		$this->layout->Assign('forumEditMessage', $message);
	}

	function indexViewtopic() {
		if ($this->rqForumSettings['active']['view'] == 'tree') {
			$this->subObjects[$this->dtMessages] = $this->db->GetObjectsByField($this->app->types[$this->dtMessages], true, 'parent', $this->currentCid);
		}

		$this->layout->Assign('forumUsers', $this->_getForumUsers($this->dtMessages, $this->subObjects[$this->dtMessages]));
		$this->layout->Assign('format', $this->dtMessages);

		$isTopicAutoPaged = isset($this->objects[$this->currentCtype][$this->currentCid][OBJ_FIELD_CHILDREN]) && $this->objects[$this->currentCtype][$this->currentCid][OBJ_FIELD_CHILDREN][$this->dtMessages] == PP_CHILDREN_FETCH_PAGED && $this->rqForumSettings['active']['view'] != 'tree';

		if ($isTopicAutoPaged) {
			return;
		}

		if ($this->rqForumSettings['active']['view'] != 'plain') {
			return;
		}

		$maxpage = $this->_getMaxPage($this->subObjects[$this->dtMessages]);

		if($this->subObjects->hasCurrent()) {
			$current = $this->objects->getCurrent();
			$currentCid = $current['id'];

			$keys = array_flip(array_keys($this->subObjects[$this->dtMessages]));
			$currentpage = ceil(($keys[$currentCid]+1)/$this->rqForumSettings['active']['perpage']);

		} else {
			$currentpage = $this->_getCurrentPage($maxpage);
		}

		$this->layout->assign('forumCurrentPage', $currentpage);

		$n = 0;
		foreach($this->subObjects[$this->dtMessages] as $k => $v) {
			$n++;
			$this->subObjects[$this->dtMessages][$k]['__page'] = (int)ceil($n/$this->rqForumSettings['active']['perpage']);
		}

		$this->_setForumPages($maxpage, $currentpage);
	}

	function indexViewforum() {
		$u = array();

		if ($this->isAllowedForumContent) {
			$isForumAutoPaged = $this->tree->leafs[$this->tree->current->id]->content[OBJ_FIELD_CHILDREN][$this->dtTopics] == PP_CHILDREN_FETCH_PAGED;

			if (!$isForumAutoPaged) {
				$this->objects[$this->dtTopics] = $this->db->getObjectsByParent($this->app->types[$this->dtTopics], true, $this->tree->current->id, DB_SELECT_TABLE, 'pinned DESC, lastreply DESC');

				$maxpage = $this->_getMaxPage($this->objects[$this->dtTopics]);
				$currentpage = $this->_getCurrentPage($maxpage);

				$this->layout->Assign('forumCurrentPage', $currentpage);

				$this->objects[$this->dtTopics] = array_slice($this->objects[$this->dtTopics], ($currentpage - 1) * $this->rqForumSettings['active']['topicpp'], $this->rqForumSettings['active']['topicpp']);

				$this->_setForumPages($maxpage, $currentpage);

				foreach ($this->objects[$this->dtTopics] as $k => $o) {
					if ($o['count'] <= $this->rqForumSettings['active']['perpage']) {
						continue;
					}

					$max_page = (int)ceil($o['count']/$this->rqForumSettings['active']['perpage']);

					if ($max_page > $this->rqForumSettings['active']['pager'] + 1) {
						$this->objects[$this->dtTopics][$k]['_pages']['pages'] = array_fill(1, $this->rqForumSettings['active']['pager'],  false);

					} else {
						$this->objects[$this->dtTopics][$k]['_pages']['pages'] = array_fill(1, $max_page,  false);
					}

					$this->objects[$this->dtTopics][$k]['_pages']['first'] = 1;
					$this->objects[$this->dtTopics][$k]['_pages']['last'] = $max_page;
				}
			}

			$u += $this->_getForumUsers($this->dtTopics, $this->objects[$this->dtTopics]);
		}

		if (count($this->tree->leafs[$this->tree->current->id]->children)) {
			$topics = $this->db->getObjectsByField($this->app->types[$this->dtTopics], true, 'is_last', true);

			$u += $this->_getForumUsers($this->dtTopics,  $topics);

			foreach($topics as $topic) {
				$this->tree->leafs[$topic['parent']]->content['__lastpost'] = $topic;
			}

			$forumstats = $this->db->Query("SELECT SUM(count+1) as total_messages, COUNT(id) as total_topics, parent FROM {$this->dtTopics} WHERE status = ".$this->db->TrueStatusString(true)." GROUP BY parent");

			foreach($forumstats as $stat) {
				$this->tree->leafs[$stat['parent']]->content['__forumstat'] = $stat;
			}

			$this->_fixForumStats($this->tree, $this->currentSid);
		}

		$this->layout->assign('forumUsers', $u);
		$this->layout->assign('format', $this->dtTopics);
	}

	function _getForumUsers($dt, $objects) {
		$usersIds = $this->_normalizeObjects($dt, $objects);
		return $this->db->getObjectsByIdArray($this->app->types['suser'], true, $usersIds);
	}

	function _getMaxPage($objects) {
		return (int)ceil((count($objects)/$this->rqForumSettings['active']['perpage']));
	}

	function _getCurrentPage($maxpage) {
		$currentpage = intval($this->request->GetVar('page', $maxpage));

		if ($currentpage < 1) {
			$currentpage = 1;
		}

		if ($currentpage > $maxpage) {
			$currentpage = $maxpage;
		}

		return $currentpage;
	}

	function _setForumPages($maxpage, $currentpage) {
		if ($maxpage <= 1) {
			return;
		}

		$pages_min = ($currentpage - $this->rqForumSettings['active']['pager']);
		if ($pages_min < 1) {
			$pages_min = 1;
		}

		$pages_max = ($currentpage + $this->rqForumSettings['active']['pager']);
		if ($pages_max > $maxpage) {
			$pages_max = $maxpage;
		}

		$pages = array_fill($pages_min, $pages_max - $pages_min + 1,  false);
		$pages[$currentpage] = 'current';

		$this->layout->assign('forumPages', array('pages' => $pages, 'first' => 1, 'last' => $maxpage));
	}

	function _getMessage($rqMid) {
		$message = $this->db->getObjectById($this->app->types[$this->dtMessages], $rqMid);

		if (!is_array($message) || !isset($message['id']) || !$message['status']) {
			$this->moduleError = 'Нет такого сообщения';
			return false;
		}

		return $message;
	}

	function _getTopic($id) {
		$topic = $this->db->getObjectById($this->app->types[$this->dtTopics], $id);

		if ($topic['locked']) {
			$this->moduleError = 'Тема закрыта. Вы не можете писать в эту тему.';
			return false;
		}

		return $topic;
	}

	function _setForumPaths() {
		$forumBasePath = createPathByParentId($this->tree, $this->forumBaseId);
		$forumCurrPath = createPathByParentId($this->tree, $this->tree->current->id);

		$this->layout->assign('forumBasePath',    $forumBasePath);
		$this->layout->assign('forumCurrPath',    $forumCurrPath);
	}

	function _setForumData() {
		if (mb_strpos($this->request->GetVar('result', ''), 'error') === 0) {
			if (array_key_exists('registerErrors', $_SESSION)) {
				$this->layout->assign('forumModifyErrors', $_SESSION['registerErrors']);
			}

			if (array_key_exists('registerData', $_SESSION)) {
				$this->layout->assign('forumModifyData', $_SESSION['registerData']);
			}

			if (array_key_exists($this->area . 'Data', $_SESSION)) {
				foreach ($_SESSION[$this->area . 'Data'] as $k => $v) {
					$this->layout->assign('forum' . ucfirst($k) . 'Data', $v);
				}
			}
		}
	}

	function _setError($message) {
		$this->layout->assign('forumCase',  'error');
		$this->layout->assign('forumTitle', 'Ошибка');
		$this->layout->assign('forumError', $message);
	}

	function _setForumDt() {
		$this->layout->assign('forumDtType',      $this->structType);
		$this->layout->assign('forumDtTopics',    $this->dtTopics);
		$this->layout->assign('forumDtMessages',  $this->dtMessages);
		$this->layout->assign('forumDtBlockings', $this->dtBlockings);
		$this->layout->assign('forumDtCases',     $this->dtCases);
	}

	function userIndex() {
		$this->rqForumSettings = $this->getUserSettingsFromRequest($this->request, $this->user);
		$this->forumBaseId     = $this->_guessBaseId($this->tree,  $this->tree->current->id);
		$this->rqFile          = $this->request->GetFile();
		$this->rqCase          = $this->request->GetVar('case', ($this->rqFile == 'index.html') ? 'viewforum' : 'viewtopic');
		$this->moduleError     = NULL;

		$this->isAllowedForumContent = array_key_exists($this->dtTopics, $this->tree->leafs[$this->tree->current->id]->content[OBJ_FIELD_CHILDREN]);

		$methodName = 'index' . ucfirst($this->rqCase);
		if (method_exists($this, $methodName)) {
			call_user_func(array(&$this, $methodName));
		} else {
			$this->moduleError = 'Некорректный запрос';
		}

		$this->_setForumData();
		$this->_setForumPaths();
		$this->_setForumDt();

		$this->layout->assign('forumSettings',    $this->rqForumSettings);
		$this->layout->assign('forumBaseId',      $this->forumBaseId);

		if (!is_null($this->moduleError)) {
			$this->_setError($this->moduleError);
		} else {
			$this->layout->assign('forumTitle', isset($this->forumTitle) ? $this->forumTitle : false);
			$this->layout->assign('forumCase',  $this->rqCase);
		}
	}

	function _guessBaseId($tree, $currentSid) {
		$forumType = $tree->leafs[$currentSid]->content[$this->structType];
		$baseId = $currentSid;

		while ($tree->leafs[$tree->leafs[$baseId]->parent]->content[$this->structType] == $forumType) {
			$baseId = $tree->leafs[$baseId]->parent;
		}

		return $baseId;
	}

	function _fixForumStats($tree, $id) {
		$o =& $tree->leafs[$id]->content;

		if (!array_key_exists('__forumstat',$o)) {
			$o['__forumstat'] = array('total_topics' => 0, 'total_messages' => 0);
		}

		$type = $o[$this->structType];

		foreach ($tree->leafs[$id]->children as $cid) {
			$c =& $tree->leafs[$cid]->content;
			if ($c[$this->structType] == $type and count($tree->leafs[$cid]->children)) {
				$this->_fixForumStats($tree, $cid);
			}

			if (array_key_exists('__forumstat',$c)) {
				$o['__forumstat']['total_topics'] += $c['__forumstat']['total_topics'];
				$o['__forumstat']['total_messages'] += $c['__forumstat']['total_messages'];
			}

			if (!array_key_exists('__lastpost',$c)) {
				continue;
			}

			if (!array_key_exists('__lastpost',$o)) {
				$o['__lastpost'] = $c['__lastpost'];
				continue;
			}

			if ($o['__lastpost']['__lastreply_unix'] < $c['__lastpost']['__lastreply_unix']) {
				$o['__lastpost'] = $c['__lastpost'];
			}
		}
	}

	function _normalizeObjects($typeName, &$objectsArray) {
		$users = array();

		if (!is_array($objectsArray)) {
			$objectsArray = array();
			return array();
		}

		foreach ($objectsArray as $k=>$v) {
			$objectsArray[$k]['__created_unix'] = DateGermanToUnix($v['sys_created']);
			$users[intval($objectsArray[$k]['sys_owner'])] = true;

			if ($typeName == $this->dtTopics) {
				$objectsArray[$k]['__lastreply_unix'] = DateGermanToUnix($v['lastreply']);
				$users[intval($objectsArray[$k]['lastauthor'])] = true;
			}
		}

		return array_keys($users);
	}

	function getUserSettingsFromRequest($request, $user) {
		$s = array();

		if ($user->id) {
			$s['permanent'] = (array_key_exists($this->area, (array)$user->data[$this->userData]) ? $user->data[$this->userData][$this->area] : array()) + $this->forumSettings;
		} else {
			$s['permanent'] = ((array_key_exists($this->area . 'Data', (array)$_SESSION) and array_key_exists('settings', (array)$_SESSION[$this->area . 'Data'])) ? $_SESSION[$this->area . 'Data']['settings'] : array()) + $this->forumSettings;
		}

		$s['active'] = $s['permanent'];

		if (array_key_exists($tmp = $request->GetVar('view' ,'-'), $this->forumSettings['views'])) {
			$s['active']['view'] = $tmp;
		}

		return $s;
	}

	function userAction() {
		$app     = $this->app;
		$db      = $this->db;
		$request = $this->request;
		$user    = $this->user;

		$successUrl = $request->GetVar('onsuccess', $request->GetReferer());
		$errorUrl   = $request->GetVar('onerror', $request->GetReferer());
		$action     = $request->GetVar('action', 'badaction');
		$error      = NULL;

		if (array_key_exists($this->area . 'Data', $_SESSION)) {
			unset($_SESSION[$this->area . 'Data'][$action]);
		}

		switch ($action) {
			case $this->dtCases['NEWMESSAGE']:
				if (isset($app->types[$this->dtBlockings]) && $db->getObjectsByWhere($app->types[$this->dtBlockings], true, "ip&mask = ".ip2long($request->getRemoteAddr())."&mask", DB_SELECT_COUNT)) {
					$error = 'blocked';
					break;
				}

				$rqFid    = $this->_prepareForumIden($request->GetVar('fid'));
				$rqMid    = $this->_prepareForumIden($request->GetVar('mid'));
				$rqTitle  = $this->_prepareForumVar($request->GetVar('title'), 256);
				$rqBody   = $this->_prepareForumVar($request->GetVar('body'));

				$_SESSION[$this->area . 'Data'][$action]['title'] = $rqTitle;
				$_SESSION[$this->area . 'Data'][$action]['body'] = $rqBody;

				if ($rqBody === '') {
					$error = 'badtext';
					break;
				}

				if ($rqTitle === '') {
					$rqTitle = trim(preg_replace('/\s+/s', ' ', bracketActivate($rqBody, $this->forumSettings['bbTags'], 'all')));
					$rqTitle = preg_replace('/(.{55,60}\S{0,12}).*/', '\1', $rqTitle);

					if ($rqTitle === '') {
						$error = 'badtext';
						break;
					}
				}

				$tree = $db->getObjects($app->types['struct'], true, DB_SELECT_TREE);

				if ($rqMid === 0) {
					$fObject = $db->GetObjectById($app->types[$this->dtForums], $rqFid);
					if (!isset($fObject['id'])) {
						$error = 'badparent';
						break;
					}

					$fAllowed = $app->types[$this->dtForums]->allowedChilds($fObject);

					if (!isset($fAllowed[$this->dtTopics])) {
						$error = 'noallowed';
						break;
					}

					if (!$user->can('write', $app->types[$this->dtTopics], $fObject)) {
						$error = 'noaccess';
						break;
					}

					// Создаем тему
					$object    = $this->createTopic($user, $app->types[$this->dtTopics], $rqFid, $rqTitle);
					$topicId   = $db->AddContentObject($app->types[$this->dtTopics], $object);
					$tObject   = $db->GetObjectById($app->types[$this->dtTopics], $topicId);

					$tObject['sys_accessput'] = USERLEVEL_USER;
					$tObject['pathname']      = $tObject['id'].'.html';

					$db->ModifyContentObject($app->types[$this->dtTopics], $tObject);
					$db->ModifyObjectSysVars($app->types[$this->dtTopics], $tObject);

					$object    = $this->createMessage($user, $app->types[$this->dtMessages], $topicId, 0, $rqTitle, $rqBody, $request->getRemoteAddr());
					$messageId = $db->AddContentObject($app->types[$this->dtMessages], $object);

				} else {
					$mObject = $db->GetObjectById($app->types[$this->dtMessages], $rqMid);
					if (!isset($mObject['id'])) {
						$error = 'badparent';
						break;
					}

					// Создаем мессадж
					$tObject   = $db->GetObjectById($app->types[$this->dtTopics], $mObject['parent']);
					$object    = $this->createMessage($user, $app->types[$this->dtMessages], $mObject['parent'], $rqMid, $rqTitle, $rqBody, $request->getRemoteAddr());
					$messageId = $db->AddContentObject($app->types[$this->dtMessages], $object);
				}

				$successUrl = createPathByParentId($tree, $tObject['parent']) . $tObject['pathname'];
				$successUrl = appendParamToUrl($successUrl, '', '', 'm' . $messageId);
				break;

			case $this->dtCases['EDITMESSAGE']: // TODO
				if (isset($app->types[$this->dtBlockings]) && $db->getObjectsByWhere($app->types[$this->dtBlockings], true, "ip&mask = ".ip2long($request->getRemoteAddr())."&mask", DB_SELECT_COUNT)) {
					$error = 'blocked';
					break;
				}

				$rqMid   = $this->_prepareForumIden($request->GetVar('mid'));
				$message = $db->GetObjectById($app->types[$this->dtMessages], $rqMid);

				if (!array_key_exists('id', $message)) {
					$error = 'nomessage';
					break;
				}

				$message  = array($message);
				$usersIds = $this->_normalizeObjects($this->dtMessages, $message);
				$message  = reset($message);

				if ($message['sys_owner'] != $user->id) {
					$error = 'notowner';
					break;
				}

				$topic = $db->GetObjectById($app->types[$this->dtTopics], $message['parent']);
				if ($topic['locked']) {
					$error = 'locked';
					break;
				}

				if (time() - $this->forumSettings['timetoedit'] > $message['__created_unix']) {
					$error = 'timeout';
					break;
				}

				$rqTitle = $this->_prepareForumVar($request->GetVar('title'), 256);
				$rqBody  = $this->_prepareForumVar($request->GetVar('body'));

				$_SESSION[$this->area . 'Data'][$action]['title'] = $rqTitle;
				$_SESSION[$this->area . 'Data'][$action]['body'] = $rqBody;

				if ($rqBody === '') {
					$error = 'badtext';
					break;
				}

				if ($rqTitle === '') {
					$rqTitle = trim(preg_replace('/\s+/s', ' ', bracketActivate($rqBody, $this->forumSettings['bbTags'], 'all')));
					$rqTitle = preg_replace('/(.{55,60}\S{0,12}).*/', '\1', $rqTitle);

					if ($rqTitle === '') {
						$error = 'badtext';
						break;
					}
				}

				$message['body']  = $rqBody;
				$message['title'] = $rqTitle;
				$message['ip']    = $request->getRemoteAddr();
				$db->ModifyContentObject($app->types[$this->dtMessages], $message);

				$tree = new NLTree($db->GetObjects($app->types['struct'], true));
				$successUrl = createPathByParentId($tree, $topic['parent']) . $topic['pathname'];
				$successUrl = appendParamToUrl($successUrl, '', '', 'm' . $message['id']);
				break;

			case $this->dtCases['SETTINGS']:
				$tmp = array();

				if ($pxForumView = $request->GetVar('pxforumview')) {
					if (!array_key_exists($pxForumView, $this->forumSettings['views'])) {
						$error = 'badrequest';
					}
					$tmp['view'] = $pxForumView;
				}

				if ($pxForumPerPage = (int)$request->GetVar('pxforumperpage',0)) {
					$tmp['perpage'] = ($pxForumPerPage < 5 or $pxForumPerPage > 50) ? 20 : $pxForumPerPage;
				}

				if ($pxForumTopicPP = (int)$request->GetVar('pxforumtopicpp', 0)) {
					$tmp['topicpp'] = ($pxForumTopicPP < 5 or $pxForumTopicPP > 50) ? 20 : $pxForumTopicPP;
				}

				$tmp['noretitle'] = (bool)$request->GetVar('pxforumnoretitle');

				if ($user->id) {
					if(is_array($user->data[$this->userData]) and array_key_exists($this->area, $user->data[$this->userData])) {
						$tmp = $tmp + $user->data[$this->userData][$this->area];
					}

					$user->data[$this->userData][$this->area] = $tmp;
					$db->ModifyContentObject($app->types['suser'], $user->data);

				} else {
					$_SESSION[$this->area . 'Data'][$action] = $tmp;
				}

				break;

			default:
				$error = 'badaction';
		}

		$nextLocation = is_null($error) ? $successUrl : $errorUrl;

		if (array_key_exists($tmp = $request->GetVar('view'), $this->forumSettings['views'])) {
			$nextLocation = appendParamToUrl($nextLocation, 'view', $tmp);
		}

		if (!is_null($error)) {
			if (!$request->GetVar('noresult')) {
				$nextLocation = appendParamToUrl($nextLocation, 'result', 'error');
				$nextLocation = appendParamToUrl($nextLocation, 'error', $error);
			}

		} else {
			if (!$request->GetVar('noresult')) {
				$nextLocation = appendParamToUrl($nextLocation, 'result', 'ok');
			}
		}

		return $nextLocation;
	}

	function _makeReplyTitle($string) {
		preg_match("/^(Re(\[([0-9]*)\])?:)?(.*)/", $string, $matches);
		if (count($matches) < 5) {
			return 'Re: '.trim($string);
		}

		if (empty($matches[1])) {
			return 'Re: '.trim($matches[4]);
		} elseif (empty($matches[3])) {
			return 'Re[2]: '.trim($matches[4]);
		} else {
			return 'Re['.($matches[3]+1).']: '.trim($matches[4]);
		}
	}

	function _makeReplyBody($body, $author, $mode) {
		$q = $this->quoteTag['tag'];
		$a = (!is_null($author)) ? trim(preg_replace('#{(\w+)}#e', '$author["\1"]', $this->quoteTag['field'])) : '';
		if ($a) $a = '=' . $a;
		$s = '';

		switch ($mode) {
			case 'nocite':
				break;

			case 'citeall':
				$s = trim($body);
				if (mb_strlen($s)) {
					$s = "[$q$a]" . $s . "[/$q]\n";
				}
				break;

			case 'cite':

			default:
				$s = trim(bracketActivate($body, $this->forumSettings['bbTags'], 'quote'));
				if (mb_strlen($s)) {
					$s = "[$q$a]" . $s . "[/$q]\n";
				}
		}

		return $s;
	}

	function _prepareForumIden($string) {
		$int = intval($string);
		if ($int < 0) {
			$int = 0;
		}

		return $int;
	}

	function _prepareForumVar($string, $maxLen=4096) {
		if (mb_strlen($string) > $maxLen) {
			$string = mb_substr($string, 0, $maxLen)."&hellip;";
		}

		$tr = array(
			'<'      => '&lt;',
			'>'      => '&gt;',
			"\""     => '&quot;',
			"\'"     => '&#39;',
			chr(132) => '&bdquo;',
			chr(145) => '&lsquo;',
			chr(146) => '&rsquo;',
			chr(147) => '&ldquo;',
			chr(148) => '&rdquo;',
			chr(171) => '&laquo;',
			chr(187) => '&raquo;',
			chr(150) => '&ndash;',
			chr(151) => '&mdash;',
			chr(185) => '&#8470;',
			chr(133) => '&hellip;',
		);

		$string = trim(strtr($string, $tr));
		return $string;
	}

	function createTopic($user, $format, $forumId, $title) {
		foreach ($format->fields as $k => $v) $object[$k] = null;
		$object['parent'] = $forumId;
		$object['title']  = $title;
		$object['status'] = true;

		return $object;
	}

	function createMessage($user, $format, $topicId, $messageId, $title, $body, $ip) {
		foreach ($format->fields as $k => $v) {
			$object[$k] = null;
		}

		$object['parent'] = $topicId;
		$object['pid']    = $messageId;
		$object['title']  = $title;
		$object['body']   = $body;
		$object['ip']     = ip2long($ip);
		$object['status'] = true;

		return $object;
	}
}

function bmc_wordwrap($str, $cols=80, $cut="\n") {
	global $bmc_vars;
	$len     = mb_strlen($str);
	$tag     = 0;
	$wordlen = 0;
	$result  = NULL;

	for ($i=0; $i < $len; $i++) {
		$chr = mb_substr($str, $i, 1);

		if ($chr == '<' || $chr == '&') {
			$tag++;
		} elseif ($chr == '>' || $chr == ';') {
			$tag--;
		} elseif ((!$tag) && (ctype_space($chr))) {
			$wordlen = 0;
		} elseif (!$tag) {
			$wordlen++;
		}

		if ((!$tag) && ($wordlen) && (!($wordlen % $cols))) {
			$chr .= $cut;
		}

		$result .= $chr;
	}

	return $result;
}

// Функция обратного вызова для preg_replace_callback().
function hrefCallback($p) {
	if (!empty($p[1])) {
		return $p[0];
	}

	// Преобразуем спецсимволы в HTML-представление.
	$name = htmlspecialchars($p[0], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET);
	// Если нет протокола, добавляем его в начало строки.
	$href = !empty($p[2])? $name : "http://$name";
	// Формируем ссылку.
	return '<a href="'.$href.'" target="_blank" title="Ссылка откроется в новом окне">'.$name.'</a>';
}

// Заменяет ссылки на их HTML-эквиваленты ("подчеркивает ссылки").
function hrefActivate($text) {
  return preg_replace_callback(
    '{
      ((?:src|href)=.)?   # если это src чего-нибудь, то не надо
      (?:
        (\w+://)          # протокол с двумя слэшами
        |                 # - или -
        www\.             # просто начинается на www
      )
      [\w-]+(\.[\w-]+)*   # имя хоста
      \S*                 # URI (но БЕЗ кавычек)
      (?:                 # последний символ должен быть...
          (?<! [[:punct:]] )  # НЕ пунктуацией
        | (?<= [-/&+*]     )  # но допустимо окончание на -/&+*
      )
    }xi',
    "hrefCallback",
    $text
  );
}

function bracketActivate($text, &$tags, $strip = false) {
	$stack = array();
	$pos = 0;

	while (preg_match('#(.*?)\[(/?)(' . implode('|',array_keys($tags)) .')(?:=((?>[^]]{1,64})(?>[^] ]*)))?\]#s',$text, $r, PREG_OFFSET_CAPTURE, $pos)) {
		$pos = $r[0][1] + mb_strlen($r[0][0]);

		if ($r[2][0] === '') { // opening tag
			if (($tmp = array_pop($stack)) === null) {
				$tmp = array('t' => '', 's' => $r[1][0]);
			} else {
				$tmp['s'] .= $r[1][0];
			}

			array_push($stack, $tmp);
			array_push($stack, array('t' => $r[3][0], 's' => '', 'p' => isset($r[4]) ? $r[4][0] : ''));

		} else { // closing tag
			if (($tmp = array_pop($stack)) === null) { // nothing in stack ???
				$tmp = array('t' => '', 's' => ($strip) ? $r[1][0] : $r[0][0]);

			} else {
				if ($tmp['t'] != $r[3][0]) { // wrong close. discard close tag
					$tmp['s'] .= ($strip) ? $r[1][0] : $r[0][0];

				} else {
					if ($strip) {
						if (isset($tags[$tmp['t']]['strip']) && $tags[$tmp['t']]['strip']) {
							$openTag = $closeTag = $tmp['s'] = $r[1][0] = '';
						} elseif ($strip == 'all') {
							$openTag = $closeTag = '';
						} else {
							$openTag  = "[{$tmp['t']}" . ($tmp['p'] ? '=' . $tmp['p'] : '') . "]";
							$closeTag = "[/{$tmp['t']}]";
						}

					} elseif ($tmp['p']) {
						if ($tags[$tmp['t']]['url'] & 2) {
							if (!mb_strpos($tmp['p'], '://')) $tmp['p'] = 'http://' . $tmp['p'];
						}

						$openTag  = str_replace(array('{#1}','{1}','{/1}'),array($tmp['p']),$tags[$tmp['t']]['o']);
						$closeTag = str_replace(array('{#1}','{1}','{/1}'),array($tmp['p']),$tags[$tmp['t']]['c']);

					} else {
						$openTag  = preg_replace('~\{1\}.*?\{/1\}~','',$tags[$tmp['t']]['o']);
						$closeTag = preg_replace('~\{1\}.*?\{/1\}~','',$tags[$tmp['t']]['c']);
					}

					$s = $openTag . $tmp['s'] . $r[1][0] . $closeTag;
					if (($tmp = array_pop($stack)) === null) {
						$tmp = array('t' => '', 's' => $s);
					} else {
						$tmp['s'] .= $s;
					}
				}
			}

			array_push($stack, $tmp);
		}
	}

	$s = '';
	foreach ($stack as $v) {
		$s .= (($v['t'] and !$strip) ? '[' . $v['t'] . ']' : '') . $v['s'];
	}
	$s .= mb_substr($text,$pos);

	return $s;
}

function PXForumDisplayMessage($string, &$bbTags) {
	$string = bracketActivate($string, $bbTags);
	$string = hrefActivate($string);
	$string = wordWrapHtmlSafe($string, 60, " ", true);
	$string = str_replace('<br />', '<br>', $string);

	return $string;
}
?>
