<?php

namespace PP\Module;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;

/**
 * Interface ModuleInterface.
 *
 * @package PP\Module
 */
interface ModuleInterface extends ContainerAwareInterface {

	const ACTION_INDEX = 'index';
	const ACTION_ACTION = 'action';
	const ACTION_JSON = 'json';
	const ACTION_POPUP = 'popup';

	/**
	 * @return mixed
	 */
	public function adminIndex();

	/**
	 * @return string
	 */
	public function adminAction();

	/**
	 * @return string
	 */
	public function adminPopup();

	/**
	 * @return mixed
	 */
	public function userIndex();

	/**
	 * @return mixed
	 */
	public function userAction();

	/**
	 * @return array|null
	 */
	public function userJson();

	/**
	 * @return string
	 */
	public function adminJson();
}
