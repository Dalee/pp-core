<?php

class SmartyTmplVarsBag extends ArrayObject
{
	/**
	 * @param array $arrayData
	 * @return void
	 */
	public function merge(array $arrayData): void
	{
		foreach ($arrayData as $key => $val) {
			parent::offsetSet($key, $val);
		}
	}

	/**
	 * @param mixed $key
	 * @return mixed
	 */
	public function offsetGet(mixed $key): mixed
	{
		return $this->offsetExists($key) ? parent::offsetGet($key) : null;
	}

}
