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

	$inner  = isset($atts['text']) ? html_escape($atts['text']) : '';
	$inner .= isset($atts['html']) ? $atts['html'] : '';

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
 * @test ["br"] >>> "<br />"
 */
function html_empty_element ($el, $atts = array()) {
	$el = (string)($el);
	$atts = html_attributes($atts);
	return sprintf("<%s />", trim($el . ' ' . $atts));
}

/**
 * @param string [$href]
 * @param string [$text]
 * @param string [$class]
 * @param string [$title]
 * @param array $atts
 * @param array normalized attributes array
 * @test ["//dalee.ru/~zx", "dzx"] >>> {"href":"//dalee.ru/~zx","text":"dzx"}
 * @test ["#1", "2", "a"] >>> {"href":"#1","text":"2","class":"a"}
 * @test ["#1", "2", ["a", "b"]] >>> {"href":"#1","text":"2","class":"a b"}
 * @test [{"text":"2","class":"b","href":"#1"}] >>> {"text":"2","class":"b","href":"#1"}
 */
function _html_a_params ($href, $text = '', $class = '', $title = '', $atts = array()) {
	$args = func_get_args();
	$args_index = array('href', 'text', 'class', 'title');

	$atts = end($args);
	if (is_array($atts) && !isset($atts[0])) {
		$args[key($args)] = null;
	} else {
		$atts = array();
	}

	foreach ($args_index as $k => $v) {
		if (!isset($args[$k]) || (is_array($args[$k]) && !isset($args[$k][0]))) {
			$args[$k] = null;
		}
	}
	$args = array_combine($args_index, array_splice($args, 0, 4));

	foreach ($args as $k => $v) {
		$atts[$k] = !empty($v) ? $v : (isset($atts[$k]) ? $atts[$k] : null);
	}

	if (isset($atts['class']) && !empty($atts['class'])) {
		$atts['class'] = is_array($atts['class']) ? join(' ', $atts['class']) : (string)($atts['class']);
	}

	return array_filter($atts);
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
	$atts = call_user_func_array('_html_a_params', func_get_args());
	return html_element('a', $atts);
}

/**
 * make an element with content
 * @test ["span", "<img src=\"yolo\" />"] >>> "<span><img src=\"yolo\" /></span>"
 * @test ["div", {"html":"<br/>","class":"blabla"}] >>> "<div class=\"blabla\"><br/></div>"
 */
function html_el ($tag, $html, $atts = array()) {
	if (empty($atts) || !is_array($atts)) {
		$atts = array();
	}

	if (is_array($html)) {
		$atts = $html;
	}
	else {
		$atts['html'] = $html;
	}

	return html_element($tag, $atts);
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
