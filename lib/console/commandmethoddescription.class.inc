<?php
/**
 * Command method description
 * Output helper class
 *
 * @category    Commands
 * @subpackage  console
 * @author      Alex Yaroshevich <zxqfox@gmail.com>
 * @copyright   2008-2012 Alex Yaroshevich
 * @license     http://www.opensource.org/licenses/mit-license.php The MIT License
 * @version     $Id: help 11.11.2011, 01:57:45\Z $
 */

//namespace console;

//use \console;

require_once (PPPATH . 'vendor/phpdoc/phpdoc.php');

class PXCommandMethodDescription {

	protected $name;
	protected $info;
	protected $reflection;
	protected $command;
	protected $commandReflection;

	public function __construct($name, \ReflectionMethod $methodRef, PXAbstractCommand $command) {
		$this->name = $name;
		$this->reflection = $methodRef;
		$this->command = $command;
		$this->commandReflection = new \ReflectionClass($command->class);

		if ($name == '__invoke') {
			$doccomment = $this->commandReflection->getDocComment();
		} else {
			$doccomment = $this->reflection->getDocComment();
		}

		$this->info = \phpdoc::fetchComment($doccomment);

/*		if (!empty($methodRef)) {
		foreach ($methodRef->getParameters() as $paramRef) {
			$param = array();
			$pname = $paramRef->getName();
			$param['optional'] = $paramRef->isOptional();
			$this->params[$pname] = $param;
		}
		}*/
	}

//	public function __set($k, $v) {
//		$this->params[$k] = $v;
//	}

	public function __get($k) {
		return isset($this->params[$k]) ? $this->params[$k] : null;
	}

	public function params($required_only = false) {
		$r = $this->reflection->getParameters();
		$required_only && $r = array_filter($r, function($p) { return !$p->isOptional(); });
		return $r;
	}

	public function __toString() {
		$r = '';
		if ($this->info) {
			$r .= $this->info . '. ';
		}

		$params = array();
		foreach ($this->params() as $p) {
			$s = '<' . $p->getName() . '>';
			$p->isDefaultValueAvailable() && $s = '[' . $s . ':' . var_export($p->getDefaultValue(), 1) . ']';
			$params[] = $s;

		}
		if (!empty($params)) {
			$r .= 'Params: ';
			$r .= join(' ', $params);
		}

		return $r;
	}
}
