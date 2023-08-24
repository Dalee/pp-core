<?php

namespace PP\Lib;

/**
 * Class PropertyCollection
 * @package PP\Lib
 */
class ArrayCollection extends Collection implements \ArrayAccess
{
    /**
     * {@inheritdoc}
     */
    public function offsetExists($offset)
    {
        return array_key_exists($offset, $this->elements);
    }

    /**
     * {@inheritdoc}
     */
    public function offsetGet($offset)
    {
        return ($this->offsetExists($offset))
            ? $this->elements[$offset]
            : null;
    }

    /**
     * {@inheritdoc}
     */
    public function offsetSet($offset, $value)
    {
        $this->elements[$offset] = $value;
    }

    /**
     * {@inheritdoc}
     */
    public function offsetUnset($offset)
    {
        if ($this->offsetExists($offset)) {
            unset($this->elements[$offset]);
        }
    }

    /**
     * Build collection from key/value array
     *
     * @param $sourceList
     */
    public function fromArray($sourceList)
    {
        foreach ($sourceList as $key => $value) {
            $this->offsetSet($key, $value);
        }
    }

    /**
     * Get value from nested arrays defined by path.
     *
     * @param string $path
     * @param string $delimiter
     * @return mixed
     */
    public function getByPath($path, $delimiter = '.')
    {
        return $this->getByPathFromArray($path, $delimiter, $this);
    }

    /**
  * @param string $path
  * @param string $delimiter
  * @return mixed
  */
    protected function getByPathFromArray($path, $delimiter, array|\PP\Lib\ArrayCollection &$from)
    {
        $keyList = explode($delimiter, $path);
        $key = array_shift($keyList);

        $result = $from[$key] ?? null;

        if ($result === null) {
            return $result;
        }

        if (!empty($keyList)) {
            return $this->getByPathFromArray(implode($delimiter, $keyList), $delimiter, $result);
        } else {
            return $result;
        }
    }
}
