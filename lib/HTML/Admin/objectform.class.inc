<?php
class PXAdminObjectForm extends NLAbstractHTMLForm {
	var $format;
	var $object;

	var $area = 'objects';

	function PXAdminObjectForm($format, $object) {
		$this->NLAbstractHTMLForm();
		$this->format = $format;
		$this->object = $object;
	}

	function setArea($area){
		$this->area = $area;
	}

	function MainForm($datatype) {
		$html        = '';
		$defaultView = 'table';

		foreach($datatype->fieldGroups as $group) {
			$k = key($group);
			$v = current($group);
			unset($v['id']);

			$viewKlass = sprintf('PXWidgetFieldGroups%sView', ucfirst(strtolower($view = (empty($datatype->groups[$k]['view']) ? $defaultView : $datatype->groups[$k]['view']))));
			if(!class_exists($viewKlass)){
				FatalError("Unknown group view '{$view}' (undefined widget class '{$viewKlass}') in datatype '{$datatype->id}'");
			}
			$groupViewer = new $viewKlass($k, $v, $datatype, $this->object);
			$html .= $groupViewer->html();
		}

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
			$this->BuildHidden('area', $this->area).
			$this->BuildHidden('action', $action).
			$this->BuildHidden('format', $this->format->id);
	}

	function LinksForm($reference, $formatTo, $links, $pLinks, $objectsOnPage = 0, $page = 1, $count = 0, $fullCount = 0, &$layout, $onlyExistingLinks = false, $filters) {
		$displayFields = array();
		foreach ($formatTo->fields as $k => $f) {
			if ($f->listed) {
				$displayFields[$k] = $f;
			}
		}

		$html  = '<h2>Связи с объектами формата &laquo;'.$formatTo->title.'&raquo; ['.sizeof($links).' из '.$fullCount.']</h2>';
		if($reference->multiple){
			$html  .= '<!--%%OBJECT2JS%%-->';
		}
		$html .= '<table class="objects links">';
		
		$thead = ''; $enabledFilters = 0;
		foreach ($displayFields as $f) {
			if($f->storageType->filterable()){
				!($filterOn = isset($filters[$formatTo->id][$f->name])) OR $enabledFilters++;
				$thead .= '<th class="ref-filter-'.( $filterOn ? 'show' : 'hide').'" ondblclick="ShowHideFilter(this)" title="Двойной клик, чтобы скрыть/показать фильтр по этому полю"><span>'.$f->description.'</span>';
				$thead .= '<input type="text" name="filters['.$formatTo->id.']['.$f->name.']" value="'.($filterOn ? $filters[$formatTo->id][$f->name]: null).'" />';
				$thead .= '</th>';
			} else {
				$thead .= '<th>'.$f->description.'</th>';
			}
		}
		$html .= '<thead><tr><th class="c3 ref-filter-'.( $enabledFilters ? 'show' : 'hide').'"><input type="submit" title="Применить фильтр(ы)" value="&gt;&gt;&gt;"/></th>' . $thead;

		foreach ($reference->fields as $k=>$v) {
			$html .= '<th>'.$v->description.'</th>';
		}
		$html .= '</tr></thead><tbody>';
		#TODO remove


		$params = array('object' => $this->object);
		foreach ($pLinks as $k=>$v) {
			$iteration = 0;
			$linksIsset = isset($links[$v['id']]);
			do {
				$shown = false;
				$html .= '<tr'.($v['status'] == false ? ' class="hide"' : '').'>';
				$html .= '<td class="c3">';
				$html .= $this->BuildHidden($reference->name.'['.$iteration.']['.$v['id'].'][id]', $v['id']);
				$html .= $this->BuildCheckBox($reference->name.'['.$iteration.']['.$v['id'].'][on]', $linksIsset, NULL, $reference->name.'-'.$iteration.'-'.$v['id']);

				$menuTitle = (strlen($v['title']) > 10) ? trim(substr($v['title'], 0, 10)).'&hellip;' : $v['title'];
				$context = 'Context(event, \'edit\', '.$v['id'].', '.(int)($v['status']).', \''.$formatTo->id.'\',  \''.$menuTitle.'\', \'\', 1, 0, 0); return false;';
				$html .= '<a oncontextMenu="'.$context.'" href="javascript:EditContent(\''.$formatTo->id.'\', \''.$v['id'].'\')">';
				$html .= '<img src="i/icon/edit.gif" alt="Изменить &laquo;'.$v['id'].'&raquo;"></a>';
				$html .= '</td>';
				
				foreach ($displayFields as $k => $f) {
					$html .= '<td>';
					$html .= '<label checkbox="'.$reference->name.'-'.$iteration.'-'.$v['id'].'" for="'.$reference->name.'-'.$iteration.'-'.$v['id'].'" class="'.($linksIsset ? 'ref-linked' : 'ref-unlinked').'">';
					$html .= $f->displayType->buildCell($f, $v);
					$html .= '</label>';
					if($reference->multiple && $linksIsset && !$shown) {
						$html .= '<span title="Нажмите, чтобы добавить ещё одну ссылку для этого объекта" class="ref-plus-tr" onclick="appendLinksRow(this)">[ + ]</span>';
						$object2JS[$v['id']] = $iteration;
						$shown = true;
					}
					$html .= '</td>';
				}

				$params['other'] = $v;

				foreach ($reference->fields as $sk=>$sv) {
					$value = ($linksIsset) ? $links[$v['id']]['fields'][$sk] : htmlspecialchars($sv->defaultValue);
					$k = $reference->name.'['.$iteration.']['.$v['id'].'][fields]['.$sk.']';

					$sv->name = $k;

					$html .= '<td>'.$sv->displayType->buildInput($sv, array($k=>$value), $params).'</td>';
				}
				$html .= '</tr>';
				$iteration++;
			} while(!is_null($links[$v['id']] = & $links[$v['id']]['next'])); //Если есть связанные ссылки, продолжаем выполнение, пока не закончится linked list
		}
		
		if(isset($object2JS)){
			$js = '<script type="text/javascript">';
			$js .= "possibleNewRows['{$reference->name}'] = {";
			foreach($object2JS as $k => $v){
				$objs[] = "'{$k}':{$v}";
			}
			$js .= join(',', $objs);
			$js .= '}';
			$js .= '</script>';
			$html = str_replace('<!--%%OBJECT2JS%%-->', $js, $html);
		}
		
		$html .= '</tbody></table>';

		$html .= '<div class="perpage" style="zoom: 1;">';

		//Reset current objects pager for All/Linked switch
		if(isset($layout->getData[$formatTo->id . '_page'])){
			$currentPager = $layout->getData[$formatTo->id . '_page'];
			unset($layout->getData[$formatTo->id . '_page']);
		}
		$html .= '<a style="float: right;" href="'.$layout->_BuildHref($formatTo->id.'_exist', 1).'" '.( $onlyExistingLinks ? 'class="selected"': '').'>привязанные</a>';
		$html .= '<a style="float: right;" href="'.$layout->_BuildHref($formatTo->id.'_exist', 0).'" '.(!$onlyExistingLinks ? 'class="selected"': '').'>все</a>';
		if(isset($currentPager)){
			$layout->getData[$formatTo->id . '_page'] = $currentPager;
		}

		if(!is_bool($onlyExistingLinks)){
			$html .= $this->BuildHidden($formatTo->id.'_exist', $onlyExistingLinks ? 1 : 0);
		}

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

		return $html;
	}

	function childrenForm($types, $counts = NULL) {
		$values = array(
			PP_CHILDREN_FETCH_ALL      => 'Все',
			PP_CHILDREN_FETCH_SELECTED => 'Текущий',
			PP_CHILDREN_FETCH_NONE     => 'Никакие',
			PP_CHILDREN_FETCH_PAGED    => 'С постраничкой'
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

			$value = ($checked) ? $this->object[OBJ_FIELD_CHILDREN][$k] : PXTypeDescription::_mapByDefaults('all');
			$this->disable = !strlen($checked);

			$html .= '<dd>'.$this->BuildDropDown(OBJ_FIELD_CHILDREN.'['.$k.']', $value, $values, 0, 0).'</dd>';
			$html .= '<dd>'.$quantity.' '.NumericEndingsRussian($quantity, 'объект', 'объекта', 'объектов');

			$this->disable = false;

			if ($quantity > 0) {
				$html .= ' &#151; отвязка невозможна';
			}

			$html .= '</dd>'."\n";
		}

		$html .= '</dl>';

		return $html;
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

		return <<<HTML
			<h2>
				Вы уверены, что хотите удалить объект &laquo;<strong>{$title}</strong>&raquo;
			</h2>

			<p class="delete">
				<input name="ack" id="ack" type="checkbox">
				<label for="ack">Да! Я хочу удалить именно этот объект!</label>
			</p>
HTML;
	}
}
?>