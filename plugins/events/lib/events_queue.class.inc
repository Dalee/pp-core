<?php

class PXEventsQueue {
	const TABLE = 'sys_events';

	static function dequeue() {
		$event = (array) PXRegistry::getDB()->query(sprintf('select * from %s order by id asc limit 1', self::TABLE));
		$event = reset($event);

		if (!empty($event['id']) && !empty($event['callback']) && !empty($event['params'])) {
			$event = self::normalize($event);
			self::delete($event['id']);
			return $event;
		}
	}

	static function all() {
		return array_map(array(__CLASS__, 'normalize'),
							PXRegistry::getDB()->query(sprintf('select * from %s where in_process = false order by id asc', self::TABLE)));
	}

	static function delete($id) {
		PXRegistry::getDB()->modifyingQuery(sprintf('DELETE FROM %s WHERE id = %s', self::TABLE, intval($id)));
	}

	static function enqueue($args) {
		PXRegistry::getDB()->modifyingQuery(sprintf("INSERT INTO %s (callback, params) values ('%s', '%s')",
			self::TABLE, PXStorageTypeYaml::toYaml($args['callback']), PXStorageTypeYaml::toYaml($args['params'])));
	}

	static function normalize($record) {
		$record['callback'] = PXStorageTypeYaml::toArray($record['callback']);
		$record['params']   = PXStorageTypeYaml::toArray($record['params']);
		return $record;
	}
}

?>
