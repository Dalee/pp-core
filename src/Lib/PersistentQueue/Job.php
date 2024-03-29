<?php

namespace PP\Lib\PersistentQueue;

use UnexpectedValueException;
use PP\Lib\IArrayable;

/**
 * Class Job.
 *
 * @package PP\Lib\PersistentQueue
 */
class Job implements IArrayable
{
    /**
     * @var string
     */
    public const STATE_FRESH = 'fresh';

    /**
     * @var string
     */
    public const STATE_FINISHED = 'finished';

    /**
     * @var string
     */
    public const STATE_FAILED = 'failed';

    /**
     * @var string
     */
    public const STATE_IN_PROGRESS = 'in progress';

    /**
     * @var int
     */
    protected $id = 0;

    /**
     * @var array
     */
    protected $payload;

    /**
     * @var string
     */
    protected $worker;

    /**
     * @var string
     */
    protected $state;

    /**
     * @var JobResult
     */
    protected $resultBag;

    /**
     * @var int
     */
    protected $ownerId;

    /**
     * Job constructor
     */
    public function __construct()
    {
        $this->state = static::STATE_FRESH;
        $this->resultBag = new JobResult();
    }

    /**
     * Converts instance to array.
     *
     * @return array
     */
    public function toArray()
    {
        return [
            'id' => $this->id,
            'title' => $this->worker,
            'payload' => $this->payload,
            'state' => $this->state,
            'result' => $this->resultBag->toArray(),
            'sys_owner' => $this->ownerId
        ];
    }

    /**
     * Returns all valid states.
     *
     * @return array
     */
    public static function getValidStates()
    {
        return [
            static::STATE_FRESH,
            static::STATE_FAILED,
            static::STATE_FINISHED,
            static::STATE_IN_PROGRESS
        ];
    }

    /**
    * Creates instance from array.
    *
    * @return static
    */
    public static function fromArray(array $object)
    {
        $job = new static();
        $job->setId(getFromArray($object, 'id', 0));
        $workerClass = getFromArray($object, 'title');
        $state = getFromArray($object, 'state', static::STATE_FRESH);

        return $job->setState($state)
            ->setPayload(getFromArray($object, 'payload', []))
            ->setWorkerClass($workerClass)
            ->setOwnerId(getFromArray($object, 'sys_owner', 0));
    }

    /**
    * @return $this
    */
    public function setPayload(array $payload)
    {
        $this->payload = $payload;

        return $this;
    }

    /**
     * @return array
     */
    public function getPayload()
    {
        return $this->payload;
    }

    /**
    * @return $this
    */
    public function setWorker(WorkerInterface $worker)
    {
        $this->setWorkerClass($worker::class);

        return $this;
    }

    /**
     * @param string $worker
     *
     * @return $this
     */
    public function setWorkerClass($worker)
    {
        $this->worker = $worker;

        return $this;
    }

    /**
     * @return WorkerInterface
     */
    public function getWorker()
    {
        if (!class_exists($this->worker)) {
            throw new UnexpectedValueException(
                sprintf('Worker class does not exist: %s', $this->worker)
            );
        }

        $worker = new $this->worker();
        if (!($worker instanceof WorkerInterface)) {
            throw new UnexpectedValueException(
                'Worker class does not implement PP\Lib\PersistentQueue\WorkerInterface'
            );
        }

        return $worker;
    }

    /**
     * @return string
     */
    public function getState()
    {
        return $this->state;
    }

    /**
     * @param $state
     * @return $this
     */
    public function setState($state)
    {
        $validStates = static::getValidStates();
        if (!in_array($state, $validStates, true)) {
            throw new UnexpectedValueException(
                sprintf('State is invalid. Valid states: %s', join(', ', $validStates))
            );
        }

        $this->state = $state;

        return $this;
    }

    /**
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * @param $id
     * @return $this
     */
    public function setId($id)
    {
        $this->id = $id;

        return $this;
    }

    /**
     * @return JobResult
     */
    public function getResultBag()
    {
        return $this->resultBag;
    }

    /**
    * @return $this
    */
    public function setResultBag(JobResult $resultBag)
    {
        $this->resultBag = $resultBag;

        return $this;
    }

    /**
     * @return int
     */
    public function getOwnerId()
    {
        return $this->ownerId;
    }

    /**
     * @param int $ownerId
     * @return $this
     */
    public function setOwnerId($ownerId)
    {
        $this->ownerId = $ownerId;

        return $this;
    }

}
