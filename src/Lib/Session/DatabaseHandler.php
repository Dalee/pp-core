<?php

namespace PP\Lib\Session;

use PP\Lib\Database\Driver\PostgreSqlDriver;
use PXDatabase;
use Symfony\Component\HttpFoundation\Session\Storage\Handler\NullSessionHandler;

class DatabaseHandler extends NullSessionHandler
{
    public const SESSION_TABLE = 'admin_session';

    /** @var PXDatabase|PostgreSqlDriver */
    protected $db;
    protected $gcCalled;

    public function __construct(PXDatabase $db)
    {
        $this->db = $db;
    }

    /**
     * @param string $savePath
     * @param string $sessionName
     * @return bool
     */
    public function open($savePath, $sessionName): bool
    {
        return true;
    }

    /**
     * @return bool
     */
    public function close(): bool
    {
        if ($this->gcCalled) {
            $sql = sprintf(
                "DELETE FROM %s WHERE (session_lifetime + session_time) < %d",
                static::SESSION_TABLE,
                time()
            );

            $this->db->ModifyingQuery($sql, null, null, false);
        }

        return true;
    }

    /**
     * @param string $sessionId
     * @return string
     * @throws \JsonException
     */
    public function read($sessionId): string
    {
        $sessionId = $this->db->EscapeString($sessionId);

        $sql = sprintf(
            "SELECT * FROM %s WHERE session_id = '%s'",
            static::SESSION_TABLE,
            $sessionId
        );

        $row = $this->db->Query($sql, true);
        if (($row = reset($row)) !== false) {
            $maxTime = (int)$row['session_time'] + (int)$row['session_lifetime'];
            if ($maxTime > time()) {
                return json_decode((string) $row['session_data']);
            }
        }

        return '';
    }

    /**
     * @param string $sessionId
     * @param string $data
     * @return bool
     */
    public function write($sessionId, $data): bool
    {
        $sessionId = $this->db->EscapeString($sessionId);
        $data = $this->db->EscapeString(json_encode($data));
        $maxlifetime = (int)ini_get('session.gc_maxlifetime');
        $now = time();

        $sql = sprintf(
            "UPDATE %s
				SET session_data = '%s', session_lifetime = %d, session_time = %d
				WHERE session_id = '%s'",
            static::SESSION_TABLE,
            $data,
            $maxlifetime,
            $now,
            $sessionId
        );

        $result = $this->db->ModifyingQuery($sql, null, null, false, true);
        if ($result === 0) {
            $sql = sprintf(
                "INSERT INTO %s
					(session_id, session_data, session_lifetime, session_time)
					VALUES ('%s', '%s', %d, %d)",
                static::SESSION_TABLE,
                $sessionId,
                $data,
                $maxlifetime,
                $now
            );

            $this->db->ModifyingQuery($sql, null, null, false, false);
        }

        return true;
    }

    /**
     * Destroy session
     * @param string $sessionId
     * @return bool
     */
    public function destroy($sessionId): bool
    {
        $sessionId = $this->db->EscapeString($sessionId);
        $sql = sprintf(
            "DELETE FROM %s WHERE session_id = '%s'",
            static::SESSION_TABLE,
            $sessionId
        );

        $this->db->ModifyingQuery($sql, null, null, false);

        return true;
    }

    /**
     * @param int $maxlifetime
     * @return int|bool
     */
    public function gc($maxlifetime): int|false
    {
        $this->gcCalled = true;

        return true;
    }

}
