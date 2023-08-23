<?php

namespace PP\Lib\Engine\Admin;

use PP\Lib\Http\Response;
use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PXModuleDescription;
use PP\Lib\Html\Layout\AdminHtmlLayout;

/**
 * Class AdminEngineIndex.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineIndex extends AbstractAdminEngine
{
    /** @var AdminHtmlLayout */
    protected $layout = ['factory' => \PP\Lib\Html\Layout\AdminHtmlLayout::class, 'helper' => true];
    protected $menu;
    protected $outerLayout = 'index';
    protected $templateMainArea = 'INNER.0.0';

    public function initLayout($klass)
    {
        $this->layout = new $klass($this->outerLayout, $this->app->types);
    }

    public function initModules()
    {
        $this->modules = $this->app->getAvailableModules();
    }

    public function initMenu()
    {
        $menuItems = [];

        foreach ($this->modules as $module) {
            // check modules acl rules
            if ($this->user->can('viewmenu', $module)) {
                if ($module->getDescription() == '' || $module->getDescription() == PXModuleDescription::EMPTY_DESCRIPTION) {
                    $menuItems[$module->getName()] = $module->getName();
                } else {
                    $menuItems[$module->getName()] = $module->getDescription();
                }
            }
        }

        $this->menu = $menuItems;
    }

    public function showAuthForm()
    {
        if (!isset($this->modules[$this->authArea])) {
            FatalError('Undefined auth module or you forget insert "allo" for "admin" auth module in acl_objects');
        }
        $moduleDescription = $this->modules[$this->authArea];
        $auth = $moduleDescription->getModule();

        $eventData = [
            'engine_type' => $this->engineType(),
            'engine_behavior' => $this->engineBehavior(),
        ];
        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
        }

        $auth->adminIndex();

        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
        }
    }

    public function fillLayout()
    {
        $this->layout->assignFlashes();
        $this->layout->setLogoutForm('?area=exit');
        $this->layout->setMenu($this->menu, $this->area, 'area', false);

        $this->layout->setTwoColumns();

        $this->layout->setGetVarToSave('area', $this->area);
        $this->layout->setGetVarToSave('sid', $this->request->getSid());
    }

    protected function checkArea($area)
    {
        if (!isset($this->modules[$area])) {
            $this->layout->setOneColumn();
            $this->layout->assignError($this->templateMainArea, 'Некорректный параметр <em>area</em> = <em>' . strip_tags((string) $area) . '</em>');
            $this->layout->assignTitle('Некорректный параметр area');
            return false;
        }

        return true;
    }

    public function runModules()
    {
        $this->initMenu();

        if (!$this->hasAdminModules()) {
            $this->showAuthForm();
            return;
        }

        $this->area = $this->request->getArea(current(array_keys($this->menu)));
        $this->fillLayout();

        if ($this->area == 'exit') {
            $this->session->invalidate(1);

            $response = Response::getInstance();
            $response->redirect(sprintf('action.phtml?area=%s&action=exit', $this->authArea));
        }

        if (!$this->checkArea($this->area)) {
            return;
        }

        $moduleDescription = $this->modules[$this->area];
        $instance = $moduleDescription->getModule();
        if ($instance instanceof ContainerAwareInterface) {
            $instance->setContainer($this->container);
        }

        $eventData = [
            'engine_type' => $this->engineType(),
            'engine_behavior' => $this->engineBehavior(),
        ];
        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
        }

        $instance->adminIndex();

        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
        }
    }

    public function html()
    {
        $response = Response::getInstance();
        $response->dontCache();

        $charset = $this->app->getProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);

        $this->db->Close();
        $this->layout->flush($charset);
    }

    /** {@inheritdoc} */
    public function engineBehavior()
    {
        return static::INDEX_BEHAVIOR;
    }
}
