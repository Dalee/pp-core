<?php
/**
 * ëÌÁÓÓ - ÈÒÁÎÉÌÉÝÅ.
 * óÔÏÉÔÓÑ ÐÏ etc/datatype.xml /model/datatypes/datatype
 *
 * @subpackage Application
 */
class PXTypeDescription {
	var $id            = NULL;
	var $title         = NULL;
	var $base          = NULL;
	var $parent        = NULL;
	var $childs        = array();
	var $references    = array();
	var $order         = NULL;
	var $nullrole      = 0;
	var $fields        = array();
	var $struct        = 'plain';
	var $byDefault     = NULL;
	var $outputHandler = NULL;

	public function addField($field) {
		$this->fields[$field->name] = $field;
	}
}
?>