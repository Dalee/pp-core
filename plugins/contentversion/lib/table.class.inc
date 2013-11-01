<?php
class PXVersionsTable extends PXAdminTableSimple {
	function cell($row) {
		$menuTitle = (mb_strlen($row['title']) > 10) ? trim(mb_substr($row['title'], 0, 10)).'&hellip;' : $row['title'];
		$context = 'Context(event, \'showversion\', '.$row['id'].', '.(int)($row['status']).', \''. $this->dict['datatypes']->id .'\',  \''.$menuTitle.'\', \'\', 1, 0, 0); return false;';
		$a = sprintf('<a oncontextMenu="%s" href="javascript:EditContent(\'%s\', \'%s\', \'contentversion\')"><img src="i/icon/edit.gif" alt=" &laquo;%s&raquo;"></a>', $context, $this->dict['datatypes']->id, $row['id'], $row['id']);
		$tr = <<<HTML
			<tr>
				<td class="c3">
					<input type="radio" name="version" value="{$row['sys_version']}" />
					{$a}
				</td>
				<td>{$row['sys_version']}</td>
				<td>{$row['sys_modifyer']}</td>
				<td>{$row['sys_created']}</td>
			</tr>
HTML;
		return $tr;
	}

	function htmlBody() {
		$_  = array();
		$i = 0;
		foreach($this->data as $row) {
			$even = $i++ % 2;
			$_[] = '<tr'.($this->showEven && $even ? ' class="even"': '').'>';
			$_[] = $this->cell($row);
			$_[] = '</tr>';
		}
 
		return join($_);
	}

}
?>
