<?php
class PXUserNull extends PXUser {
	function can($what, $format, $object) {
		FatalError('Хм... А почему реальный юзер все еще не создан?');
	}
}
?>