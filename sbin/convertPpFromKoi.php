#!/usr/bin/php5 -q
<?

include_once 'Common/functions.inc';

$ppDir     = getFromArray($_SERVER['argv'], 1);
$charsetTo = getFromArray($_SERVER['argv'], 2);

if (empty($charsetTo)) {
	die("gimme charset\n");
}


if (is_dir($ppDir)) {
	convertDir($ppDir, $charsetTo);
} else {
	die("gimme dirname\n");
}


function convertDir($dirName, $charsetTo) {
	$d = dir($dirName);

	while ($e = $d->read()) {
		if ($e{0} == ".") continue;

		$f = "{$dirName}/{$e}";

		if (!is_readable($f)) {
			continue;
		}

		if (is_dir($f)) {
			convertDir($f, $charsetTo);

		} else {
			$type = mime_content_type($f);

			if (preg_match("/text.*charset=iso-8859-1/", $type)) {
				$fcontent = file_get_contents($f);
				$fcontent = myconv('k', $charsetTo, $fcontent);
				WriteStringToFile($f, $fcontent);
				echo $f.":".mime_content_type($f)."\n";
			}
		}
	}
}

?>