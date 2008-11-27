<?php
if (!defined('FORM_ALLOW_NONE')) {
	define('FORM_ALLOW_NONE',  0);
	define('FORM_ALLOW_READ',  1);
	define('FORM_ALLOW_WRITE', 2);
}

class PXAdminObjectForm extends NLAbstractHTMLForm {
	var $format;
	var $object;

	function PXAdminObjectForm($format, $object) {
		$this->NLAbstractHTMLForm();
		$this->format = $format;
		$this->object = $object;
	}

	function MainForm($tree, $selfTree, $datatype) {
		$html  = $this->BaseInputs('main');

		$param = array(
			'parents'     => $tree->getPlainTree(getFromArray($this->object, 'id')),
			'selfParents' => $selfTree->getPlainTree(getFromArray($this->object, 'id')),
			'even'        => false,
			'tree'        => $tree,
			'datatype'    => $datatype
		);

		$html .= '<table class="mainform">';
		foreach ($this->format->fields as $k=>$v) {
			if($k === 'id') {
				continue;
			}

			if(!isset($this->object[$k]) && isset($v->defaultValue)) {
				$this->object[$k] = htmlspecialchars($v->defaultValue);
			}

			$html .= PXDisplayType::callMethod('buildRow', $v, $this->object, $param);
		}
		$html .= '</table>';

		if (isset($this->object['sys_created']) || isset($this->object['sys_modified']) || isset($this->object['ownerlogin'])) {
			$html .= '<dl class="sysinfo">';
			$html .= isset($this->object['sys_created'])  ? '<dt>Создан:  </dt><dd>'.substr($this->object['sys_created'],  0, 19).'</dd>' : '';
			$html .= isset($this->object['sys_modified']) ? '<dt>Изменен: </dt><dd>'.substr($this->object['sys_modified'], 0, 19).'</dd>' : '';
			$html .= isset($this->object['ownerlogin'])   ? '<dt>Владелец:</dt><dd>'.$this->object['ownerlogin'].'</dd>'                  : '';
			$html .= '</dl>';
		}

		return $html;
	}

	function LeftControls() {
		return  $this->BuildSubmit('Сохранить').$this->BuildSubmit('Сохранить и закрыть', "close");
	}

	function RightControls() {
		return  $this->BuildReset('Отменить').$this->BuildClose('Закрыть');
	}

	function BaseInputs($action) {
		return  $this->BuildHidden('id', ((isset($this->object['id'])) ? $this->object['id'] : '')).
			$this->BuildHidden('area', 'objects').
			$this->BuildHidden('action', $action).
			$this->BuildHidden('format', $this->format->id);
	}

	function LinksForm($reference, $formatTo, $links, $pLinks, 
		$objectsOnPage = 0, $page = 1, $count = 0, &$layout, $onlyExistingLinks = false) {
		$displayFields = array();
		foreach ($formatTo->fields as $k => $f) {
			if ($f->listed) {
				$displayFields[$k] = $f;
			}
		}

		$html  = '<h2>Связи с объектами формата &laquo;'.$formatTo->title.'&raquo; ['.sizeof($links).' из '.$count.']</h2>';
		$html .= '<table class="objects links">';
		$html .= '<thead><tr><th class="c3"></th>';

		foreach ($displayFields as $f) {
			$html .= '<th>'.$f->description.'</th>';
		}

		foreach ($reference->fields as $k=>$v) {
			$html .= '<th>'.$v->description.'</th>';
		}
		$html .= '</tr></thead><tbody>';

		foreach ($pLinks as $k=>$v) {
			$html .= '<tr'.($v['status'] == false ? ' class="hide"' : '').'>';
			$html .= '<td class="c3">';
			$html .= $this->BuildHidden($reference->name.'['.$v['id'].'][id]', $v['id']);
			$html .= $this->BuildCheckBox($reference->name.'['.$v['id'].'][on]', isset($links[$v['id']]), NULL, $reference->name.$v['id']);

			$menuTitle = (strlen($v['title']) > 10) ? trim(substr($v['title'], 0, 10)).'&hellip;' : $v['title'];
			$context = 'Context(event, \'edit\', '.$v['id'].', '.(int)($v['status']).', \''.$formatTo->id.'\',  \''.$menuTitle.'\', \'\', 1, 0, 0); window.event.cancelBubble = true; return false;';
			$html .= '<a oncontextMenu="'.$context.'" href="javascript:EditContent(\''.$formatTo->id.'\', \''.$v['id'].'\')">';
			$html .= '<img src="i/icon/edit.gif" alt="Изменить &laquo;'.$v['id'].'&raquo;"></a>';
			$html .= '</td>';

			foreach ($displayFields as $k => $f) {
				$html .= '<td>';
				$html .= '<label checkbox="'.$reference->name.$v['id'].'" for="'.$reference->name.$v['id'].'" style="width: 100%; '.(isset($links[$v['id']]) ? 'font-weight: bold;' : '').'">';
				$html .= PXDisplayType::callMethod('buildCell', $f, $v);
				$html .= '</label>';

				$html .= '</td>';
			}
			foreach ($reference->fields as $sk=>$sv) {
				$value = (isset($links[$v['id']])) ? $links[$v['id']]['fields'][$sk] : NULL;

				$k = $reference->name.'['.$v['id'].'][fields]['.$sk.']';

				$sv->name = $k;

				$html .= "\n".'<td>'.PXDisplayType::callMethod('buildInput', $sv, array($k=>$value)).'</td>';
			}
			$html .= '</tr>';
		}
		$html .= '</tbody></table>';

		if($onlyExistingLinks) {
			$count = sizeof($links);
		}

		$html .= '<div class="perpage" style="zoom: 1;">';
			$html .= '<a style="float: right;" href="'.$layout->_BuildHref($formatTo->id.'_exist', 1).'" '.( $onlyExistingLinks ? 'class="selected"': '').'>привязанные</a>';
			$html .= '<a style="float: right;" href="'.$layout->_BuildHref($formatTo->id.'_exist', 0).'" '.(!$onlyExistingLinks ? 'class="selected"': '').'>все</a>';


		if ($count > $objectsOnPage && $objectsOnPage > 0) {
			$allPages = ceil($count/$objectsOnPage)+1;
			$start    = (ceil($page/10)-1)*10+1;
			$max      = $start + 10;

			if ($max > $allPages) {
				$max = $allPages;
			}

			if ($page > 10) {
				$prev = (ceil($start/10)-1)*10 - 9;
			}

			$last = $allPages - $start - 10;

			if ($last > 0) {
				$next = (ceil($start/10)-1)*10 + 11;
			}

			if (isset($prev)) {
				$html .= '<A href="'.$layout->_BuildHref($formatTo->id.'_page', $prev).'">';
				$html .= '<IMG src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
				$html .= '</A>';
			}

			for ($i=$start; $i<$max; $i++) {
				$html .= '<A';

				if ($i == $page) {
					$html .= ' class="selected"';
				}

				$html .= ' href="'.$layout->_BuildHref($formatTo->id.'_page', $i).'" title="Страница '.$i.'">'.$i.'</A>';
			}

			if (isset($next) && $next > 0) {
				$html .= '<A href="'.$layout->_BuildHref($formatTo->id.'_page', $next).'">';
				$html .= '<IMG src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
				$html .= '</A>';
			}

		}
		$html .= '</DIV>';

		return $this->BaseInputs('links').$html;
	}

	function childrenForm($types, $counts = NULL) {
		$values = array(
			PP_CHILDREN_FETCH_ALL      => 'Все',
			PP_CHILDREN_FETCH_SELECTED => 'Текущий',
			PP_CHILDREN_FETCH_NONE     => 'Никакие'
		);

		$name = OBJ_FIELD_CHILDREN;
		$html = <<<HTML
		<script type="text/javascript">
			function changeChildrenDropDown(form, id, checked) {
				for (var i=0; i<form.elements.length; i++) {
					if (form.elements[i].name == '{$name}[' + id + ']') {
						form.elements[i].disabled = !checked;
					}
				}
			}
		</script>
HTML;
		$html .= '<dl class="allowed">'."\n";

		foreach ($types as $k=>$v) {
			$quantity = (isset($counts[$k])) ? $counts[$k] : 0;

			if (!in_array($k, $this->format->childs)) {
				continue;
			}

			$checked  = isset($this->object[OBJ_FIELD_CHILDREN][$k]) ? ' checked'  : '';
			$disabled = $quantity > 0                                ? ' disabled' : '';
			$onClick  = ' onclick="changeChildrenDropDown(this.form, \''.$k.'\', this.checked);"';

			$html .= '<dt>';
			$html .= '<input type="checkbox"'.$onClick.$checked.$disabled.' id="'.OBJ_FIELD_CHILDREN.'['.$k.']">';
			$html .= '<label for="'.OBJ_FIELD_CHILDREN.'['.$k.']">'.$v->title.'</label>';
			$html .= '</dt>';

			$value = ($checked) ? $this->object[OBJ_FIELD_CHILDREN][$k] : PXApplication::_mapByDefaults('all');
			$this->disable = !strlen($checked);

			$html .= '<dd>'.$this->BuildDropDown(OBJ_FIELD_CHILDREN.'['.$k.']', $value, $values, 0, 0).'</dd>';
			$html .= '<dd>'.$quantity.' '.NumericEndingsRussian('объект', $quantity, array('ов', '', 'а', 'а', 'а', 'ов', 'ов', 'ов', 'ов', 'ов'));

			$this->disable = false;

			if ($quantity > 0) {
				$html .= ' &#151; отвязка невозможна';
			}

			$html .= '</dd>'."\n";
		}

		$html .= '</dl>';

		return $this->baseInputs('children').$html;
	}

	function AccessForm($accessList) {
		$html  = '<h2>На редактирование:</h2><p>'.$this->BuildDropDown("sys_accessmod", $this->object['sys_accessmod'], $accessList).'</p>';
		if (count($this->format->childs)) {
			$html .= '<h2>На добавление дочерних объектов:</h2><p>'.$this->BuildDropDown("sys_accessput", $this->object['sys_accessput'], $accessList).'</p>';
		}
		return $this->BaseInputs('access').$html;
	}

	function EditTextFileForm($file, $dir) {
		return  $this->BuildHidden('action', 'edit').
			$this->BuildHidden('area', 'file').
			$this->BuildHidden('mdir', str_replace(BASEPATH, '', $dir)).
			$this->BuildHidden('mfile', $file).
			$this->BuildText('filesource', htmlspecialchars(ReadFileToString($dir.'/'.$file)), '100%');
	}

	function removeForm() {
		$title = $this->object['title'];

		return $this->BaseInputs('remove').<<<HTML
			<h2>
				Вы уверены, что хотите удалить объект &laquo;<strong>{$title}</strong>&raquo;
			</h2>

			<p class="delete">
				<input name="ack" id="ack" type="checkbox">
				<label for="ack">Да! Я хочу удалить именно этот объект!</label>
			</p>
HTML;
	}

	function getAllowedForms($userLevel) {
		$forms = array();
		$forms['main'] = 'Основная информация';

		if (isset($this->object['id']) && $this->object['id'] != 0) {
			if (count($this->format->childs)) {
				$forms['children'] = 'Разрешенные дети';
			}

			if (!empty($this->format->references)) {
				$forms['links'] = 'Связи';
			}

			$forms['remove']   = 'Удаление';
		}

		return $forms;
	}
}

?>
