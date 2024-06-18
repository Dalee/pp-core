<?php

namespace PP\Module;

use PP\DependencyInjection\ContainerAwareInterface;

/**
 * Interface ModuleInterface.
 *
 * @package PP\Module
 */
interface ModuleInterface extends ContainerAwareInterface
{
    public const ACTION_INDEX = 'index';
    public const ACTION_ACTION = 'action';
    public const ACTION_JSON = 'json';
    public const ACTION_POPUP = 'popup';

    /**
     * @return mixed
     */
    public function adminIndex();

    /**
     * @return string
     */
    public function adminAction();

    /**
     * @return string
     */
    public function adminPopup();

    /**
     * @return mixed
     */
    public function userIndex();

    /**
     * @return mixed
     */
    public function userAction();

    /**
     * @return array|null
     */
    public function userJson();

    /**
     * @return string
     */
    public function adminJson();
}
