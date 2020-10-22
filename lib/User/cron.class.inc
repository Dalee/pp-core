<?php

class PXUserCron extends PXUser
{
	public function aclType()
	{
		return 'GOD';
	}

	public function can($what, $format = NULL, $object = NULL)
	{
		return true;
	}
}
