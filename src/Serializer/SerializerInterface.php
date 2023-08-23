<?php

namespace PP\Serializer;

/**
 * Interface SerializerInterface.
 *
 * @package PP\Serializer
 */
interface SerializerInterface
{
    /**
     * @return string
     */
    public function getName();

    /**
     * @return bool
     */
    public function isSupported();

    /**
     * @param $data
     * @return string
     */
    public function serialize($data);

    /**
     * @param $data
     * @return mixed
     */
    public function unserialize($data);
}
