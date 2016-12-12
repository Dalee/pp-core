<?php

namespace PP\Properties;

/**
 * Class PropertyLoader
 * @package PP\Properties
 */
class PropertyLoader {

	/**
	 * Load all properties as key => value.
	 *
	 * @param \PXDatabase|\NLPGSQLDatabase $database
	 * @return array
	 * @internal should be used only in PXApplication context
	 */
	public static function getPropertyList($database) {
		$loadSql = sprintf(
			'SELECT "name", "value" FROM %s',
			DT_PROPERTIES
		);

		$propertyListRaw = $database->Query($loadSql);
		return array_flat($propertyListRaw, 'name', 'value');
	}

	/**
	 * Load raw property list.
	 *
	 * @param \PXDatabase|\NLPGSQLDatabase $database
	 * @return array
	 * @internal should be used only in properties.module.inc
	 */
	public static function getRawPropertyList($database) {
		$loadSql = sprintf(
			'SELECT id, "name", description, "value", sys_uuid FROM %s ORDER BY "name"',
			DT_PROPERTIES
		);

		return $database->Query($loadSql, true);
	}

	/**
	 * Fetch property by id
	 *
	 * @param int $id
	 * @param \PXDatabase|\NLPGSQLDatabase $database
	 * @return array|null
	 * @internal should be used only in properties.module.inc
	 */
	public static function getPropertyById($id, $database) {
		if (empty($id)) {
			return null;
		}

		$loadSql = sprintf(
			'SELECT id, "name", description, "value", sys_uuid FROM %s WHERE id=%d',
			DT_PROPERTIES,
			$database->EscapeString($id)
		);

		$rows = $database->Query($loadSql, true);
		return $rows? reset($rows) : null;
	}
}
