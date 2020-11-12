<?php

abstract class PXAbstractTrigger
{

	/**
	 * @var PXTriggerDescription
	 */
	protected $description;

	public function __construct(PXTriggerDescription $description)
	{
		$this->description = $description;
	}
}
