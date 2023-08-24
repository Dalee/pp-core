<?php

namespace PP\Module;

/**
 * Class MassChangeModule.
 *
 * @package PP\Module
 */
class MassChangeModule extends AbstractModule
{
    protected $helper;

    public function adminJson()
    {
        $this->helper = new \stdClass();
        $this->helper->objectType = $this->request->getVar('format');
        $this->helper->operation = preg_replace("#[^a-z0-9_-]#i", '', $this->request->getVar('handler'));
        $this->helper->options = $this->request->getVar('options');

        if (!empty($this->helper->options)) {
            $this->helper->options = json_decode($this->helper->options);
        }

        $this->helper->options = is_object($this->helper->options) ? $this->helper->options : null;
        $this->helper->objectIds = (array)$this->request->getVar('objects', []);
        $this->helper->objectIds = array_filter($this->helper->objectIds, 'is_numeric');

        if (!(isset($this->app->types[$this->helper->objectType]) || count($this->helper->objectIds))) {
            FatalError('Malformed action params');
        }

        return $this->doOperation();
    }

    /**
     * @return null|object
     */
    protected function doChangeParent()
    {
        $dtype = $this->app->types[$this->helper->objectType];
        $parentField = isset($dtype->fields['parent'])
            ? 'parent'
            : (isset($dtype->fields['pid']) ? 'pid' : null);

        $newParent = !empty($this->helper->options->parent) && is_numeric($this->helper->options->parent)
            ? $this->helper->options->parent
            : 0;

        if ($newParent && $parentField) {
            foreach ($this->helper->objectIds as $id) {
                $object = $this->db->GetObjectById($dtype, $id);
                if (!$object || $object[$parentField] == $newParent) {
                    continue;
                }

                $object[$parentField] = $newParent;
                $this->db->ModifyContentObject($dtype, $object);
            }

            return null;
        }

        return \PXEngineJSON::toError('Недопустимый родитель объекта');
    }

    /**
     * @return object
     */
    protected function doCommonMultipleDelete()
    {
        foreach ($this->helper->objectIds as $objectId) {
            $this->db->DeleteContentObject($this->app->types[$this->helper->objectType], $objectId);
        }

        $cnt = count($this->helper->objectIds);
        $res = sprintf(
            '%s %d %s',
            NumericEndingsRussian($cnt, 'удалён', 'удалено', 'удалено'),
            $cnt,
            NumericEndingsRussian($cnt, 'объект', 'объекта', 'объектов')
        );

        return \PXEngineJSON::toSuccess($res);
    }

    /**
     * @return null|object
     */
    protected function doCommonMultipleStatusChange()
    {
        $dtype = $this->app->types[$this->helper->objectType];
        $states = ['true' => true, 'false' => false];
        $status = (isset($this->helper->options->status) && in_array($this->helper->options->status, ['true', 'false']))
            ? $this->helper->options->status
            : null;

        if (isset($states[$status])) {
            $status = $states[$status];
            foreach ($this->helper->objectIds as $id) {
                $object = $this->db->GetObjectById($dtype, $id);
                if (!$object || $object['status'] == $status) {
                    continue;
                }

                $object['status'] = $status;
                $this->db->ModifyContentObject($dtype, $object);
            }

            return null;
        }

        return \PXEngineJSON::toError('Недопустимый статус объекта');
    }

    /**
     * @return object
     */
    protected function doOperation()
    {
        $operationName = strtolower($this->helper->operation);
        $result = null;

        // operation names defined in lib/HTML/Admin/Widgets/Multiops/*.class.inc
        switch ($operationName) {
            case 'docommonmultiplestatuschange':
                $result = $this->doCommonMultipleStatusChange();
                break;

            case 'dochangeparent':
                $result = $this->doChangeParent();
                break;

            case 'docommonmultipledelete':
                $result = $this->doCommonMultipleDelete();
                break;
        }

        $result = ($result === null)
            ? (object)$result
            : $result;

        if (isset($result->redirect)) {
            $this->response->redirect($result->redirect);
        }

        return $result;
    }

}
