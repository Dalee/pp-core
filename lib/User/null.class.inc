<?php
class PXUserNull extends PXUser {
	function can($what, $format = NULL, $object = NULL) {
		FatalError('Хм... А почему реальный юзер все еще не создан?');
	}
}
?>