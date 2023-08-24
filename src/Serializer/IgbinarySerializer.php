<?php

namespace PP\Serializer;

/**
 * Class IgbinarySerializer.
 *
 * @package PP\Serializer
 */
class IgbinarySerializer implements SerializerInterface
{
    /**
     * @return string
     */
    public function getName()
    {
        return 'igbinary';
    }

    /**
     * @return bool
     */
    public function isSupported()
    {
        return extension_loaded('igbinary');
    }

    /**
     * @param $data
     * @return string
     */
    public function serialize($data)
    {
        return igbinary_serialize($data);
    }

    /**
     * @param $data
     * @return mixed
     */
    public function unserialize($data)
    {
        return @igbinary_unserialize($data);
    }

}
