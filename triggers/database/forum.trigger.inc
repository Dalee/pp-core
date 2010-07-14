<?php
class PXTriggerDatabaseForum extends PXAbstractDatabaseTrigger {
	var $dtForums;
	var $dtTopics;
	var $dtMessages;

	function PXTriggerForum(&$settings) {
		$this->dtForums    = 'struct';
		$this->dtTopics    = $settings['topics'];
		$this->dtMessages  = $settings['messages'];
	}

	function _tranzactionStart($transaction) {
		if(!$transaction) {
			return;
		}

		// �������� ����������
		$this->db->transactionBegin();
	}

	function _tranzactionStop($transaction) {
		if(!$transaction) {
			return;
		}

		// ���������� ����������
		$this->db->transactionCommit();
	}

	function _updateLastTopic($id) {
		// ����� ��������� ���������
		$last = $this->db->getObjectsByParentLimited($this->db->types[$this->dtMessages], null, $id, 1, 0, DB_SELECT_TABLE, 'sys_created DESC');
		reset($last);
		$last = current($last);

		if(!is_array($last)) {
			return;
		}

		// ��������� lastreply = sys_created ���������� ��������� �� status = 1
		// ��������� lastauthor = ������ ���������� ��������� �� status = 1, ���� �� ����:
		$lastauthor = !is_null($last['sys_owner']) ? ', lastauthor = '.$last['sys_owner'] : '';
		$this->db->Query("UPDATE " . $this->dtTopics . " SET lastreply = '" . $last['sys_created'] . "'" . $lastauthor . " WHERE id = " . $id, true);

		// ��������� count = ���������� ��������� � ������ - 1
		$this->db->Query("UPDATE ".$this->dtTopics." SET ".$this->db->mapFields('count')." = ((SELECT count(id) FROM ".$this->dtMessages." WHERE parent = ".$id." AND status = '1') - 1) WHERE id = ".$id, true);
	}

	function _updateParentTopic($id) {
		$db =& $this->db;

		// ������� �������� ������
		$parent = $db->Query("SELECT parent FROM ".$this->dtTopics." WHERE id = ".$id, true);
		if(!sizeof($parent)) {
			return;
		}

		reset($parent);

		$parent = current($parent);
		$parent = $parent['parent'];

		// ��������� is_last ���� ������� � ������ � 0
		$db->Query("UPDATE ".$this->dtTopics." SET is_last = '0' WHERE parent = ".$parent, true);

		// ��������� is_last ��� ������ � ��������� lastreply
		$db->ModifyingQuery("UPDATE ".$this->dtTopics." SET is_last = '1' WHERE id = (
				SELECT MAX(id) FROM ".$this->dtTopics." WHERE parent = ".$parent." AND lastreply = (
					SELECT MAX(lastreply) FROM ".$this->dtTopics." WHERE parent = ".$parent." and status = '1'
				)
			)");
	}

	function updateTopic(&$db, $id, $transaction = true) {
		$this->db =& $db;

		$this->_tranzactionStart($transaction);
		$this->_updateLastTopic($id);
		$this->_updateParentTopic($id);
		$this->_tranzactionStop($transaction);
	}

	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if($format->id == $this->dtMessages) {
			$this->UpdateTopic($db, $object['parent']);
		}

		return;
	}

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if($format->id == $this->dtMessages) {
			$this->UpdateTopic($db, $object['parent']);
		}

		if($format->id == $this->dtTopics) {
			$this->UpdateTopic($db, $object['id']);
		}

		return;
	}

	function OnDeleteObject(&$db, &$objId, &$format, &$object) {
		if($format->id == $this->dtMessages) {
			$this->UpdateTopic($db, $object['parent']);
		}

		if($format->id == $this->dtTopics) {
			$this->UpdateTopic($db, $object['id']);
		}

		return;
	}
}
?>