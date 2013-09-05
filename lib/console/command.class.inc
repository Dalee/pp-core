<?php
/**
 * abstract command
 *
 * @category    Commands
 * @subpackage  console
 * @author      Alex Yaroshevich <zxqfox@gmail.com>
 */

//namespace console;

//use \console;

abstract class PXAbstractCommand {

	/** @var string $class name of scope */
	protected $class;

	/** @var string $name of scope */
	protected $name;

	/** @var array flags aliases */
	protected $flags = array();

	/** @var string $help short usage info, flags, options, etc. */
	protected $help;

	/** @var string full description of scope */
	protected $description;

	/**
	 * Constructor.
	 *
	 * @param string $name The name of the command
	 */
	public function __construct() {
		$this->configure();
		$this->class = get_called_class();
		!$this->name && $this->name = str_replace("commands\\", '', $this->class);
	}

	protected function configure() {
		PXConsole::use_aliases($this->flags);
	}

	public function _method($method) {
		$methods = $this->methods();
		return $methods[$this->name . ($method ? ':' . $method : '')];
	}

	protected function methods() {
		static $methods = array();

		$invokeRef = new \ReflectionMethod($this->class, '__invoke');
		$methods[$this->name] = new PXCommandMethodDescription('__invoke', $invokeRef, $this);

		foreach (get_class_methods($this) as $methodName) {
			$methodRef = new \ReflectionMethod($this->class, $methodName);
			if (!$methodRef->isPublic() || strpos($methodRef->getName(), '_') === 0) {
				continue;
			}

			$method = new PXCommandMethodDescription($methodName, $methodRef, $this);
			$methods[$this->name . ':' . $methodName] = $method;
		};

		return $methods;
	}

	public function __get($k) {
		if (isset($this->$k)) {
			return $this->$k;
		} elseif (is_callable(array($this, $k))) {
			return $this->$k();
		}
	}

	public function __invoke() {
		if (!empty($this->help)) {
			con_head("Help");
			con_text($this->help);
			con_eol();
		}

		con_head("Commands");
		con_list($this->methods());
		con_eol();

		con_head("Flags");
		con_list(array_flip($this->flags));
		con_eol();
	}

}
