<?php
final class PXTriggersList extends ArrayObject {
	function __get($triggerType) {
		return empty($this[$triggerType]) || !is_array($this[$triggerType]) ? array() : $this[$triggerType];
	}
}
?>
