<?php

namespace PP\Lib\Engine\Admin;

use PP\Lib\Session\DatabaseHandler;
use PXApplication;
use PP\Lib\Engine\AbstractEngine;
use Symfony\Component\HttpFoundation\Session\Storage\NativeSessionStorage;
use Symfony\Component\HttpFoundation\Session\Session;

abstract class AbstractAdminEngine extends AbstractEngine
{
    public const SESSION_NAME = 'sid';

    // TODO: auth module should export authorizable behaviour, like this -> (bool)$module->thisIsAdminAuthModule()
    protected $authArea = 'auth';

    /**
     * @var Session
     */
    protected $session = ['factory' => \Symfony\Component\HttpFoundation\Session\Session::class, 'helper' => true];

    protected $initOrder = ['container', 'app', 'db', 'request', 'session', 'user', 'layout'];

    public function engineClass()
    {
        return static::ADMIN_ENGINE_ID;
    }

    protected function initSession($klass)
    {
        $storage = new NativeSessionStorage([
            'cookie_httponly' => true,
            'cookie_secure' => \PXRegistry::getRequest()->GetHttpProto() === 'https',
            'use_strict_mode' => true,
            ], new DatabaseHandler($this->db));

        $this->session = new $klass($storage);
        $this->session->setName(static::SESSION_NAME);
        $this->session->start();
    }

    // static because PXEngineAdminJSON inherited from PXEngineJSON (stupid, but traits available only from php 5.4)
    protected function getModule(PXApplication $app, $area)
    {
        return array_filter([$area => $app->getAvailableModule($area)]);
    }

    protected function hasAdminModules()
    {
        // $user->isAdmin() is obsolete for Admin Engines,
        // because $this->modules are filtered with $user->can('admin'...) before
        return count($this->modules) > 1 || (count($this->modules) == 1 && !isset($this->modules[$this->authArea]));
    }
    /** {@inheritdoc} */
    public function engineType()
    {
        return static::ADMIN_ENGINE_TAG;
    }

}
