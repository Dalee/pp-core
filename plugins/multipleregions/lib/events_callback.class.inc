<?

class PXSREventsCallback
{
	public static function addToObjects($params)
	{
		[$app, $db] = [PXRegistry::getApp(), PXRegistry::getDB()];

		$to = $from = null;
		extract($params);

		$pattern = 'UPDATE %1$s SET %2$s = %2$s||\'%3$s\'::int WHERE %4$s';
		$regionalDatatypes = array_filter($app->types, [__CLASS__, "filter"]);

		$reg_mark = PXMultiRegions::REGION_MARK;

		foreach ($regionalDatatypes as $dt) {
			$s = sprintf($pattern, $dt->id, $reg_mark, $to, $db->intersectIntArray($reg_mark, [$from]));
			$db->modifyingQuery($s);
		}

		return 'ok';
	}


	public static function filter($dttype)
	{
		return isset($dttype->fields['sys_regions']);
	}
}

?>
