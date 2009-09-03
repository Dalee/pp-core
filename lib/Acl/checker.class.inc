<?php
class CheckerObject {

	public static function get($format){
		if($format === NULL){
			$format =  new PXTypeDescription;
		}
					
		if(is_a($format, 'PXTypeDescription')) {
			return new ObjectChecker();
		} 
		else if(is_a($format, 'PXModuleDescription')) {
			return new ModuleChecker();
		}
		else {
			FatalError("This Acl type doesn't implements!");
		}
	}
}
?>