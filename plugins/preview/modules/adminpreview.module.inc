<?php

use PP\Module\AbstractModule;

class PXModuleAdminPreview extends AbstractModule
{
    public function adminIndex()
    {
        $this->response->dontCache();

        $this->response->setContentType('text/html', CHARSET_UTF8);

        $format = $this->request->getVar('format');
        $id = intval($this->request->getVar('id'));

        if (!isset($this->app->types[$format]) || $id < 1) {
            return;
        }

        $format = $this->app->types[$format];

        if ($format->parent != 'struct') {
            return;
        }

        $object = $this->db->getObjectsByField($format, null, 'id', $id);
        $object = reset($object);

        if (empty($object)) {
            return;
        }

        $tree = PXRegistry::getStruct();
        $path = createPathByParentId($tree, is_null($object['parent']) ? $object['id'] : $object['parent']);

        if ($parsed_url = parse_url($path)) {
            if (empty($parsed_url['host'])) {
                $path = sprintf("http://%s%s", $this->request->getHttpHost(), $path);
            } else {
                $grouped_aliases = [];

                foreach ($this->app->modules['main']->settings['domainAlias'] as $host) {
                    [$alias, $path] = explode("=", strtr($host, [" " => ""]));

                    if ($parsed_url['host'] == $path) {
                        $eng = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');
                        $httpHost = strtr($this->request->getHttpHost(), [$eng . '.' => '']);

                        if (strstr($alias, $eng)) {
                            $alias = strtr($alias, [$eng . "." => ""]);
                            $is_eng = 1;
                        }

                        $hostPattern = '/^' . str_replace('*', '([a-z0-9\-]+)', trim($alias)) . '$/i';

                        if (preg_match($hostPattern, $httpHost)) {
                            $parsed_url['host'] = sprintf("%s%s", !empty($is_eng) ? $eng . '.' : '', $httpHost);
                            $path = 'http://' . strtr(join("/", array_slice($parsed_url, 1)), ["//" => "/"]);
                            break;
                        }
                    }
                }
            }
        }

        if (!is_null($object['parent']) && !empty($object['pathname'])) {
            $path = sprintf("%s%s", $path, $object['pathname']);
        }

        $this->response->redirect($path = appendParamToUrl($path, PXPluginPreview::ENABLE_PREVIEW_FLAG, '1'));
        exit;
    }
}

?>
