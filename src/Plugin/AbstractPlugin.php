<?php

namespace PP\Plugin;

/**
 * TODO: should be abstract, but, it has some references
 *
 * Class AbstractPlugin
 * @package PP\Plugin
 */
class AbstractPlugin
{
    protected $name = null;

    /**
  * @param \PXApplication $app
  */
    public function __construct(public $app, public $description)
    {
        $this->name = $description->getName();
        $this->path = dirname((string) $this->description->getPathToPlugin());

        array_map($this->loadModule(...), $this->description->modules);
        array_map($this->loadTrigger(...), $this->description->triggers);

        $this->initialize($app);
    }

    public function initialize($app)
    {
    }

    public function initSet($params = null)
    {
    }

    public static function getParam($pluginName, $paramName)
    {
        return @\PXRegistry::getApp()->plugins[$pluginName]->params[$paramName];
    }

    public function loadTrigger($relativePath)
    {
        [$type, $name] = explode("/", (string) $relativePath);
        $this->app->registerTrigger($type, ["name" => $name] + ["folder" => $this->name]);
    }

    public function load($path, $pattern = "%s")
    {
        /** @noinspection PhpFormatFunctionParametersMismatchInspection */
        require_once sprintf("%s/{$pattern}", $this->path, $path);
    }

    public function loadWithLoader($folder, $classPrefix, $filename_without_ext, $extension = 'class.inc')
    {
        \PXLoader::getInstance("{$this->path}/{$folder}/")
            ->load("{$classPrefix}{$filename_without_ext}", "{$filename_without_ext}.{$extension}");
    }


    public function loadModule($relativePath)
    {
        $this->load($relativePath, "modules/%s.module.inc");
    }

    public function loadCronrun($relativePath)
    {
        $this->load($relativePath, "cronruns/%s.cronrun.inc");
    }

    public function loadDisplayType($filename_without_ext)
    {
        $this->loadWithLoader('displayTypes', 'PXDisplayType', $filename_without_ext);
    }

    public function loadStorageType($filename_without_ext)
    {
        $this->loadWithLoader('storageTypes', 'PXStorageType', $filename_without_ext);
    }

    public function loadOnlyInAdmin($path)
    {
        if ($this->app->isAdminEngine()) {
            $this->load($path);
        }
    }

    // what the hell is that?
    public static function autoload($className)
    {
        $f = \PXLoader::find($className);

        if (!strstr((string) $f, "/plugins/")) {
            return;
        }

        if (file_exists($f)) {
            require_once $f;

        } else {
            @unlinkDir(CACHE_PATH . "/config");
            @unlink(CACHE_PATH . "/loader");
        }
    }
}
