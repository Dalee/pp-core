<?php

namespace PP\Lib\Html\Layout;

/**
 * Class NullLayout
 * @package PP\Lib\Html\Layout
 */
class NullLayout implements LayoutInterface
{
    /**
     * {@inheritdoc}
     */
    public function assign($name, $value)
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setApp(\PXApplication $app)
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setLang($lang = 'rus')
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getLang()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function getSmarty()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function getIndexTemplate()
    {
        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function setContent($content)
    {
        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function getContent()
    {
        return null;
    }

}
