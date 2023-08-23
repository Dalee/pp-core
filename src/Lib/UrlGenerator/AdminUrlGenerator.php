<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class AdminUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class AdminUrlGenerator extends AbstractUrlGenerator
{
    /**
     * {@inheritDoc}
     */
    public function indexUrl($params = [])
    {
        $oldParams = [];
        $url = '/admin/';
        $oldParams['area'] = $this->getArea();
        $params = array_replace($oldParams, $params);
        return $this->generateUrl($url, $params);
    }

    /**
     * {@inheritDoc}
     */
    public function actionUrl($params = [])
    {
        $oldParams = [];
        $url = '/admin/action.phtml';
        $oldParams['area'] = $this->getArea();
        $sid = $this->getSid();
        if ($sid !== null) {
            $oldParams['sid'] = $sid;
        }
        $params = array_replace($oldParams, $params);
        return $this->generateUrl($url, $params);
    }

    /**
     * {@inheritDoc}
     */
    public function jsonUrl($params = [])
    {
        $oldParams = [];
        $url = '/admin/json.phtml';
        $oldParams['area'] = $this->getArea();
        $params = array_replace($oldParams, $params);
        return $this->generateUrl($url, $params);
    }

    /**
     * {@inheritDoc}
     */
    public function popupUrl($params = [])
    {
        $oldParams = [];
        $url = '/admin/popup.phtml';
        $oldParams['area'] = $this->getArea();
        $sid = $this->getSid();
        if ($sid !== null) {
            $oldParams['sid'] = $sid;
        }
        $params = array_replace($oldParams, $params);
        return $this->generateUrl($url, $params);
    }

}
