<?php

class PXTriggerDatabasePreview extends PXAbstractDatabaseTrigger
{
	public function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit)
	{
		$true_and_false = strtr($db->trueStatusString(true) . "|" . $db->trueStatusString(null), ["'" => ""]);

		if (PXRegistry::getApp()->isEngineInstanceof('Sbin')) {
			return;
		}

		if (PXRegistry::getRequest()->isSetVar(PXPluginPreview::ENABLE_PREVIEW_FLAG) && PXRegistry::getUser()->can(PXPluginPreview::PREVIEW_ACL_TYPE)) {
			PXRegistry::getResponse()->dontCache();

			$where = preg_replace($_1 = sprintf("#(%s.status\s*?=\s*?'(%s)')(\sand|or)?#i", $format->id, $true_and_false), '', $where);
			$where = preg_replace('#\(\)(\s*?and|or)?#i', '', $where);
			($where == '') && $where = null;
		}
	}
}

?>
