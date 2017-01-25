<?php

namespace PP\Command;

use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use PP\Lib\Command\AbstractCommand;

/**
 * Class CreateMetaCommand
 * @package PP\Command
 */
class FillMetaCommand extends AbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('db:fill:meta')
			->setDescription('Fill all empty sys_meta field');
	}

	/**
	 * {@inheritdoc}
	 */
	public function execute(InputInterface $input, OutputInterface $output) {
		$limit = 100;

		foreach ($this->app->types as $type) {
			$needProcess = false;
			foreach ($type->fields as $v) {
				if (!$v->storageType->storedInDb()) {
					$needProcess = true;
					break;
				}
			}

			if (!$needProcess) {
				$output->writeln(sprintf("<info>No need to be processed:</info> %s", $type->id));
				continue;
			}

			// @TODO: refactor, to check sys_meta = NULL
			$output->writeln(sprintf("<info>Processing:</info> %s", $type->id));
			$queryUpdateFmt = 'UPDATE %s SET %s WHERE id = %s';
			$querySelectFmt = 'SELECT * FROM %s WHERE id > %d ORDER BY id ASC LIMIT %d';
			$lastId = 0;

			while (true) {
				$selector = sprintf($querySelectFmt, $type->id, $lastId, $limit);
				$objectList = $this->db->Query($selector);
				if (empty($objectList)) {
					break;
				}

				$this->db->_NormalizeTable($objectList, $type, false);
				foreach ($objectList as $object) {

					$sysMetaField = [];
					foreach ($type->fields as $k => $v) {
						if (!$v->storageType->storedInDb()) {
							$p = array('id' => $object['id'], 'format' => $type->id);
							if (($proceedFileResult = $v->storageType->proceedFile($v, $object, $p))) {
								$sysMetaField[$k] = $proceedFileResult;
							}
						}
					}

					$metaField = (count($sysMetaField) > 0)
						? $this->db->MapData(json_encode($sysMetaField))
						: 'NULL';

					$metaField = sprintf("%s = %s", OBJ_FIELD_META, $metaField);

					// fire!
					$lastId = $object['id'];
					$query = sprintf($queryUpdateFmt, $type->id, $metaField, $lastId);
					$this->db->query($query);
				}
			}
		}
	}
}
