<?
define('BACKTRACE', function_exists('debug_backtrace'));
define('NL_FATAL_ERROR', 16384);

class NLErrorReporter {

	static function GSC($varName, $action, $value=NULL) {
		static $count = 0;
		static $list = array();

		if(isset($$varName)) {
			switch($action) {
				case 'get':
					return $$varName;
					break;

				case 'set':
				default:
					$$varName = $value;
					break;
			}
		}
	}

	static function CatchError($errno=0, $errstr='', $errfile='', $errline=0, $errobj=NULL, $stripSecond=false) {
		static $errorTypes = array(
			E_ERROR           => 'Error',
			E_WARNING         => 'Warning',
			E_PARSE           => 'Parsing Error',
			E_NOTICE          => 'Notice',

			E_CORE_ERROR      => 'Core Error',
			E_CORE_WARNING    => 'Core Warning',

			E_COMPILE_ERROR   => 'Compile Error',
			E_COMPILE_WARNING => 'Compile Warning',

			E_USER_ERROR      => 'User Error',
			E_USER_WARNING    => 'User Warning',
			E_USER_NOTICE     => 'User Notice',

			NL_FATAL_ERROR    => 'Fatal Error',

			E_STRICT          => 'Runtime Notice'
		);

		if ($errno & (NL_FATAL_ERROR | error_reporting())) {
			$count = NLErrorReporter::GSC('count', 'get');
			$list  = NLErrorReporter::GSC('list', 'get');

			$error = array();

			$error['type'] = isset($errorTypes[$errno]) ? $errorTypes[$errno] : 'Unkown error type '.$errno;

			NLErrorReporter::TraceError($error,  $stripSecond);

			$error['text'] = "\n<br />\n* <b>" . $error['type'] . ":</b> $errstr in <b>$errfile</b> on line <b>$errline</b>\n<br />\n";

			if(!isset($_SERVER['REQUEST_METHOD'])) {
				$error['text'] = strip_tags($error['text']);
			}

			if(ini_get('display_errors')) {
				$id = mt_rand();

				if(isset($_SERVER['REQUEST_METHOD'])) {
					echo '<div class="error-debug">'.$error['text'];
					echo '<a href="" onclick="document.getElementById(\'error'.$id.'\').style.display=\'block\'; this.style.display = \'none\'; return false;">&darr;</a> ';
					echo '<a href="" onclick="this.parentNode.style.display = \'none\';return false;">x</a>';
					echo '<div id="error'.$id.'" style="display:none;">';
					echo NLErrorReporter::TraceTable($error, 'table');
					echo '</div></div>';
				} else {
					echo $error['text'];
				}
			}

			$list[$count] = $error;
			$count++;

			NLErrorReporter::GSC('count', 'set', $count);
			NLErrorReporter::GSC('list',  'set', $list);
			return true;
		}

		# Since PHP 5.2.0 we have to return false to populate $php_errormsg
		# This variable used in myiconv and some other functions
		return false;

	}

	static function TraceProcess() {
		if (!BACKTRACE) {
			return false;
		}

		$stack = debug_backtrace();
		array_shift($stack);

		$trace = array();

		foreach ($stack as $i => $call) {
			$e = array();

			$e['file'] = isset($call['file']) ? str_replace(BASEPATH, '', $call['file']) : NULL;
			$e['line'] = isset($call['file']) ? $call['line'] : NULL;

			$e['function'] = isset($call['class']) ? $call['class'].$call['type'] : '';
			$e['function'] .= $call['function'];

			$e['args'] = array();

			if(isset($call['args'])) {
				foreach ($call['args'] as $k=>$v) {
					$type = gettype($v);

					switch($type) {
						case 'resource':
							$value = get_resource_type($v);
							break;

						case 'array':
							$value = '['.sizeof($v).']';
							break;

						case 'object':
							$value = get_class($v);
							break;

						case 'boolean':
							$value = $v ? 'true' : 'false';
							break;

						case 'string':
							$value = "'$v'";
							break;

						case 'NULL':
							$value = 'NULL';
							break;

						default:
							$value = (string)$v;
							break;
					}

					$e['args'][$k] = array('type'=>$type, 'value'=>$value);
				}

			}

			$trace[] = $e;

		}

		return $trace;

	}

	static function TraceError(&$error, $stripSecond=false) {
		$stack = NLErrorReporter::TraceProcess();

		if(!is_array($stack)) {
			return false;
		}

		array_shift($stack);
		$first = array_shift($stack);

		if($stripSecond) {
			$first = array_shift($stack);
		}

		$error['file']  = isset($first['file']) ? $first['file'] : NULL;
		$error['line']  = isset($first['line']) ? $first['line'] : NULL;

		$error['debug'] = $stack;

		return true;
	}

	static function ErrorCount() {
		return NLErrorReporter::GSC('count', 'get');
	}

	static function ShutDown() {
		$count = NLErrorReporter::GSC('count', 'get');
		$list  = NLErrorReporter::GSC('list', 'get');

		if($count && !ini_get('display_errors')) {
			NLErrorReporter::SendMail();
		}

		exit();
	}

	static function SendMail() {
		$count = NLErrorReporter::GSC('count', 'get');
		$list  = NLErrorReporter::GSC('list', 'get');

		$th = 16;

		$r  = "\n";

		if(isset($_SERVER['REQUEST_METHOD'])) {
			$r .= "\n".str_pad('RequestUri:',  $th, ' ').'http://'.NLRequest::GetHttpHost().NLRequest::GetRequestUri();
			$r .= "\n".str_pad('RemoteAddr:',  $th, ' ').NLRequest::GetRemoteAddr();
			$r .= "\n".str_pad('HttpReferer:', $th, ' ').NLRequest::GetHttpReferer();

			$hr = "\n".str_repeat('_', 80)."\n";
		}

		$r .= $hr."\nОшибки\n";

		$errors = array('Fatal Error'=>0, 'Warning'=>0, 'Notice'=>0);

		foreach($list as $e) {
			$errors[$e['type']] = isset($errors[$e['type']]) ? $errors[$e['type']] + 1 : 1;
		}

		foreach($errors as $k=>$v) {
			if(!$v) {
				unset($errors[$k]);
			}
		}

		if(sizeof($list) > 32) {
			$r .= "\n>> Слишком много ошибок.";

			foreach($errors as $k=>$v) {
				$r .= "\n>>".str_pad('   '.$k, $th, ' ').$v;
			}

			$r .= "\n";

			$list = array_slice($list, 0, 32);
		}

		foreach($list as $k=>$e) {
			$r .= "\nER>".($k+1).strip_tags(str_replace("\n", ' ', $e['text']));

			foreach($e['debug'] as $e) {
				$r .= "\n";
				$r .= "\n".str_pad('   Файл:',      $th, ' ').$e['file'];
				$r .= "\n".str_pad('   Строка:',    $th, ' ').$e['line'];
				$r .= "\n".str_pad('   Функция:',   $th, ' ').$e['function'];
				$r .= "\n".str_pad('   Параметры:', $th, ' ');

				foreach($e['args'] as $k=>$v) {
					$r .= $v['value'];

					if($k+1!=sizeof($e['args'])) {
						$r .= ', ';
					}
				}

				$r .= "\n";
			}
		}

		$attach = array('_POST'=>NULL, '_GET'=>NULL, '_COOKIE'=>NULL, '_SESSION'=>NULL, '_SERVER'=>NULL, '_FILES'=>NULL);
		session_start();
		foreach($attach as $k=>$v) {
			if(isset($GLOBALS[$k]) && is_array($GLOBALS[$k]) && sizeof($GLOBALS[$k])) {
				$r .= $hr."\n$".$k."\n";

				foreach($GLOBALS[$k] as $kk=>$vv) {
					$r .= "\n".str_pad($kk, $th, ' ');
					$r .= trim(d3($vv,   false));
					$r .= "\n\n";
				}
			}
		}

		$toEmail = isset($_SERVER['SERVER_ADMIN']) ? $_SERVER['SERVER_ADMIN'] : 'support@dalee.ru';
		$replyTo = 'webmaster@'.$_SERVER['SERVER_NAME'];

		$computer = IS_WIN && isset($_ENV['COMPUTERNAME']) ? strtolower($_ENV['COMPUTERNAME']) : @rtrim(`hostname`);

		$from = array('name'=>'Error Reporter', 'email'=>'robot@'.$_SERVER['SERVER_NAME']);

		$subject = $computer.':';

		foreach($errors as $k=>$v) {
			$subject .= ' '.$k.' ('.$v.')';

		}

		$r = convert_cyr_string($r, 'k', 'w');

		return @mail($toEmail, $subject, $r, 'From: '.$from['name'].' <'.$from['email'].">\r\n".'Reply-To: '.$replyTo."\r\n");
	}

	static function TraceTable($error, $type='table') {
		$text = '';

		switch($type) {
			case 'table':
				$text .= '<table>';
				$text .= '<tr><th width="25%">Файл:</th><th width="10%">Строка:</th><th>Функция:</th><th>Параметры:</th></tr>';

				foreach ($error['debug'] as $e) {
					$text .= '<tr>';
					$text .= '<td>'.$e['file'].'</td>';
					$text .= '<td>'.$e['line'].'</td>';
					$text .= '<td>'.str_replace('->', '&rarr;', $e['function']).'</td><td>';

					foreach($e['args'] as $k=>$v) {
						$text .= ' <span class="'.$v['type'].'" title="'.$v['type'].'">'.$v['value'].'</span>';

						if($k+1!=sizeof($e['args'])) {
							$text .= ', ';
						}
					}
					$text .= '</td>';
					$text .= '</tr>';
				}

				$text .= '</table>';
				break;

			case 'html':
				foreach ($error['debug'] as $e) {
					$text .= "<br/>";
					$text .= "<br/> Файл: <strong>".$e['file']."</strong>";
					$text .= "<br/> Строка:\t <strong>".$e['line']."</strong>";
					$text .= "<br/> Функция:\t <strong>".$e['function']."</strong>";
					$text .= "<br/> Параметры:\t <strong>";

					foreach($e['args'] as $k=>$v) {
						$text .= $v['value'];

						if($k+1!=sizeof($e['args'])) {
							$text .= ', ';
						}
					}
					$text .= "</strong><br/>";
				}
				break;

			case 'text':
				foreach ($error['debug'] as $e) {
					$text .= "\n";
					$text .= "\n Файл:\t\t".$e['file'];
					$text .= "\n Строка:\t".$e['line'];
					$text .= "\n Функция:\t".$e['function'];
					$text .= "\n Параметры:\t";

					foreach($e['args'] as $k=>$v) {
						$text .= $v['value'];

						if($k+1!=sizeof($e['args'])) {
							$text .= ', ';
						}
					}
					$text .= "\n";
				}
				break;
		}

		return $text;
	}

	static function FatalError($errorMessage='', $errorType='') {
		$text = '';
		$error = array();

		NLErrorReporter::TraceError($error, true);

		$error['type'] = $errorType;
		$error['text'] = $errorMessage;

		$email = isset($_SERVER['SERVER_ADMIN']) ? $_SERVER['SERVER_ADMIN'] : 'support@netlogic.ru';

		if(!ini_get('display_errors')) {
			if(isset($_SERVER['REQUEST_METHOD'])) {
				$text .= '<html><head><title>Error</title></head><body>';
				$text .= '<h1>Error</h1>';
				$text .= '<p>Sorry, but '._StrToLower($errorType).' occured and your request can not be contented</p>';
				$text .= '<p>Please, inform <a href="mailto:'.$email.'">site administrator</a> about it.</p>';

				$text .= '<h1>Ошибка</h1>';
				$text .= '<p>Извините, произошла ошибка, ваш запрос не может быть корректно завершен.</p>';
				$text .= '<p>Пожалуйста, сообщите о ней <a href="mailto:'.$email.'">администратору сайта</a>.</p>';
				$text .= '</body></html>';

				if(IS_WIN) {
					$text = convert_cyr_string($text, 'k', 'w');
				}
			}

			NLErrorReporter::CatchError(NL_FATAL_ERROR, $error['text'], $error['file'], $error['line'], NULL, true);

		} else {
			if(isset($_SERVER['REQUEST_METHOD'])) {
				$text .= "<html><head><title>".$error['text']." &#8212; Fatal Error</title>
					<style>
						body {
							color: #000000;
							background-color: #FFFFFF;
							font: normal normal small Georgia;
						}

						table {
							width: 100%;
							border-collapse: collapse;
							border: 1px solid #000000;
						}

						th, td {
							border: 1px solid #000000;
							background-color: #FFFFFF;
							text-align: left;
							vertical-align: top;
							padding: 0.25em 0.5em;
						}

						th {
							font-size: x-small;
						}

						span {
							font-size: x-small;
							color: #000000
						}

						pre.desc {
							margin-left: 100px;
						}

						.string {color: #000066;}
						.integer {color: #660000;}
						.array {color: #006600;}
						.object {color: #666600;}
						.null {
							color: #666666;
						}
					</style>
				</head><body>";
				$text .= "<h1>Error</h1>";
				$text .= "<p>Sorry, but "._StrToLower($error['type'])." occured at <strong>".$error['file']."</strong>, <strong>".$error['line']."</strong>, and your request can not be contented</p>";
				$text .= "<p>Error description:</p>";
				$text .= "<pre class=\"desc\">{$error['text']}</pre>";

				if(isset($error['debug'])) {
					NLErrorReporter::TraceTable($error, 'table');
				}

				$text .= '</body></html>';

				if(IS_WIN) {
					$text = convert_cyr_string($text, 'k', 'w');
				}

			} else {
				$text = "\n\n\nERROR";
				$text .= "\nSorry, but "._StrToLower($error['type'])." occured at ".$error['file'].", ".$error['line'].", and your request can not be contented";
				$text .= "\nError description: ".$error['text'];

				if(isset($error['debug'])) {
					NLErrorReporter::TraceTable($error, 'text');
				}
				$text .= "\n\n";

				if(IS_WIN) {
					$text = convert_cyr_string($text, 'k', 'd');
				}
			}
		}

		echo $text;
	}
}

set_error_handler(array('NLErrorReporter', 'CatchError'));
register_shutdown_function(array('NLErrorReporter', 'ShutDown'));

?>
