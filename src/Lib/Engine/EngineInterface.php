<?php

namespace PP\Lib\Engine;

/**
 * Engine interface
 */
interface EngineInterface {

	/** @var int User engine id */
	public const USER_ENGINE_ID = 1;

	/** @var int Admin engine id */
	public const ADMIN_ENGINE_ID = 2;

	/** @var int Sbin engine id */
	public const SBIN_ENGINE_ID = 3;

	/** @var string User engine tag */
	public const USER_ENGINE_TAG = 'user';

	/** @var string Admin engine tag */
	public const ADMIN_ENGINE_TAG = 'admin';

	/** @var string Sbin engine tag */
	public const SBIN_ENGINE_TAG = 'sbin';

	/** @var string Index engine behavior tag */
	public const INDEX_BEHAVIOR = 'index';
	/** @var string Action engine behavior tag */
	public const ACTION_BEHAVIOR = 'action';
	/** @var string JSON engine behavior tag */
	public const JSON_BEHAVIOR = 'json';
	/** @var string Popup engine behavior tag */
	public const POPUP_BEHAVIOR = 'popup';

	/**
	 * Runs the Engine
	 *
	 * @return self
	 */
	public function start();

	/**
	 * Runs modules
	 */
	public function runModules();

	/**
	 * Returns engine class ID
	 *
	 * @return int
	 */
	public function engineClass();

	/**
	 * Returns service container
	 *
	 * @return \Symfony\Component\DependencyInjection\Container
	 */
	public function getContainer();

	/**
	 * Returns engine type tag
	 *
	 * @return string
	 */
	public function engineType();

	/**
	 * Returns engine behavior tag
	 *
	 * @return string
	 */
	public function engineBehavior();
}
