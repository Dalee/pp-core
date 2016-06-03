<?php
/**
 * Created by PhpStorm.
 * User: arkady
 * Date: 30/05/16
 * Time: 21:31
 */

namespace PP\Lib\Auth;


use Symfony\Component\HttpFoundation\Session\Session;

interface AuthInterface {

	/**
	 * @return bool
	 */
	function isCredentialsValid();

	/**
	 * @param \PXRequest $request
	 * @return $this
	 */
	public function setRequest(\PXRequest $request);

	/**
	 * @param \PXDatabase $db
	 * @return $this
	 */
	public function setDb(\PXDatabase $db);

	/**
	 * @param \PXApplication $app
	 * @return $this
	 */
	public function setApp(\PXApplication $app);

	/**
	 * @param \PXUser $user
	 * @return $this
	 */
	public function setUser(\PXUser $user);

	/**
	 * @param Session|null $session
	 * @return mixed
	 */
	public function setSession(Session $session = null);
}
