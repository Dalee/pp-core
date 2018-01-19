<?php

use PP\Lib\Engine\AbstractEngine;
use Symfony\Component\HttpFoundation\Session\Storage\NativeSessionStorage;
use Symfony\Component\HttpFoundation\Session\Storage\Handler\NullSessionHandler;

class PXEngineSbin extends AbstractEngine {

	var $result;

	var $request = ['factory' => 'PXNullRequest'];
	var $user = ['factory' => 'PXUserCron'];

	/**
	 * @var Symfony\Component\HttpFoundation\Session\Session
	 */
	protected $session = ['factory' => 'Symfony\Component\HttpFoundation\Session\Session', 'helper' => true];

	protected $initOrder = ['container', 'app', 'db', 'request', 'session', 'user', 'layout'];

	protected function initSession($klass) {
		$storage = new NativeSessionStorage([], new NullSessionHandler());
		$this->session = new $klass($storage);
		$this->session->start();
	}

	function initDirectories() {
	}

	function initModules() {
	}

	function runModules() {
	}

	public function engineClass() {
		return static::SBIN_ENGINE_ID;
	}

	/** {@inheritdoc} */
	public function engineType() {
		return static::SBIN_ENGINE_TAG;
	}

	/** {@inheritdoc} */
	public function engineBehavior() {
		return static::SBIN_BEHAVIOR;
	}


}
