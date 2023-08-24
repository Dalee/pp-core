<?php

namespace PP\Lib;

use Closure;
use Countable;
use IteratorAggregate;
use ArrayIterator;
use JsonSerializable;

/**
 * Class Collection
 * @package PP\Lib
 */
class Collection implements Countable, IteratorAggregate, JsonSerializable
{
    /**
     * @var array
     */
    protected $elements = [];

    /**
  * Collection constructor.
  */
    public function __construct(array $elements = [])
    {
        $this->elements = $elements;
    }

    /**
     * @return array
     */
    public function toArray()
    {
        return $this->elements;
    }

    /**
     * @return bool
     */
    public function isEmpty()
    {
        return empty($this->elements);
    }

    /**
     * @return int
     */
    public function count()
    {
        return count($this->elements);
    }

    /**
  * @return array
  */
    public function map(Closure $func)
    {
        return array_map($func, $this->elements);
    }

    /**
  * @return static
  */
    public function filter(Closure $func)
    {
        return new static(array_filter($this->elements, $func));
    }

    /**
     * @return mixed
     */
    public function first()
    {
        return reset($this->elements);
    }

    /**
     * @return mixed
     */
    public function last()
    {
        return end($this->elements);
    }

    /**
     * @return ArrayIterator
     */
    public function getIterator()
    {
        return new ArrayIterator($this->elements);
    }

    /**
  * @return bool
  */
    public function containsKey(mixed $key)
    {
        return array_key_exists($key, $this->elements);
    }

    /**
  * @return mixed
  */
    public function get(mixed $key, mixed $default = null)
    {
        if ($this->containsKey($key)) {
            return $this->elements[$key];
        }

        return $default;
    }

    /**
  * @return Collection
  */
    public function set(mixed $key, mixed $value)
    {
        if (is_null($key)) {
            $this->elements[] = $value;
        } else {
            $this->elements[$key] = $value;
        }

        return $this;
    }

    /**
  * @return Collection
  */
    public function push(mixed $value)
    {
        $this->set(null, $value);

        return $this;
    }

    /**
     * @inheritdoc
     */
    public function jsonSerialize()
    {
        return $this->toArray();
    }

}
