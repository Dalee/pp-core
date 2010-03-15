<?php
	class PXModuleMultiopsMasschange extends PXModule{
		private 
			$helper;
		
		public 
			function adminAction(){
				$this->settings['helpers'] = @array_merge((array)$this->settings['helpers'], array(dirname(__FILE__) . '/Helpers/'));
				$this->helper->objectType  = $this->request->getVar('format');
				$this->helper->operation   = preg_replace("#[^a-z0-9_-]#i", '', $this->request->getVar('handler'));
				$this->helper->options     = $this->request->getVar('options');
				
				if(!empty($this->helper->options)){
					$this->helper->options = $this->getJSON()->unserialize($this->helper->options);
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
			
			private function getJSON(){
				require_once 'JSON/json.class.inc';
				static $instance;
				
				return $instance ? $instance : new JSON;
			}
		
			private function route($res){
				if(isset($res['redirect'])){
					return $res['redirect'];
				}
				
				$this->response->send($this->getJSON()->serialize((object)$res));
				exit;
			}
	}
?>