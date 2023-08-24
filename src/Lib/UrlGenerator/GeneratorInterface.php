<?php

namespace PP\Lib\UrlGenerator;

/**
 * Interface GeneratorInterface
 * @package PP\Lib\UrlGenerator
 */
interface GeneratorInterface
{
    /**
     * @param array[string]string $params
     * @return string
     */
    public function generate($params = []);

    /**
     * @param array[string]string $params
     * @return string
     */
    public function indexUrl($params = []);

    /**
     * @param array[string]string $params
     * @return string
     */
    public function actionUrl($params = []);

    /**
     * @param array[string]string $params
     * @return string
     */
    public function jsonUrl($params = []);

    /**
     * @param array[string]string $params
     * @return string
     */
    public function popupUrl($params = []);

}
