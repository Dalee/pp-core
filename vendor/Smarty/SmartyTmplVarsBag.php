<?php

class SmartyTmplVarsBag extends ArrayObject
{
	public function merge(array $arrayData): void
	{
		foreach ($arrayData as $key => $val) {
			parent::offsetSet($key, $val);
		}
	}

	public function offsetGet(mixed $key): mixed
	{
		return $this->offsetExists($key) ? parent::offsetGet($key) : null;
	}

}
