<?php

namespace PP\Lib\Auth;

class Null extends AuthAbstract {

	/**
	 * {@inheritdoc}
	 */
	public function isCredentialsValid() {
		return true;
	}

}
