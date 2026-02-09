<?php

namespace PP\Command;

use PP\Lib\Command\AbstractCommand;
use PP\Properties\PropertyTypeBuilder;
use PXAuditLogger;
use Ramsey\Uuid\Uuid;
use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;

/**
 * Add or update single property
 *
 * Class SetProperty
 * @package PP\Command
 */
class SetPropertyCommand extends AbstractCommand
{
    /**
     * {@inheritdoc}
     */
    protected function configure()
    {
        $this
            ->setName('db:property:set')
            ->setDescription('Set application property')
            ->setHelp('Set property')
            ->addArgument('key', InputArgument::REQUIRED, 'property name')
            ->addArgument('val', InputArgument::OPTIONAL, 'property value', '')
            ->addArgument('description', InputArgument::OPTIONAL, 'property description', '');
    }

    /**
     * {@inheritdoc}
     */
    protected function execute(InputInterface $input, OutputInterface $output): int
    {
        $key = $input->getArgument('key');
        $val = $input->getArgument('val');
        $description = $input->getArgument('description');

        $dbFields = ['name', 'value'];
        $dbValues = [$key, $val];

        if (!empty($description)) {
            $dbFields[] = 'description';
            $dbValues[] = $description;
        }

        $sql = sprintf('SELECT id, value FROM %s WHERE "name"=\'%s\'', DT_PROPERTIES, $this->db->EscapeString($key));
        $result = $this->db->query($sql);

		$audit = PXAuditLogger::getLogger();

		if ((is_countable($result) ? count($result) : 0) > 0) {
            $result = array_flat($result[0], 'id');

			if ($result['value'] === $val) {
				$output->writeln("Property: $key: <info>nothing to change</info>");

				return Command::SUCCESS;
			}

			$updateResult = $this->db->UpdateObjectById(DT_PROPERTIES, $result['id'], $dbFields, $dbValues);
            $output->writeln("Property: $key: <info>updated</info>");

			$auditSource = $this->getAuditSource($result['id']);

			if (!is_numeric($updateResult)) {
				$auditMessage = sprintf('%s `%s`', 'Параметр изменен', $key);
				$audit->info(
					description: $auditMessage,
					source: $auditSource,
					diff: json_encode(['value']),
				);
			} else {
				$errMessage = sprintf('%s `%s`', 'Ошибка изменения параметра', $key);
				$audit->error($errMessage, $auditSource);

				return Command::FAILURE;
			}
		} else {
            $dbFields[] = 'sys_uuid';
            $dbValues[] = Uuid::uuid4()->toString();

			$id = $this->db->InsertObject(DT_PROPERTIES, $dbFields, $dbValues);
            $output->writeln("Property: $key: <info>inserted</info>");

			if ($id > 0) {
				$auditMessage = sprintf('%s `%s`', 'Параметр добавлен', $key);
				$audit->info($auditMessage, $this->getAuditSource($id));
			} else {
				$errMessage = sprintf('%s `%s`', 'Ошибка добавления параметра', $key);
				$audit->error($errMessage, $this->getAuditSource());

				return Command::FAILURE;
			}
        }

        return Command::SUCCESS;
    }

	private function getAuditSource(int|null $id = 0): string
	{
		return sprintf('%s/%s', PropertyTypeBuilder::TYPE_ID, (int) $id);
	}
}
