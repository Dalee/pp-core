<?php
/**
 * CSS minifier plugin for url compression
 *
 */
class CssConvertImageUrlMinifierPlugin extends aCssMinifierPlugin {

	private $reMatch = '/url\S*\([\"\']?(.+?)[\"\']?\)/';
	private $include = array (
			'background',
			'background-image',
			'src' // @font { src: url() }
		);

	/**
	 * Implements {@link aCssMinifierPlugin::minify()}.
	 * 
	 * @param aCssToken $token Token to process
	 * @return boolean Return TRUE to break the processing of this token; FALSE to continue
	 */
	public function apply(aCssToken &$token) {
		if (!in_array($token->Property, $this->include) || !preg_match($this->reMatch, $token->Value, $m) || false !== strpos($token->Value, 'data:')) {
			return false;
		}

		$path = $m[1];
		// resolve path to sourceFile if relative
		if ($path[0] !== '/') {
			$sourceFilePath = $this->configuration['sourceFile'];
			$re = sprintf('@^%s[^/]+/(htdocs/|blocks/)?@', preg_quote(BASEPATH, '@'));
			$to = '/';
			if (strpos($sourceFilePath, 'htdocs') === false) { // hack for .blocks
				$to = '/.blocks/';
			}
			$fullPath = realpath(dirname($sourceFilePath) . '/' . $path);
			$path = preg_replace($re, $to, $fullPath);
		}

		$imageTag = PXHtmlImageTag::getInstance();
		$result = $imageTag->buildProperty(array('src' => $path));

		$token->Value = str_replace($m[0], $result, $token->Value);
		return true;
	}

	/**
	 * Implements {@link aMinifierPlugin::getTriggerTokens()}
	 * 
	 * @return array
	 */
	public function getTriggerTokens() {
		return array (
			"CssRulesetDeclarationToken"
		);
	}
}

?>
