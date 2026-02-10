<?php

namespace PP\Properties;

use PP\Lib\Database\Driver\PostgreSqlDriver;

/**
 * Class PropertyLoader
 * @package PP\Properties
 */
class PropertyLoader
{
    /**
    * Load all properties as key => value.
    *
    * @return array
    * @internal should be used only in PXApplication context
    */
    public static function getPropertyList(\PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver $database)
    {
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
    * @return array
    * @internal should be used only in properties.module.inc
    */
    public static function getRawPropertyList(\PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver $database)
    {
        $loadSql = sprintf(
            'SELECT id, "name", description, "value", sys_uuid FROM %s ORDER BY "name"',
            DT_PROPERTIES
        );

        return $database->Query($loadSql, true);
    }

	/**
	 * Load raw property list with IDs filtering.
	 *
	 * @param \PXDatabase|PostgreSqlDriver $database
	 * @param array $ids
	 *
	 * @return array
	 * @internal should be used only in properties.module.inc
	 */
	public static function getRawPropertyListByIds(\PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver $database, array $ids): array
	{
		if (empty($ids)) {
			return [];
		}

		$loadSql = sprintf(
			'SELECT id, "name", description, "value", sys_uuid FROM %s WHERE id IN (%s) ORDER BY "name"',
			DT_PROPERTIES,
			join(', ', array_map('intval', $ids))
		);
		$propertyList = $database->Query($loadSql, true);

		return array_column($propertyList, null, 'id');
	}

    /**
    * Fetch property by id
    *
    * @param int $id
    * @return array|null
    * @internal should be used only in properties.module.inc
    */
    public static function getPropertyById($id, \PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver $database)
    {
        if (empty($id)) {
            return null;
        }

        $loadSql = sprintf(
            'SELECT id, "name", description, "value", sys_uuid FROM %s WHERE id=%d',
            DT_PROPERTIES,
            $database->EscapeString($id)
        );

        $rows = $database->Query($loadSql, true);
        return $rows ? reset($rows) : null;
    }
}
