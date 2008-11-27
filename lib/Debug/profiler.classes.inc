<?php
class PXProfiler {
	var $_stack = array();
	var $_collection = array();

	function &getInstance() {
		static $object;

		if (!isset($object)) {
			$object = new PXProfiler();
		}

		return $object;
	}

	function Disabled() {
		return !PROFILER;
	}

	function begin($label) {
		if(PXProfiler::Disabled()) {
			return;
		}

		static $call_counter = 0;
		$self = & PXProfiler::getInstance();
		$self->enq(new PXProfilerStruct($label, $self->stackSize(), $call_counter++));
	}

	function end() {
		if (PXProfiler::Disabled()) {
			return;
		}

		$self = & PXProfiler::getInstance();
		if($self->stackSize() == 0) return;

		$item = $self->deq();
		$item->finalize();
		$self->append($item);
	}

	function enq($item){
		array_push($this->_stack, $item);
	}

	function deq() {
		return array_pop($this->_stack);
	}

	function finalize() {
		if (PXProfiler::Disabled()) {
			return array();
		}

		$self = & PXProfiler::getInstance();
		while ($self->stackSize() > 0){
			PXProfiler::end();
		}
		$collection = $self->getItems();
		ksort($collection);
		return $collection;
	}

	function append($item){
		$this->_collection[$item->index] = $item;
	}

	function getItems(){
		return $this->_collection;
	}

	function stackSize(){
		return count($this->_stack);
	}
}

class PXProfilerStruct {
	var $start;
	var $end;
	var $delta;
	var $level;
	var $eventName;
	var $index;

	function PXProfilerStruct($name, $level, $index) {
		$this->index     = $index;
		$this->eventName = $name;
		$this->level     = $level;
		$this->start     = getMicroTime();
	}

	function finalize() {
		$this->end   = getMicroTime();
		$this->delta = $this->end - $this->start;
	}

}

?>
