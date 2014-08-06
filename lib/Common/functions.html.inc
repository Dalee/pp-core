<?php
/**
 * HTML tags renderer
 *
 * @author    ya@dalee.ru
 * @namespace html
 */

/**
 * makes html doctype string
 * @param string $type one of html5, xhtml11, or (xhtml1|html4)-(strict|trans|frame). html5 by default
 * @return string
 * @test [] >>> "<!DOCTYPE html>"
 * @test ["html5"] >>> "<!DOCTYPE html>"
 * @test ["html4-strict"] >>> "<!DOCTYPE html \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"
 */
function html_doctype ($type = 'html5') {
	$aliases = array(
		'html4' => 'html4-strict',
		'xhtml' => 'xhtml1-strict'
	);
	$doctypes = array(
		'html4-strict'  => 'html "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd"',
		'html4-trans'   => 'html "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"',
		'html4-frame'   => 'html "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd"',
		'xhtml1-strict' => 'html "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"',
		'xhtml1-trans'  => 'html "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"',
		'xhtml1-frame'  => 'html "-//W3C//DTD XHTML 1.0 Frameset//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"',
		'xhtml11'       => 'html "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"',
		'html5'         => 'html',
	);

	if (isset($aliases[$type])) {
		$type = $aliases[$type];
	}

	$doctype = isset($doctypes[$type])? $doctypes[$type] : $doctypes['html5'];

	return "<!DOCTYPE {$doctype}>";
}

/**
 * html entities escaper
 * @param string $s raw text
 * @return string escaped text
 * @test ["\">&"] >>> "&quot;&gt;&amp;"
 */
function html_escape ($s) {
	return htmlentities($s, ENT_QUOTES, 'UTF-8');
}

/**
 * makes tag attributes string
 * @param array $atts
 * @return string
 */
function html_attributes ($atts) {
	$atts = (array)($atts);
	$out = '';


	foreach ($atts as $a => $v) {
		if ($a !== 'text' && $a !== 'html' && !is_null($v)) {
			$v = is_array($v)? join(" ", $v) : $v;
			$out .= ' ' . $a . '="' . html_escape($v) . '"';
		}
	}

	return trim($out);
}

/**
 * makes open tag string
 * @param string $el
 * @param array $atts
 * @return string
 */
function html_open_element ($tag, $atts = array()) {
	$tag = (string)($tag);
	$atts = html_attributes($atts);
	if ($atts) $atts = ' ' . $atts;
	return "<$tag$atts>";
}

/**
 * makes close element string
 * @param string $tag
 * @param string
 */
function html_close_element ($tag) {
	$tag = (string)($tag);
	return "</$tag>";
}

/**
 * makes
 * @param string $el Tag name
 * @param string [$html] Inner html content
 * @param array $atts Tag attributes
 * @return string
 * @test ["div", "vobla", {"class":"maharaja"}] >>> "<div class=\"maharaja\">vobla</div>"
 * @test ["div", {"class":"maharaja"}] >>> "<div class=\"maharaja\" />"
 */
function html_element ($el, $html, $atts = array()) {
	if (is_string($html)) {
		$atts['html'] = $html;
	}
	elseif (is_array($html)) {
		$atts = $html;
	}

	$text = isset($atts['text'])? html_escape($atts['text']) : '';
	$html = isset($atts['html'])? $atts['html'] : '';
	$html = is_array($html)? join($html) : $html;

	$inner = trim($text . $html);

	if (empty($inner)) {
		return html_empty_element($el, $atts);
	}

	$out  = html_open_element($el, $atts);
	$out .= $inner;
	$out .= html_close_element($el);

	return $out;
}

/**
 * makes empty element
 * @param string $el Tag name
 * @param array $atts
 * @return string
 * @test ["div", {"class":"maharaja"}] >>> "<div class=\"maharaja\" />"
 * @test ["img", {"src":"maharaja"}, true] >>> "<img src=\"maharaja\">"
 * @test ["br"] >>> "<br />"
 * @test ["br", true] >>> "<br>"
 */
function html_empty_element ($el, $atts = array(), $skipCloser = false) {
	$el = (string)($el);
	if (is_bool($atts)) {
		$skipCloser = $atts;
		$atts = null;
	}
	$atts = html_attributes($atts);
	return sprintf("<%s%s>", trim($el . ' ' . $atts), $skipCloser? '' : ' /');
}

/**
 * normalizes passed args with names
 * expect last parameter as array (or nothing)
 * @param array $args
 * @param array $names
 * @return array
 * @test [[{"D":"d"}], ["A","B","C"]] >>> {"D":"d"}
 * @test [["a","b",{"D":"d"}], ["A","B","C"], true] >>> {"D":"d","A":"a","B":"b","C":null}
 * @test [["//dalee.ru/~zx", "dzx"], ["href","text","class","title"]] >>> {"href":"//dalee.ru/~zx","text":"dzx"}
 * @test [["#1", "2", "a"], ["href","text","class","title"]] >>> {"href":"#1","text":"2","class":"a"}
 * @test [["#1", "2", ["a", "b"]], ["href","text","class","title"]] >>> {"href":"#1","text":"2","class":["a","b"]}
 * @test [[{"text":"2","class":"b","href":"#1"}], ["href","text","class","title"]] >>> {"text":"2","class":"b","href":"#1"}
 */
function _html_duck_params ($args, $names, $empties = false) {
	$atts = end($args);
	if (is_array($atts) && !empty($args[0]) && !isset($atts[0])) {
		$args[key($args)] = null;
	}
	else {
		$atts = array();
	}

	// drop invalid values
	foreach ($names as $k => $v) {
		if (!isset($args[$k]) || (is_array($args[$k]) && !isset($args[$k][0]))) {
			$args[$k] = null;
		}
	}
	// and combine hash
	$args = array_combine($names, array_splice($args, 0, 4));

	// collect return
	foreach ($args as $k => $v) {
		$atts[$k] = !empty($v)? $v : (isset($atts[$k])? $atts[$k] : null);
	}

	return $empties? $atts : array_filter($atts);
}

/**
 * make an A element
 * @param string [$href]
 * @param string [$text]
 * @param string [$class]
 * @param string [$title]
 * @param array $atts
 * @return string
 * @test ["//dalee.ru/~zx", "dzx"] >>> "<a href=\"//dalee.ru/~zx\">dzx</a>"
 * @test ["#1", "2", "a"] >>> "<a href=\"#1\" class=\"a\">2</a>"
 * @test ["#1", "2", ["a", "b"]] >>> "<a href=\"#1\" class=\"a b\">2</a>"
 * @test [{"text":"2","class":"b","href":"#1"}] >>> "<a class=\"b\" href=\"#1\">2</a>"
 */
function html_a ($href, $text = '', $class = '', $title = '', $atts = array()) {
	$atts = $atts + _html_duck_params(func_get_args(), array('href', 'text', 'class', 'title'));

	return html_element('a', $atts);
}

/**
 * make an element with content
 * @test ["span", "<img src=\"yolo\" />"] >>> "<span><img src=\"yolo\" /></span>"
 * @test ["div", {"html":"<br/>","class":"blabla"}] >>> "<div class=\"blabla\"><br/></div>"
 * @test ["div", {"class":"blabla"}, "<br>"] >>> "<div class=\"blabla\"><br></div>"
 * @test ["div"] >>> "<div />"
 */
function html_el ($tag, $atts = array(), $html = '') {
	if (is_string($atts)) {
		$html = $atts;
		$atts = array();
	}

	if (empty($atts) || !is_array($atts)) {
		$atts = array();
	}

	if (!empty($html)) {
		$atts['html'] = $html;
	}

	return html_element($tag, $atts);
}

/**
 * make a block
 * @test ["b1", "<img src=\"yolo\" />"] >>> "<div class=\"b1\"><img src=\"yolo\" /></div>"
 * @test ["b2", {"html":"<br/>","class":"blabla"}] >>> "<div class=\"b2 blabla\"><br/></div>"
 * @test ["b3"] >>> "<div class=\"b3\" />"
 */
function html_block ($name, $atts = array(), $html = '') {
	if (is_string($atts)) {
		$html = $atts;
		$atts = array();
	}

	if (empty($atts) || !is_array($atts)) {
		$atts = array();
	}

	if (!empty($html)) {
		$atts['html'] = $html;
	}

	// fetch tag if exists or use div
	$tag = isset($atts['tag'])? $atts['tag'] : 'div';
	unset($atts['tag']);

	// prepend block class
	$atts['class'] = !isset($atts['class'])? $name :
		(is_array($atts['class'])? array($name) + $atts['class'] : $name . ' ' . $atts['class']);

	return html_element($tag, $atts);
}

/**
 * br generator
 * @param string [$class]
 * @param array $atts
 * @return string html
 * @test [] >>> "<br>"
 * @test ["a"] >>> "<br class=\"a\">"
 * @test [["a","b","c"]] >>> "<br class=\"a b c\">"
 * @test [{"class":["a","b","c"]}] >>> "<br class=\"a b c\">"
 * @test ["a",{"class":["b"]}] >>> "<br class=\"a\">"
 * @test [["a","c"],{"class":["b"]}] >>> "<br class=\"a c\">"
 */
function html_br ($class = null, $atts = array()) {
	$atts = (array)$atts;

	// skip class if none passed
	if (is_array($class) && empty($atts) && !isset($class[0])) {
		$atts = $class;
	}
	elseif ($class) {
		$atts['class'] = $class;
	}

	return html_empty_element('br', $atts, true);
}

/**
 * img generator
 * @param string [$src]
 * @param string|array [$class]
 * @param string [$alt]
 * @param array $atts
 * @return string
 * @test [[]] >>> "<img src=\"\" alt=\"\">"
 * @test ["//dalee.ru/logo.png"] >>> "<img src=\"//dalee.ru/logo.png\" alt=\"\">"
 * @test ["/1.png",["q","w"]] >>> "<img src=\"/1.png\" class=\"q w\" alt=\"\">"
 */
function html_img ($src, $class = null, $alt = '', array $atts = array()) {
	$atts = $atts + _html_duck_params(func_get_args(), array('src', 'class', 'alt'), true);

	$atts['src'] = $atts['src'] ?: '';
	$atts['alt'] = $atts['alt'] ?: '';

	return html_empty_element('img', $atts, true);
}

/**
 * take on of passed params cycled
 * @param string|array $cycle bunch of comma delimited variants or a list with
 * @param string [$name] skipable optional parameter with unique name. null by default
 * @param number|bool [$iterate] pass here a number of iterations on this step or false to prevent changing iterator
 * @return string
 * @test ["a,b,c"] >>> "a"
 * @test ["a,b,c"] >>> "b"
 * @test ["a,b,c", false] >>> "c"
 * @test ["a,b,c"] >>> "c"
 * @test ["a,b", "q", 2] >>> "a"
 * @test ["a,b", "q", 2] >>> "a" --- once again
 * @test ["a,b", "q", 1] >>> "a"
 * @test ["a,b", 2] >>> "a"
 * @test ["a,b", 1] >>> "a"
 * @test ["a,b"] >>> "b" --- try2
 * @test ["a,b"] >>> "b" --- try3
 */
function html_cycle ($cycle, $name = null, $iterate = 1) {
	if (is_numeric($name) || is_bool($name)) {
		$iterate = $name;
		$name = null;
	}
	$iteratePassed = sizeof(func_get_args()) > 1 && $name === null;

	$items = is_array($cycle)? $cycle : explode(',', $cycle);
	$cycle = is_array($cycle)? join(',', $cycle) : $cycle;
	$name  = !is_null($name)? $name : md5($cycle);

	static $cycles;
	if (!isset($cycles[$name])) {
		$cycles[$name]['position'] = 0;
		$cycles[$name]['iterate'] = $iterate;
	}
	$current = $cycles[$name]['position'] % count($items);
	$iterate = $iteratePassed? $iterate : $cycles[$name]['iterate'];
	if ($iterate !== false) {
		$cycles[$name]['position'] += (int)($iterate);
	}

	return $items[$current];
}
