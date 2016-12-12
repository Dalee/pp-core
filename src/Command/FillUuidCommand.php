<?php

namespace PP\Command;

use Ramsey\Uuid\Uuid;
use Symfony\Component\Console\Helper\ProgressBar;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use PP\Lib\Command\AbstractCommand;

class FillUuidCommand extends AbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('db:fill-uuid')
			->setDescription('Fill all empty sys_uuid fields')
			->setHelp('Process concrete datatype, search for empty sys_uuid field and fill it')
			->addArgument('datatype', InputArgument::OPTIONAL, 'Datatype name: struct, html, etc..');
	}

	/**
	 * {@inheritdoc}
	 */
	public function execute(InputInterface $input, OutputInterface $output) {
		$datatype = $input->getArgument('datatype');
		if ($datatype !== null) {
			if (!isset($this->app->types[$datatype])) {
				$output->writeln('<error>Error:</error> Unknown datatype '.$datatype);
				return 1;
			}

			$datatype = $this->app->types[$datatype];
			$this->processDatatype($output, $datatype);

		} else {
			foreach ($this->app->types as $datatype) {
				$this->processDatatype($output, $datatype);
			}
		}

		return 0;
	}

	/**
	 * Process datatype and display progress
	 *
	 * @param OutputInterface $output
	 * @param \PXTypeDescription $datatype
	 */
	protected function processDatatype($output, $datatype) {

		$output->writeln("<info>Processing:</info> ".$datatype->id);
		$where = sprintf("(%s is NULL OR %s = '')", OBJ_FIELD_UUID, OBJ_FIELD_UUID);
		$count = $this->db->getObjectsByWhere($datatype, null, $where, DB_SELECT_COUNT);
		if ($count == 0) {
			return;
		}

		$batch = 100;
		$lastId = 0;

		$sqlSelectFmt = "SELECT id FROM %s WHERE %s AND (%s > %d) ORDER BY %s ASC LIMIT %d";
		$sqlUpdateFmt = "UPDATE %s SET %s = '%s' WHERE %s = %d";

		// process in batches..
		$progress = new ProgressBar($output, $count);

		while (true) {
			$selectSql = sprintf(
				$sqlSelectFmt,
				$datatype->id,
				$where,
				OBJ_FIELD_ID,
				$lastId,
				OBJ_FIELD_ID,
				$batch
			);

			$itemList = $this->db->query($selectSql, true);
			if (empty($itemList)) {
				break;
			}

			foreach ($itemList as $item) {
				$progress->advance();
				$lastId = $item[OBJ_FIELD_ID];

				// generate new uuid and update
				$uuid = $this->db->EscapeString(Uuid::uuid4());
				$updateSql = sprintf(
					$sqlUpdateFmt,
					$datatype->id,
					OBJ_FIELD_UUID,
					$uuid,
					OBJ_FIELD_ID,
					$item[OBJ_FIELD_ID]
				);

				$this->db->query($updateSql, true);
			}
		}

		$progress->finish();
		$output->writeln("");
	}
}
