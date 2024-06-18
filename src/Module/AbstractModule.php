<?php

namespace PP\Module;

use PP\DependencyInjection\ContainerAwareTrait;

/**
 * Class AbstractModule.
 *
 * @package PP\Module
 */
abstract class AbstractModule implements ModuleInterface
{
    use ContainerAwareTrait;

    /**
     * @var \PXApplication
     */
    public $app;

    /**
     * @var \PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver
     */
    public $db;

    /**
     * @var \PXRequest
     */
    public $request;

    /**
     * @var \PXUser|\PXUserAuthorized
     */
    public $user;

    /**
     * @var \PP\Lib\Html\Layout\LayoutAbstract|\PP\Lib\Html\Layout\AdminHtmlLayout
     */
    public $layout;

    /**
     * @var \PP\Lib\Http\Response
     */
    public $response;

    public function __construct(public $area, public $settings, protected $__selfDescription = null)
    {
        //for module acl checks purposes

        \PXRegistry::assignToObject($this);
    }

    /**
     * @return array
     */
    public static function getAclModuleActions()
    {
        $app = \PXRegistry::getApp();

        return [
            'viewmenu' => $app->langTree->getByPath('module_macl_rules.actions.viewmenu.rus'),
            'admin' => $app->langTree->getByPath('module_macl_rules.actions.admin.rus'),
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function adminIndex()
    {
        $this->layout->assignError('INNER.1.0', 'Функция <em>adminIndex</em> данного модуля не определена');
    }

    /**
     * {@inheritdoc}
     */
    public function adminPopup()
    {
        $this->layout->assignError('OUTER.CONTENT', 'Функция <em>adminPopup</em> данного модуля не определена');
    }

    /**
     * {@inheritdoc}
     */
    public function adminAction()
    {
        FatalError("Функция <em>adminAction</em> данного модуля не определена");
    }

    /**
     * {@inheritdoc}
     */
    public function userIndex()
    {
    }

    /**
     * {@inheritdoc}
     */
    public function userAction()
    {
    }

    /**
     * {@inheritdoc}
     */
    public function userJson()
    {
    }

    /**
     * {@inheritdoc}
     */
    public function adminJson()
    {
    }
}
