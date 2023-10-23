<?php

namespace PP\Command;

use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\DependencyInjection\ContainerBuilder;
use Symfony\Component\DependencyInjection\Dumper\PhpDumper;
use Symfony\Component\Config\FileLocator;
use Symfony\Component\DependencyInjection\Loader\YamlFileLoader;
use Symfony\Component\Config\ConfigCache;
use PP\Lib\Command\AbstractCommand;

/**
 * Class CompileContainerCommand.
 *
 * @package PP\Command
 */
class CompileContainerCommand extends AbstractCommand
{
    /**
     * {@inheritdoc}
     */
    protected function configure()
    {
        $this
            ->setName('pp:dump:container')
            ->setDescription('Compiles container to static php file');
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Exception
     */
    public function execute(InputInterface $input, OutputInterface $output)
    {
        $file = CACHE_PATH . DIRECTORY_SEPARATOR . 'container.php';
        $containerConfigCache = new ConfigCache($file, false);

        $path = APPPATH . 'config';
        $container = new ContainerBuilder();
        $loader = new YamlFileLoader($container, new FileLocator($path));

        $loader->load('services.yml');
        $container->compile(true);

        $dumper = new PhpDumper($container);
        $containerConfigCache->write(
            $dumper->dump(['class' => 'MyCachedContainer']),
            $container->getResources()
        );

        return Command::SUCCESS;
    }

}
