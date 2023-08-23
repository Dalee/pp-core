<?php

namespace PP\Command\Migrate;

use PP\Lib\Command\MigrateAbstractCommand;
use PP\Migration\MigrationAbstract;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;

/**
 * Class MigrateCommand
 * @package PP\Command
 */
class MigrateUpCommand extends MigrateAbstractCommand
{
    /**
     * {@inheritdoc}
     */
    protected function configure()
    {
        $this
            ->setName('db:migrate:up')
            ->setDescription('Migrate all pending migrations');
    }

    /**
     * {@inheritdoc}
     */
    public function execute(InputInterface $input, OutputInterface $output)
    {
        $migrationList = $this->getPendingMigrations();
        if (count($migrationList) === 0) {
            $output->writeln("No pending migrations");
            return 0;
        }

        $this->dbDriver->transactionBegin();
        $output->writeln("<info>Starting migrations</info>");
        try {
            $migrationPath = $this->getMigrationsDirectory();

            foreach ($migrationList as $fileName) {
                $className = $this->getMigrationClassWithNamespace($fileName);
                $requirePath = join(DIRECTORY_SEPARATOR, [$migrationPath, $fileName]);
                $classInstance = $this->getClassInstance($requirePath, $className);

                // filling up migration list of sql code
                $classInstance->up();

                // formatting set of instructions
                $sqlList = $classInstance->getSqlList();
                $sqlList[] = $this->getMigrationSqlFinalizer($fileName);

                // applying sql
                foreach ($sqlList as $sql) {
                    $result = $this->dbDriver->ModifyingQuery($sql);
                    if ($result === ERROR_DB_BADQUERY || $result === ERROR_DB_CANNOTCONNECT) {
                        throw new \Exception("Migration failed: ${fileName}");
                    }
                }

                $output->writeln("<info>${fileName}</info> migrated successfully");
            }

            $this->dbDriver->transactionCommit();
            $output->writeln("<info>Done!</info>");

        } catch (\Exception $e) {
            $this->dbDriver->transactionRollback();
            throw $e;
        }
    }

    /**
     * @param string $filePath
     * @param string $className
     * @return MigrationAbstract
     * @throws \Exception
     */
    protected function getClassInstance($filePath, $className)
    {
        require_once $filePath;

        if (!class_exists($className)) {
            throw new \Exception("Class: ${className} doesn't exist");
        }

        $classInstance = new $className();
        if (!$classInstance instanceof MigrationAbstract) {
            throw new \Exception("Class: ${className} is not instance of AbstractMigration");
        }

        return $classInstance;
    }
}
