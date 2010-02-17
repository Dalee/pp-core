<?php
class PXProfiler {
	var $_stack      = array();
	var $_collection = array();
	var $_isMemoryProfile;
	var $_enabled;

	function &getInstance() {
		static $object;

		if (!isset($object)) {
			$object = new PXProfiler();
			$object->_isMemoryProfile = function_exists('memory_get_usage');
			$object->_enabled = defined("PROFILER") ? PROFILER : false;
		}

		return $object;
	}

	function on(){
		$_ = & PXProfiler::getInstance();
		$_->_enabled = true;
	}
	
	function off(){
		$_ = & PXProfiler::getInstance();
		$_->_enabled = false;
	}

	function Disabled() {
		$_ = & PXProfiler::getInstance();
		return !$_->_enabled;
	}

	function begin($label) {
		if(PXProfiler::Disabled()) {
			return;
		}

		static $call_counter = 0;
		$self = & PXProfiler::getInstance();

		$cls = $self->_isMemoryProfile ? 'PXProfilerStructWithMemory' : 'PXProfilerStruct';

		$self->enq(new $cls($label, $self->stackSize(), $call_counter++));
	}

	function end() {
		if (PXProfiler::Disabled()) {
			return;
		}

		$self = & PXProfiler::getInstance();
		if($self->stackSize() == 0) {
			return;
		}

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

	function toString() {
		$collection = PXProfiler::finalize();
		array_walk($collection, create_function('&$item', '$item = $item->toString();'));

		$result = '';
		$self   = & PXProfiler::getInstance();

		if($self->_isMemoryProfile) {
			$result .= sprintf('%16s%16s', 'MemoryStart(Kb)', 'MemoryDelta(Kb)');
		}

		$result .= sprintf('%8s','Time');
		$result .= "\tProcess\n\n";
		$result .= implode("\n", $collection);

		if(!CLI) {
			$result = '<pre>'.$result.'</pre>';
		}

		return $result;
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
	var $time;
	var $level;
	var $eventName;
	var $index;

	function PXProfilerStruct($name, $level, $index) {
		$this->index           = $index;
		$this->eventName       = $name;
		$this->level           = $level;
		$this->time['start']   = getMicroTime();
	}

	function finalize() {
		$this->time['end']     = getMicroTime();
		$this->time['delta']   = $this->time['end']   - $this->time['start'];
	}

	function toString() {
		$time    = sprintf('%8.4f', $this->time['delta']);
		$level   = str_repeat("\t", $this->level + 1);
		$text    = preg_replace('/[\n\t]/', ' ', $this->eventName);
		
		return $time . $level . $text;
	}
}

class PXProfilerStructWithMemory extends PXProfilerStruct {
	var $memory;

	function PXProfilerStructWithMemory($name, $level, $index) {
		$this->PXProfilerStruct($name, $level, $index);
		$this->memory['start'] = memory_get_usage()/1024;
	}

	function finalize() {
		parent::finalize();

		$this->memory['end']   = memory_get_usage()/1024;
		$this->memory['delta'] = $this->memory['end'] - $this->memory['start'];
	}

	function toString() {
		return sprintf('%16.2f%16.2f', $this->memory['start'], $this->memory['delta']) . parent::toString();
	}
}

?>
