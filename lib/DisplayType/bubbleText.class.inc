<?php

class PXDisplayTypeBubbleText extends PXDisplayType
{
	static public $scriptsIncluded = false;

	public $bubbleTextShortContent = null;
	public $bubbleTextFullContent = null;

	public function buildCell($field, $object, $param = NULL)
	{
		$html = [];

		if (!self::$scriptsIncluded) {
			$html[] = $this->css();
			$html[] = $this->script();

			self::$scriptsIncluded = true;
		}

		$html[] = sprintf("<div class='fullBrief'>%s</div>", $this->fullContent($object, $field));
		$html[] = $this->shortContent($object, $field);

		return join($html);
	}

	public function shortContent($object, $field)
	{
		if ($this->bubbleTextShortContent) {
			return $this->bubbleTextShortContent;
		}

		return $object[$field->name];
	}

	public function fullContent($object, $field)
	{
		if ($this->bubbleTextFullContent) {
			return $this->bubbleTextFullContent;
		}

		return $object[$field->name];
	}

	public function script()
	{
		return <<<HTML
<script>
	$(function() {
		var fullBrief = $("div.fullBrief");

		var aTD = fullBrief.parent();
		aTD.css('cursor', 'pointer');

		aTD.bind("mouseenter", function() {
			$(this).children('div.fullBrief').show();
		});

		aTD.bind("mouseleave", function() {
			$(this).children('div.fullBrief').hide();
		});
	});
</script>
HTML;
	}

	public function css()
	{
		return <<<HTML
<style>
	div.fullBrief {
		position: absolute;
		z-index: 100;
		width: 400px;
		min-height: 20px;
		background: #FFFFE1;
		color: #000;
		border: 1px solid #000;
		padding: 10px;
		font-size: 110%;
		display:none;
	}
</style>
HTML;
	}
}
