<?php

namespace PP\Serializer;

/**
 * Interface SerializerAwareInterface.
 *
 * @package PP\Serializer
 */
interface SerializerAwareInterface
{
    public function setSerializer(SerializerInterface $serializer);

}
