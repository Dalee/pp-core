<?php

namespace PP\Lib\Auth;

class NullAuth extends AuthAbstract {

	/**
	 * {@inheritdoc}
	 */
	public function isCredentialsValid() {
		return true;
	}

}
