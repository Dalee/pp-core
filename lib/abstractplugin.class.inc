<?php
class PXAbstractPlugin {
	protected $name = null;
	protected $description;

	function __construct($app, $description) {
		$this->description = $description;
		$this->name = $description->getName();
		$this->registerComponents($app);
		$this->initialize($app);
	}

	function initialize($app) {
	}

	function initSet($params = null) {
	}

	private function registerComponents($app) {
		$fileName = BASEPATH . 'libpp/plugins/' . $this->name . '/config.xml';
		if(!file_exists($fileName)) {
			return;
		}

		if (!($dom = PXml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domComs = $dom->xpath("/components//component");
		if (!is_array($domComs) || count($domComs) == 0) {
			FatalError("Не описаны компоненты плагина");
		}

		foreach($domComs as $domCom) {
			$attrs = $domCom->attributes();
			foreach($attrs as $item) {
				${$item->name} = utf8_decode($item->value);
			}
			if(!isset($type)) {
				FatelError("Укажите тип компонента");
			}
			$type = 'register' . ucfirst($type);
			$this->$type($domCom->childNodes(), $app);
		}
	}

	private function registerTrigger($nodes, $app) {
		$args = array();
		foreach($nodes as $k => $node) {
			$attrs = $node->attributes();
			foreach($attrs as $attr) {
				$args[$attr->name] = $attr->value;
			}
		}
		if(!isset($args['type'])) {
			FatalError('Укажите тип trigger');
		}
		$type = $args['type'];
		unset($args['type']);
		if(empty($args)) {
			FatalError('Укажите тип trigger');
		}
		$args['folder'] = $this->name;

		$app->registerTrigger($type, $args);
	}
}
?>
