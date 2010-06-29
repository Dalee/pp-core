<?

class PXVersionsTab extends PXAdminWidget {
	function html() {
		$request = PXRegistry::getRequest();
		$module = @PXRegistry::getApp()->modules[$request->getArea()]; 

		if($module && $module->getModule() instanceof PXModuleObjects && $request->getId()) {
			$href = NLAbstractLayout::buildHref('action', 'contentversion');

			if(@PXRegistry::getApp()->types[$request->getFormat()]->fields['noversion']) {
				return "";
			}

			return sprintf("<li %s><a href='%s'>%s</a><b></b></li>", 
					($request->getAction() == "contentversion" ? "class='current'" : ""),
					$href,
					"Версии");
		}

		return "";
	}
}

?>
