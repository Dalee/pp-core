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
			'src', /* @font { src: url() } */
		);

	/**
	 * Implements {@link aCssMinifierPlugin::minify()}.
	 * 
	 * @param aCssToken $token Token to process
	 * @return boolean Return TRUE to break the processing of this token; FALSE to continue
	 */
	public function apply(aCssToken &$token) {
		if (!in_array($token->Property, $this->include) || !preg_match($this->reMatch, $token->Value, $m)) {
			return false;
		}

		if (substr($m[1], 0, 1) === '.') {
			$re = sprintf('@^%s[^/]+/htdocs/@', preg_quote(BASEPATH, '@'));
			$m[1] = preg_replace($re, '/', realpath(dirname($this->configuration['sourceFile']).'/'.$m[1]));
		}

		$imageTag = PXHtmlImageTag::getInstance();
		$result = $imageTag->buildProperty(array('src' => $m[1]));

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