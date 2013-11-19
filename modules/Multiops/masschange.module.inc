<?php
	class PXModuleMultiopsMasschange extends PXModule{
		private
			$helper;
		
		public
			function adminJson(){
				$this->settings['helpers'] = @array_merge((array)(BASEPATH . $this->settings['helpers']), array(dirname(__FILE__) . '/Helpers/'));
				$this->helper = new stdClass();
				$this->helper->objectType  = $this->request->getVar('format');
				$this->helper->operation   = preg_replace("#[^a-z0-9_-]#i", '', $this->request->getVar('handler'));
				$this->helper->options     = $this->request->getVar('options');
				
				if(!empty($this->helper->options)){
					$this->helper->options = json_decode($this->helper->options);
				}
				
				$this->helper->options   = is_object($this->helper->options) ? $this->helper->options : null;
				$this->helper->objectIds = (array)$this->request->getVar('objects', array());
				
				if(!(
						isset($this->app->types[$this->helper->objectType]) ||
						sizeof($this->helper->objectIds = array_filter($this->helper->objectIds, 'is_numeric'))
					)
				){
					FatalError('Malformed action params');
				}
				return $this->makeOperation();
			}
		
		private
			function makeOperation(){
				$RESULT = null;
				
				foreach($this->settings['helpers'] as $path){
					if(file_exists($helper_file = $path . strtolower($this->helper->operation) . '.inc')){
						include $helper_file;
						break;
					}
				}
				
				return $this->route($RESULT);
			}
			
			private function route($res){
				if(isset($res->redirect)){
					$this->response->redirect($res->redirect);
				}
				
				return (object)$res;
			}
	}
?>