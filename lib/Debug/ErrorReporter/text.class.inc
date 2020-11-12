<?php

class PXErrorReporterText extends PXErrorReporterHTML
{
    public function fatalWarning($error)
    {
        echo "\n\n\n* FATAL ERROR \n";
        echo $error['message'] . "\n\n";
    }

    public function environmentsUris()
    {
        $script = implode(' ', $_SERVER['argv']);
        $user = $this->getUnixUser();
        $root = $_SERVER['DOCUMENT_ROOT'];
        $path = getcwd();

        return <<<HTML
			<dt>Script:</dt>
			<dd>
				{$script}
			</dd>

			<dt>Path:</dt>
			<dd>
				{$path}
			</dd>

			<dt>Root:</dt>
			<dd>
				{$root}
			</dd>

			<dt>User:</dt>
			<dd>
				{$user}
			</dd>
HTML;
    }

    public function getWindowsUser()
    {
        $user = [];

        if (isset($_SERVER['USERNAME'])) {
            $user[] = $_SERVER['USERNAME'];
        }

        if (isset($_SERVER['USERDOMAIN'])) {
            $user[] = $_SERVER['USERDOMAIN'];
        }

        return implode('/', $user);
    }

    public function getUnixUser()
    {
        $user = [];

        if (isset($_SERVER['USER'])) {
            $user[] = $_SERVER['USER'];
        }

        if (isset($_SERVER['SUDO_USER'])) {
            $user[] = $_SERVER['SUDO_USER'];
        }

        return implode('/', $user);
    }

}

?>
