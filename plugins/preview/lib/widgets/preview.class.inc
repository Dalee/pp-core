<?php

class PXAdminWidgetPreview extends PXAdminWidget {
	function html() {
		$req = PXRegistry::getRequest();
		$app = PXRegistry::getApp();

		list($format, $id) = array($req->getVar('format'), (int) $req->getVar('id'));

		if (!isset($app->types[$format]) 
			|| $app->types[$format]->parent != 'struct'
			|| $id < 1
			|| !PXRegistry::getUser()->can(PXPluginPreview::PREVIEW_ACL_TYPE)
		) {
			return '';
		}

		$link   = "/admin/?area=adminpreview&format={$req->getVar('format')}&id={$req->getVar("id")}";
		$link   = "window.open('{$link}','mywindow','resizable=yes,location=yes,scrollbars=yes,status=yes,menubar=yes');";

		$html[] = "<button type='button' onclick=\"{$link}\">Предпросмотр</button>";
		return join($html);
	}
}

?>
