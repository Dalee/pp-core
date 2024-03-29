<?php

namespace PP\Lib\Command;

use PP\Lib\Database\Driver\PostgreSqlDriver;

/**
 * Class MigrateAbstractCommand
 * @package PP\Command
 */
abstract class MigrateAbstractCommand extends AbstractBasicCommand
{
    /** @var PostgreSqlDriver */
    protected $dbDriver;

    protected $namespace = 'PP\Migration';
    protected $classPart = 'Migration';

    protected $template = <<<PHP
<?php

namespace {{namespace}};

class {{class}} extends MigrationAbstract {

	/**
	 * {@inheritdoc}
	 */
	public function up() {
	}

}
PHP;

    /**
     * @param PostgreSqlDriver $dbDriver
     * @return $this
     */
    public function setDbDriver($dbDriver)
    {
        $this->dbDriver = $dbDriver;
        $this->dbDriver->Connect();

        return $this;
    }

    /**
     * @return string
     */
    protected function getMigrationsDirectory()
    {
        return APPPATH . '/migrations';
    }

    /**
     * Return filename list of pending migrations.
     * Query should run without any cache.
     *
     * @return string[]
     * @throws \Exception
     */
    protected function getPendingMigrations()
    {
        $applied = $this->dbDriver->Query("SELECT * FROM _migrations", true);
        if (!is_array($applied)) {
            throw new \Exception("migrate:up failed, invalid query");
        }

        $applied = array_map(fn ($item) => $item['filename'], $applied);

        $dir = new \DirectoryIterator($this->getMigrationsDirectory());

        $full = [];
        foreach ($dir as $fileName) {
            if ($fileName->getExtension() !== 'php') {
                continue;
            }

            $full[] = $fileName->getBasename();
        }

        $finalResult = array_diff($full, $applied);
        sort($finalResult);

        return $finalResult;
    }

    /**
     * @param string $fileName
     * @return string
     */
    protected function getMigrationSqlFinalizer($fileName)
    {
        return "INSERT INTO _migrations (filename) VALUES ('${fileName}')";
    }

    /**
     * @param string $fileName
     * @return string
     */
    protected function getMigrationClass($fileName)
    {
        $version = basename($fileName, '.php');
        return $this->classPart . $version;
    }

    /**
     * @param string $fileName
     * @return string
     */
    protected function getMigrationClassWithNamespace($fileName)
    {
        return $this->namespace . '\\' . $this->getMigrationClass($fileName);
    }
}
