<?php

namespace PP\Command;

use PP\Lib\Command\AbstractCommand;
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

        $sql = sprintf('SELECT id FROM %s WHERE "name"=\'%s\'', DT_PROPERTIES, $this->db->EscapeString($key));
        $result = $this->db->query($sql);
        if ((is_countable($result) ? count($result) : 0) > 0) {
            $result = array_flat($result[0], 'id');

            $this->db->UpdateObjectById(DT_PROPERTIES, $result['id'], $dbFields, $dbValues);
            $output->writeln("Property: ${key}: <info>updated</info>");

        } else {
            $dbFields[] = 'sys_uuid';
            $dbValues[] = Uuid::uuid4()->toString();

            $this->db->InsertObject(DT_PROPERTIES, $dbFields, $dbValues);
            $output->writeln("Property: ${key}: <info>inserted</info>");
        }

        return Command::SUCCESS;
    }
}
