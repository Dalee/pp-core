<?
/*
TODO:
 - подсветка новых сообщений
 - модерирование
 -
*/

class PXModuleForum extends PXAbstractModule {
	var $dtForums;
	var $dtTopics;
	var $dtMessages;
	var $dtBlockings;
	var $structType;
	var $userTitle;
	var $userData;
	var $forumSettings;
	var $quoteTag;

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

	function PXModuleForum($area, $settings) {
		$this->PXAbstractModule($area, $settings);
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
			if (is_array($settings['quotetag'])) $settings['quotetag'] = reset($settings['quotetag']);
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
			'view'    => $settings['view'],
			'perpage' => $settings['perpage'],
			'topicpp' => $settings['topicpp'],
			'noretitle' => false,
			'timetoedit' => $settings['timetoedit'],
			'pager'   => $settings['pager'],
			/*
			'sortby'  => array(
				'title'     => 'sortby=title',
				'answers'   => 'sortby=answers',
				'created'   => 'sortby=created',
				'lastreply' => 'sortby=lastreply',
			), */
			'bbTagsJS' => $bbTagsJS,
			'bbTags' => $bbTags,
		);
		foreach(array_map(create_function('$a','return explode("|", $a);'),$settings['views']) as $v) {
			$this->forumSettings['views'][$v[0]] = $v[1];
		}
		@session_start();
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, &$currentCid, &$currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {

		$forumSettings = $this->getUserSettingsFromRequest($request, $user);

		$forumBaseId   = $this->_guessBaseId($tree, $currentSid);
		$forumBasePath = createPathByParentId($tree, $forumBaseId);
		$forumCurrPath = createPathByParentId($tree, $currentSid);

		$rqFile = $request->GetFile();
		$rqCase = $request->GetVar('case', ($rqFile == 'index.html') ? 'viewforum' : 'viewtopic');

		if (strpos($request->GetVar('result', ''), 'error') === 0) {
			if (array_key_exists('registerErrors', $_SESSION)) {
				$layout->Assign('forumModifyErrors', $_SESSION['registerErrors']);
			}
			if (array_key_exists('registerData', $_SESSION)) {
				$layout->Assign('forumModifyData', $_SESSION['registerData']);
			}
			if (array_key_exists($this->area . 'Data', $_SESSION)) {
				foreach ($_SESSION[$this->area . 'Data'] as $k => $v) {
					$layout->Assign('forum' . ucfirst($k) . 'Data', $v);
				}
			}
		}

		$moduleError = NULL;

		switch ($rqCase) {
			case $this->dtCases['SETTINGS']:
				$forumTitle = 'Настройки интерфейса';
				if ($rqFile != 'index.html' || $forumBaseId != $currentSid) {
					$moduleError = 'Некорректный запрос.';
					break;
				}
				break;
			case $this->dtCases['PROFILE']:
				$forumTitle = 'Персональные данные';
				if ($rqFile != 'index.html' || $forumBaseId != $currentSid) {
					$moduleError = 'Некорректный запрос.';
					break;
				}
				if (!$user->IsAuthed()) {
					$moduleError = 'Редактирование персональных данных возможно только для авторизовавшегося пользователя';
					break;
				}
				break;
			case $this->dtCases['REGISTER']:
				$forumTitle = 'Регистрация';
				if ($rqFile != 'index.html' || $forumBaseId != $currentSid) {
					$moduleError = 'Некорректный запрос.';
					break;
				}
				break;
			case $this->dtCases['VIEWUSER']:
				if ($rqFile != 'index.html' || $forumBaseId != $currentSid) {
					$moduleError = 'Некорректный запрос.';
					break;
				}
				$u = $db->GetObjectsByFieldLimited($app->types['suser'], "TRUE", 'title', $request->GetVar('id'), 1, 0);
				if (count($u)) {
					$forumUser = current($u);
					$forumTitle = 'Информация о пользователе '.$forumUser[$this->userTitle];
					$forumUser['__unix']     = DateGermanToUnix($forumUser['sys_created']);
					$forumUser['__messages'] = $db->GetObjectsByField($app->types[$this->dtMessages], "TRUE", 'sys_owner', $forumUser['id'], DB_SELECT_COUNT);
					$layout->Assign('forumUser', $forumUser);
				} else {
					$moduleError = 'Нет такого пользователя.';
				}
				break;
			case $this->dtCases['NEWMESSAGE']:
				if ($rqMid = $request->GetVar('mid')) {
					$forumTitle = 'Ответ на сообщение';
					$message = $db->GetObjectById($app->types[$this->dtMessages], $rqMid);
					if (!is_array($message) || !isset($message['id']) || !$message['status']) {
						$moduleError = 'Нет такого сообщения';
						break;
					}
					$topic = $db->GetObjectById($app->types[$this->dtTopics], $message['parent']);
					if ($topic['locked']) {
						$moduleError = 'Тема закрыта. Вы не можете писать в эту тему.';
						break;
					}
					$message = array($message);
					$usersIds = $this->_normalizeObjects($this->dtMessages, $message);
					$u = $db->GetObjectsByIdArray($app->types['suser'], true, $usersIds);
					$layout->Assign('forumUsers', $u);
					$author = array_key_exists($message[0]['sys_owner'], $u) ? $u[$message[0]['sys_owner']]: null;
					$message[0]['__retitle'] = $this->_makeReplyTitle($message[0]['title']);
					$message[0]['__rebody']  = $this->_makeReplyBody($message[0]['body'], $author, $request->GetVar('mode', 'nocite'));
					$layout->Assign('forumReMessage', $message[0]);
				} else {
					$forumTitle = 'Создание новой темы';
				}
				break;
			case $this->dtCases['EDITMESSAGE']: // TODO
				if (!$rqMid = $request->GetVar('mid')) {
					$moduleError = 'Нет такого сообщения';
					break;
				}

				$message = $db->GetObjectById($app->types[$this->dtMessages], $rqMid);
				if (!is_array($message) || !isset($message['id']) || !$message['status']) {
					$moduleError = 'Нет такого сообщения';
					break;
				}
				$message = array($message);
				$usersIds = $this->_normalizeObjects($this->dtMessages, $message);
				$message = reset($message);

				if ($message['sys_owner'] != $user->id) {
					$moduleError = 'Вы не можете редактировать чужие сообщения';
					break;
				}

				$topic = $db->GetObjectById($app->types[$this->dtTopics], $message['parent']);
				if ($topic['locked']) {
					$moduleError = 'Тема закрыта. Вы не можете писать в эту тему.';
					break;
				}

				if (time() - $this->forumSettings['timetoedit'] > $message['__created_unix'] and $request->GetVar('result') !== 'ok') {
					$moduleError = 'Срок редактирования сообщения закончился';
					break;
				}

				$forumTitle = 'Редактирование сообщения';
				$layout->Assign('forumEditMessage', $message);
				break;
			case $this->dtCases['VIEWTOPIC']:
				$usersIds = $this->_normalizeObjects($this->dtMessages, $subObjects[$this->dtMessages]);
				$u = $db->GetObjectsByIdArray($app->types['suser'], true, $usersIds);
				$layout->Assign('forumUsers', $u);
				if ($forumSettings['active']['view'] == 'plain') {
					$maxpage = (int)ceil((count($subObjects[$this->dtMessages])/$forumSettings['active']['perpage']));

					if($currentSCid !== -1) {
						$keys = array_flip(array_keys($subObjects[$this->dtMessages]));
						$currentpage = ceil(($keys[$currentSCid]+1)/$forumSettings['active']['perpage']);
					} else {
						$currentpage = intval($request->GetVar('page', $maxpage));
						if ($currentpage < 1) $currentpage = 1;
						if ($currentpage > $maxpage) $currentpage = $maxpage;
					}

					$n = 0;
					foreach($subObjects[$this->dtMessages] as $k => $v) {
						$n++;
						$subObjects[$this->dtMessages][$k]['__page'] = (int)ceil($n/$forumSettings['active']['perpage']);
					}
					$layout->Assign('forumCurrentPage', $currentpage);
					if ($maxpage > 1) {
						$pages_min = ($currentpage - $forumSettings['active']['pager']);
						if ($pages_min < 1) $pages_min = 1;
						$pages_max = ($currentpage + $forumSettings['active']['pager']);
						if ($pages_max > $maxpage) $pages_max = $maxpage;
						$pages = array_fill($pages_min, $pages_max - $pages_min + 1,  false);
						$pages[$currentpage] = 'current';
						$layout->Assign('forumPages', array('pages' => $pages, 'first' => 1, 'last' => $maxpage));
					}
				}
				break;
			case $this->dtCases['VIEWFORUM']:
				$u = array();
				if (array_key_exists($this->dtTopics, $tree->leafs[$currentSid]->content['allowed'])) {
					$objects[$this->dtTopics] = $db->GetObjectsByParent($app->types[$this->dtTopics], true, $currentSid, DB_SELECT_TABLE, 'pinned DESC, lastreply DESC');

					$usersIds = $this->_normalizeObjects($this->dtTopics, $objects[$this->dtTopics]);
					$u += $db->GetObjectsByIdArray($app->types['suser'], true, $usersIds);

					$currentpage = intval($request->GetVar('page', 1));
					$maxpage = (int)ceil((count($objects[$this->dtTopics])/$forumSettings['active']['topicpp']));
					if ($currentpage < 1) $currentpage = 1;
					if ($currentpage > $maxpage) $currentpage = $maxpage;
					$objects[$this->dtTopics] = array_slice($objects[$this->dtTopics], ($currentpage - 1) * $forumSettings['active']['topicpp'], $forumSettings['active']['topicpp']);
					$layout->Assign('forumCurrentPage', $currentpage);
					if ($maxpage > 1) {
						$pages_min = ($currentpage - $forumSettings['active']['pager']);
						if ($pages_min < 1) $pages_min = 1;
						$pages_max = ($currentpage + $forumSettings['active']['pager']);
						if ($pages_max > $maxpage) $pages_max = $maxpage;
						$pages = array_fill($pages_min, $pages_max - $pages_min + 1,  false);
						$pages[$currentpage] = 'current';
						$layout->Assign('forumPages', array('pages' => $pages, 'first' => 1, 'last' => $maxpage));
					}
					foreach ($objects[$this->dtTopics] as $k => $o) {
						if ($o['count'] <= $forumSettings['active']['perpage']) continue;
						$max_page = (int)ceil($o['count']/$forumSettings['active']['perpage']);
						if ($max_page > $forumSettings['active']['pager'] + 1) {
							$objects[$this->dtTopics][$k]['_pages']['pages'] = array_fill(1, $forumSettings['active']['pager'],  false);
						} else {
							$objects[$this->dtTopics][$k]['_pages']['pages'] = array_fill(1, $max_page,  false);
						}
						$objects[$this->dtTopics][$k]['_pages']['first'] = 1;
						$objects[$this->dtTopics][$k]['_pages']['last'] = $max_page;
					}
				}
				if (count($tree->leafs[$currentSid]->children)) {
					$topics = $db->GetObjectsByField($app->types[$this->dtTopics], true, 'is_last', true);
					$usersIds = $this->_normalizeObjects($this->dtTopics, $topics);
					$u += $db->GetObjectsByIdArray($app->types['suser'], true, $usersIds);
					foreach($topics as $topic) {
						$tree->leafs[$topic['parent']]->content['__lastpost'] = $topic;
					}
					$forumstats = $db->Query("SELECT SUM(count+1) as total_messages, COUNT(id) as total_topics, parent FROM {$this->dtTopics} WHERE status = ".$db->TrueStatusString(true)." GROUP BY parent");
					foreach($forumstats as $stat) {
						$tree->leafs[$stat['parent']]->content['__forumstat'] = $stat;
					}
					$this->_fixForumStats($tree,$currentSid);
				}
				$layout->Assign('forumUsers', $u);
				break;
			default:
				$moduleError = 'Некорректный запрос';
				break;

		}
		$layout->Assign('forumSettings',    $forumSettings);
		$layout->Assign('forumBaseId',      $forumBaseId);
		$layout->Assign('forumBasePath',    $forumBasePath);
		$layout->Assign('forumCurrPath',    $forumCurrPath);
		$layout->Assign('forumDtType',      $this->structType);
		$layout->Assign('forumDtTopics',    $this->dtTopics);
		$layout->Assign('forumDtMessages',  $this->dtMessages);
		$layout->Assign('forumDtBlockings', $this->dtBlockings);
		$layout->Assign('forumDtCases',     $this->dtCases);
		if (!is_null($moduleError)) {
			$layout->Assign('forumCase',  'error');
			$layout->Assign('forumTitle', 'Ошибка');
			$layout->Assign('forumError', $moduleError);
		} else {
			$layout->Assign('forumTitle', isset($forumTitle) ? $forumTitle : false);
			$layout->Assign('forumCase',  $rqCase);
		}
	}

	function _guessBaseId(&$tree, $currentSid) {
		$forumType = $tree->leafs[$currentSid]->content[$this->structType];
		$baseId = $currentSid;
		while ($tree->leafs[$tree->leafs[$baseId]->parent]->content[$this->structType] == $forumType) {
			$baseId = $tree->leafs[$baseId]->parent;
		}
		return $baseId;
	}

	function _fixForumStats(&$tree, $id) {
		$o =& $tree->leafs[$id]->content;
		if (!array_key_exists('__forumstat',$o)) {
			$o['__forumstat'] = array('total_topics' => 0, 'total_messages' => 0);
		}
		$type = $o[$this->structType];
		foreach ($tree->leafs[$id]->children as $cid) {
			$c =& $tree->leafs[$cid]->content;
			if ($c[$this->structType] == $type and count($tree->leafs[$cid]->children)) $this->_fixForumStats($tree, $cid);
			if (array_key_exists('__forumstat',$c)) {
				$o['__forumstat']['total_topics'] += $c['__forumstat']['total_topics'];
				$o['__forumstat']['total_messages'] += $c['__forumstat']['total_messages'];
			}
			if (!array_key_exists('__lastpost',$c)) continue;
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

	function getUserSettingsFromRequest(&$request, &$user) {
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

	function userAction(&$app, &$request, &$user, &$db) {
		$successUrl = $request->GetVar('onsuccess', $request->GetReferer());
		$errorUrl   = $request->GetVar('onerror', $request->GetReferer());
		$action     = $request->GetVar('action', 'badaction');
		$error = NULL;
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
					if (!in_array($this->dtTopics, $app->GetAllowedChildsKeys($app->types[$this->dtForums], $fObject))) {
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
					$tObject['pathname'] = $tObject['id'].'.html';
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
				$message = array($message);
				$usersIds = $this->_normalizeObjects($this->dtMessages, $message);
				$message = reset($message);
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
				if (strlen($s)) {
					$s = "[$q$a]" . $s . "[/$q]\n";
				}
				break;
			case 'cite':
			default:
				$s = trim(bracketActivate($body, $this->forumSettings['bbTags'], 'quote'));
				if (strlen($s)) {
					$s = "[$q$a]" . $s . "[/$q]\n";
				}
		}
		return $s;
	}

	function _prepareForumIden($string) {
		$int = intval($string);
		if ($int < 0) $int = 0;
		return $int;
	}

	function _prepareForumVar($string, $maxLen=4096) {
		if (strlen($string) > $maxLen) {
			$string = substr($string, 0, $maxLen)."&hellip;";
		}
		$tr = array(
			'<'      => '&lt;',
			'>'      => '&gt;',
			"\""     => '&quot;',
			"\'"      => '&#39;',
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

	function createTopic(&$user, &$format, $forumId, $title) {
		foreach ($format->fields as $k => $v) $object[$k] = null;
		$object['parent'] = $forumId;
		$object['title']  = $title;
		$object['status'] = true;
		return $object;
	}

	function createMessage(&$user, &$format, $topicId, $messageId, $title, $body, $ip) {
		foreach ($format->fields as $k => $v) $object[$k] = null;
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
	$len     = strlen($str);
	$tag     = 0;
	$wordlen = 0;
	$result  = NULL;

	for ($i=0; $i<$len; $i++) {
		$chr = $str{$i};
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
	if (!empty($p[1])) return $p[0];
	// Преобразуем спецсимволы в HTML-представление.
	$name = htmlspecialchars($p[0]);
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
		$pos = $r[0][1] + strlen($r[0][0]);
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
							if (!strpos($tmp['p'], '://')) $tmp['p'] = 'http://' . $tmp['p'];
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
	foreach ($stack as $v) $s .= (($v['t'] and !$strip) ? '[' . $v['t'] . ']' : '') . $v['s'];
	$s .= substr($text,$pos);
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
