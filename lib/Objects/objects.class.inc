<?php
/**
 * documentation was lost
 */

/**
 * Objects container
 *
 * @property-read array $current the current object
 * @property-read array $first the first object in selection
 */
class PXObjects extends PXContentObjects
{

	/**
	 * current (active) object id
	 * @var int
	 * @protected
	 */
	public $currentId;

	/**
	 * current (active) object type
	 * @var string
	 * @protected
	 */
	public $currentType;

	public function hasCurrent()
	{
		return @isset($this[$this->currentType][$this->currentId]);
	}

	public function getCurrent()
	{
		return $this[$this->currentType][$this->currentId];
	}

	public function hasItems($type = null)
	{
		$type = $type ?: $this->currentType;
		return @isset($this[$type]) && (is_countable($this[$type]) ? count($this[$type]) : 0);
	}

	public function getFirst($type)
	{
		return !empty($this[$type]) && is_array($this[$type]) ? reset($this[$type]) : null;
	}

	public function getAllowedChilds()
	{
		$currentObject = $this->getCurrent();
		return PXRegistry::getApp()->getAllowedChilds($this->currentType, $currentObject);
	}

	public function add($type, $objsArray)
	{
		$this[$type] = $objsArray;
	}

	public function __get($key)
	{
		switch ($key) {
			case 'current':
				if ($this->hasCurrent()) {
					return $this->getCurrent();
				}
				break;
			case 'first':
				if ($this->hasItems($this->currentType)) {
					return $this->getFirst($this->currentType);
				}
				break;
			default:
				if (isset($this->$key)) {
					return $this->$key;
				}
		}

		return null;
	}

	public
	function setCurrent($id, $type)
	{
		$this->currentId = $id;
		$this->currentType = $type;
	}

	public function findCurrent($type, $pathname)
	{
		if (!isset(PXRegistry::getTypes($type)->fields['pathname'])) {
			return;
		}

		foreach ($this[$type] as $o) {
			if ($o['pathname'] == $pathname) {
				$this->setCurrent($o['id'], $type);
				return;
			}
		}
	}
}
