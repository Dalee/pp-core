<?

class PXSREventsCallback  {
	static function addToObjects($params) {
		list($app, $db) = array(PXRegistry::getApp(), PXRegistry::getDB());

		extract($params);

		$pattern = 'UPDATE %1$s SET %2$s = %2$s||\'%3$s\'::int WHERE %4$s';
		$regionalDatatypes = array_filter($app->types, array(__CLASS__, "filter"));

		$reg_mark = PXMultiRegions::REGION_MARK;

		foreach($regionalDatatypes as $dt) {
			$s = sprintf($pattern, $dt->id, $reg_mark, $to, $db->intersectIntArray($reg_mark, array($from)));
			$db->modifyingQuery($s);
		}

		return 'ok';
	}


	static function filter($dttype) {
		return isset($dttype->fields['sys_regions']);
	}
}

?>
