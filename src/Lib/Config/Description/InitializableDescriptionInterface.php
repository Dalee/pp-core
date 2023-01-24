<?php

namespace PP\Lib\Config\Description;

interface InitializableDescriptionInterface {

	/**
  * Called when PXApplication wakes up from cache
  *
  * @return void
  */
 public function initialize(\PXApplication $app);
}
