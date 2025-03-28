<?php

namespace PP\Lib\Auth;

use Symfony\Component\HttpFoundation\Session\SessionInterface;

interface AuthInterface
{
    /**
     * @param array $credentials
     * @return bool
     */
    public function isCredentialsValid(array $credentials): bool;

    /**
     * @return bool
     */
    public function isAuthorized(): bool;

    /**
     * @param array $uArray
     * @return bool
     */
    public function fillUserFields(array $uArray): void;

    /**
     * @return bool
     */
    public function auth(): bool;

    /**
     * @return bool
     */
    public function unAuth(): bool;

    /**
     * @param \PXRequest $request
     * @return $this
     */
    public function setRequest(\PXRequest $request): self;

    /**
     * @param \PXDatabase $db
     * @return $this
     */
    public function setDb(\PXDatabase $db): self;

    /**
     * @param \PXApplication $app
     * @return $this
     */
    public function setApp(\PXApplication $app): self;

    /**
     * @param \PXUser $user
     * @return $this
     */
    public function setUser(\PXUser $user): self;

    /**
     * @param Session|null $session
     * @return $this
     */
    public function setSession(?SessionInterface $session = null): self;

    /**
     * @return bool
     */
    public function onAuth(): bool;

    /**
     * @param string $passwd
     * @return string
     */
    public static function passwdToDB(string $passwd): string;

    /**
     * @param string $plainPassword
     * @param string $hash
     * @return bool
     */
    public static function verifyPassword(string $plainPassword, ?string $hash): bool;
}
