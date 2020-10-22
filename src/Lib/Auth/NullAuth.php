<?php

namespace PP\Lib\Auth;

class NullAuth extends AuthAbstract {

	/**
	 * {@inheritdoc}
	 */
	public function isAuthorized(): bool
	{
		return true;
	}

	/**
	 * {@inheritdoc}
	 */
	public function isCredentialsValid(array $credentials): bool
	{
		return true;
	}

}
