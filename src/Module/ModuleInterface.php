<?php

namespace PP\Module;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;

/**
 * Interface ModuleInterface.
 *
 * @package PP\Module
 */
interface ModuleInterface extends ContainerAwareInterface {

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
