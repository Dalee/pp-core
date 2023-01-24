<?php

namespace PP\Contracts;

interface IApplication
{
	/**
	 * @param string $formatName
	 * @return array
	 */
	public function initContentObject(string $formatName): array;
}
