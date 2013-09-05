<?php

if (!is_callable('get_class_constants')) {
	function get_class_constants($class) {
		$class = new ReflectionClass(is_string($class) ? $class : get_class($class));
		return $class->getConstants();
	}
}

if (class_exists("phpdoc")) {
	return;
}

class phpdoc {

	const T_UNKNOWN  =   0x00;
	const T_FLAG     =   0x01; // @static, @final, @deprecated
	const T_STRING   =   0x02;
	const T_PATH     =   0x04;
	const T_URL      =   0x08;
	const T_TYPE     =   0x10; // some kind of type: integer|string|mixed|className|\namespace\className etc.
	const T_NAME     =   0x20; // some kind of name: class::method(), function(), className etc.
	const T_VAR      =   0x40; // $var|$array["var"] etc.
	const T_DOCPATH  =   0x80; // package[/subpackage[#section[.subsection]]]
	const T_DESC     =  0x100; // any string after any other
	const T_ACCESS   =  0x200; // private|protected|public
	const T_MULTI    = 0x1000; // flag for multiprops

	protected static $tag_parts = array(
		self::T_ACCESS  => array('list' => array('private', 'protected', 'public')),
		self::T_PATH    => array('part' => 'path', 'regexp' => '/^([\da-z_\.-\/]+)$/i'),
		self::T_URL     => array('part' => 'url',  'regexp' => '/^([a-z]+:)?(\/\/)?([a-z\d_]+@)?([\da-z\.-]+)\.([a-z\.]{2,6})([\/\w \.-]*)*\/?$/i'),
		self::T_VAR     => array('part' => 'name', 'regexp' => '/^&?\$[a-z0-9_]+(\[\"[^\"]+\"\])*$/i'),
		self::T_NAME    => array('part' => 'name', 'regexp' => '/^\$[a-z0-9_]+(\[\"[^\"]+\"\])*|[\\a-z0-9_]+(::[\\a-z0-9_]+)?(\(\s*\))?$/i'),
		self::T_TYPE    => array('part' => 'type', 'regexp' => '/^[\\a-z0-9_\|]+$/i'),
		self::T_DOCPATH => array('part' => 'path', 'regexp' => '/^[a-z0-9\-_]+(\/[a-z0-9\-_]+(#[a-z0-9\-_]+(\.[a-z0-9\-_]+)?)?)?$/i'),
	);

	// mixes
	const T_GLOBAL    = 0x0050; // T_TYPE | T_VAR
	const T_USES      = 0x1120; // T_NAME | T_DESC | T_MULTI
	const T_LINK      = 0x112c; // T_URL  | T_PATH | T_NAME | T_DESC
	const T_VARDEF    = 0x0110; // T_TYPE | T_DESC
	const T_PARAM     = 0x1150; // T_VAR  | T_TYPE | T_DESC | T_MULTI
	const T_METHOD    = 0x1130; // T_TYPE | T_NAME | T_DESC | T_MULTI
	const T_TUTORIAL  = 0x1180; // T_DOCPATH | T_DESC | T_MULTI
	const T_MSTRING   = 0x1002; // T_STRING | T_MULTI
	const T_ALL       = 0x02ff; // *

	protected static $tags = array(
		"abstract"    => self::T_FLAG,
		"access"      => self::T_ACCESS,
		"author"      => self::T_MSTRING,
		"category"    => self::T_MSTRING,
		"comment"     => self::T_UNKNOWN,
		"copyright"   => self::T_STRING,
		"deprecated"  => self::T_FLAG,
		"example"     => self::T_MSTRING,
		"final"       => self::T_FLAG,
		"filesource"  => self::T_FLAG,
		"global"      => self::T_GLOBAL,
		"ignore"      => self::T_FLAG,
		"internal"    => self::T_MSTRING,
		"license"     => self::T_MSTRING,
		"link"        => self::T_LINK, // url | path | name | desc | multi
		"method"      => self::T_METHOD,
		"name"        => self::T_VAR,
		"package"     => self::T_NAME,
		"param"       => self::T_PARAM, // var | type | desc | multi
		"property"    => self::T_PARAM,
		"property-read"  => self::T_PARAM,
		"property-write" => self::T_PARAM,
		"return"      => self::T_VARDEF, // type | desc
		"see"         => self::T_USES,
		"since"       => self::T_STRING,
		"static"      => self::T_FLAG,
		"staticvar"   => self::T_VARDEF, // type | desc
		"subpackage"  => self::T_NAME, // A SUBPACKAGE NAME MUST BE ONE WORD WITH NO SPACES OR NEWLINES CONTAINING ONLY LETTERS, DIGITS, and "_", "-", "[" or "]"
		"todo"        => self::T_MSTRING,
		"tutorial"    => self::T_TUTORIAL,
		"uses"        => self::T_USES,
		"var"         => self::T_VARDEF, // type | desc
		"version"     => self::T_STRING,
	);

	protected $file;
	protected $content;

	/**
	 * Get tag name by number
	 * @warning For debug purposes only!
	 * @param integer $i value of type constant
	 * @return string
	 * @ignore
	 */
	public static function typename($i) {
		static $consts = array();
		if (empty($consts)) {
			$consts = get_class_constants(__CLASS__);
		}
		return array_search($i, $consts);
	}

	/**
	 * Constructor
	 *
	 * @access public
	 * @param string $file the file to scan
	 * @return phpdoc
	 */
	public function __construct($file) {
		$this->file = $file;
		$this->content = file_get_contents($this->file);
		/*$handle = fopen($this->file, "r");
		$length = filesize($this->file);
		$this->content = fread($handle, $length);
		fclose($handle);*/
	}

	/**
	 * Get the classes name in the file
	 *
	 * @access private
	 * @return array
	 */
	static public function parse($file = null) {
		$class = __CLASS__;
		$self = isset($this) ? $this : new $class($file);
		$matches = array();

		$tokens = token_get_all($self->content);
		$class_token = false;

		foreach ($tokens as $token) {
			if (!is_array($token)) {
				continue;
			}
			if ($token[0] == T_CLASS) {
				$class_token = true;
			} else if ($class_token && $token[0] == T_STRING) {
				$class_token = false;
				//FOUND
				$matches[] = $token[1];
			}
		}

		$c = array();

		foreach ($matches as $id => $cl) {
			if (!class_exists($cl)) {
				require_once($file);
			}
			if (!class_exists($cl)) {
				continue;
			}

			$reflector = new ReflectionClass($cl);
			$methods = array_map(function($m){ return $m->getName(); }, $reflector->getMethods());

			$desc = $reflector->getDocComment();

			$m = array();

			foreach ($methods as $method) {
				$gm = $reflector->getMethod($method);
				$parameters = $gm->getParameters();
				$comment = $gm->getDocComment();

				$dq = $self->parseComments($comment);
				$d = $self->parseParameters($dq, $parameters);

				$gm->isConstructor() && $method = '__construct';
				$gm->isDestructor() && $method = '__destruct';

				$gm->isDeprecated() && $d['deprecated'] = true;
				$gm->isFinal() && $d['final'] = true;
				$gm->isAbstract() && $d['abstract'] = true;
				$gm->isStatic() && $d['static'] = true;

				empty($d['access']) && $d['access'] = $gm->isProtected() ? 'protected' : ($gm->isPrivate() ? 'private' : 'public');

				$m = array_merge($m, array($method => $d));
			}

			$c = array_merge($c, array($cl => array('methods'=>$m, 'desc' => $self->parseComments($desc))));
		}

		return $c;
	}

	/**
	 * Parse comments
	 *
	 * @access private
	 * @param string $comment the comment of each method
	 * @return array a array of string or of array which contains the formated comment and arguments
	 */
	private function parseComments($doccomment) {
		$lines   = explode("\n", str_replace("\r", "", $doccomment));
		$result  = array();
		$result['comment'] = array();

		$param_position = 0;

		$prev_tag = 'comment';
		foreach ($lines as $line) {
			if (!preg_match("/^\s*\*([^\/].*)$/", trim($line), $matches)) {
				continue;
			}

			$l = trim($matches[1]);
			if (($l[0] === '@' || $l[0] === '!') && ctype_alpha($l[1])) {
				$tag = ltrim(string_shift($l), '!@');
				$prev_tag = $tag;
				$content = $l;
			} else {
				// multistrings
				$tag = $prev_tag;
				$content = $l;
			}
			$content = trim(str_replace("\t", " ", $content));
			while (strpos($content, "  ") !== false) {
				$content = str_replace("  ", " ", $content);
			}

			$tag_flags = isset(static::$tags[$tag]) ? static::$tags[$tag] : static::T_UNKNOWN;
			$marks = array();
			if ($tag_flags > 0) { // try to parse bits to array of flags
				for ($i = 1; $i <= self::T_ALL; $i <<= 1) {
					if ($tag_flags & $i) {
						$marks[] = $i;
					}
				}
			}

			if (empty($marks)) {
				$marks[] = $tag_flags; // itself by default
			}

			$parts = array();

			if ($tag_flags == self::T_PARAM) {
				$param_position ++;
				$parts['position'] = $param_position;
			}

			reset($marks);
			while (($i = current($marks)) !== false) {
				switch ($i) {
					case self::T_FLAG:
						$parts = true;
						break;
					case self::T_ACCESS: // private|protected|public
						if (in_array($content, self::$tag_parts[$i]['list'])) {
							$parts = $content;
						}
						break;
					case self::T_PATH:
					case self::T_URL:
					case self::T_VAR:     // $var|$array["var"] etc.
					case self::T_NAME:    // some kind of name: class::method(), function(), className etc.
					case self::T_TYPE:    // some kind of type: integer|string|mixed|className|\namespace\className etc.
					case self::T_DOCPATH: // package[/subpackage[#section[.subsection]]]
						$str = string_shift($content);
						if (empty($str) || ctype_graph($str) && preg_match(self::$tag_parts[$i]['regexp'], $str)) {
							$parts[self::$tag_parts[$i]['part']] = $str ?: '';
						} else {
							echo 'woops? fixit: '.self::typename($i) . ' "' . $str . '" in "' . $line . '" -- ' . $this->file . PHP_EOL;
							break;
							string_unshift($content, $str);
							array_push($marks, $i);
						}
						break;
					case self::T_DESC: // any string after any other
						$parts['desc'] = $content;
						break;
					case self::T_STRING:
					default:
						$parts = $content;
						break;
				}
				next($marks);
			}

			if ($tag_flags & self::T_MULTI || $tag_flags === self::T_UNKNOWN) {
				if (empty($result[$tag])) {
					$result[$tag] = array();
				}
				$result[$tag][] = $parts;
			} else {
				$result[$tag] = $parts;
			}
		}

		if (!empty($result['comment'])) {
			$result['comment'] = join("\n", $result['comment']);
		} else {
			$result['comment'] = '';
		}

		return $result;
	}
	
	/**
	 * Add Missing Parameters to the parsed Comment
	 *
	 * @access private
	 * @param array $parsedComment the array returned by the parseComments function
	 * @param array $params an array of object containing parameters of the function
	 * @return void
	 */
	private function parseParameters($parsedComment, $params) {
		$array_params = array();

		foreach ($params as $p) {
			$param = array(
				'name' => $p->getName(),
				'position' => $p->getPosition(),
				'desc' => null,
				'type' => null,
			);

			if ($p->isDefaultValueAvailable()) {
				$param['default'] = $p->getDefaultValue();
			}

			if ($p->isArray()) {
				$param['type'] = 'array';
			}
			try {
				if ($p->getClass()) {
					$param['type'] = $p->getClass()->getName();
				}
			} catch (ReflectionException $e) {
				if (strpos($e->getMessage(), 'Class callback does not exist') !== false) {
					$param['type'] = 'callback';
				}
			}

			$param['source_href'] = $p->getFileName() . ':' . $p->getStartLine() . ':' . $p->getEndLine();

			$array_params[] = $param;
		}

		if (!empty($parsedComment['param'])) {
			foreach ($parsedComment['param'] as $param) {
				foreach ($array_params as $i => $ch) {
					if (@$param['name'] == '$' . $ch['name'] || empty($param['name']) && @$param['position'] === $ch['position']) {
						!empty($param['type']) && $array_params[$i]['type'] = $param['type'];
						!empty($param['desc']) && $array_params[$i]['desc'] = $param['desc'];
					}
				}
			}
		}

		$parsedComment['param'] = $array_params;

		return $parsedComment;
	}

	public static function fetchComment($doccomment) {
		$lines = explode("\n", str_replace("\r", "", $doccomment));
		foreach ($lines as $line) {
			if (trim($line, '\r\n\t /*') == '') {
				continue;
			}
			if (preg_match("/^\s*\*([^\/].*)$/", trim($line), $matches)) {
				return trim($matches[1]);
			}
			return $line;
		}
	}

}

