<?php
class PXAdminForm extends PXAdminComplexWidget {
	private $action;
	private $area;
	private $object;
	private $format;
	private $layout;
	private $childTypes;
	private $childCount = null;
	private $linksParam;
	public  $disable;

	const BEFORE_CONTENT        = 0;
	const CONTENT               = 1;
	const AFTER_CONTENT         = 2;

	const PREPEND_LEFTCONTROLS  = 3;
	const LEFTCONTROLS          = 4;
	const APPEND_LEFTCONTROLS   = 5;

	const PREPEND_RIGHTCONTROLS = 6;
	const RIGHTCONTROLS         = 7;
	const APPEND_RIGHTCONTROLS  = 8;
	
	const TITLE                = 9;
	const MENU                 = 10;

	public $blocks = array( //set up available subwidgets areas
		self::PREPEND_LEFTCONTROLS => array(),
		self::LEFTCONTROLS         => array(),
		self::APPEND_LEFTCONTROLS  => array(),

		self::PREPEND_RIGHTCONTROLS =>	array(),
		self::RIGHTCONTROLS         => array(),
		self::APPEND_RIGHTCONTROLS  => array(),
		
		self::MENU                 => array(),
		self::TITLE                => array(),

		self::BEFORE_CONTENT       => array(),
		self::CONTENT              => array(),
		self::AFTER_CONTENT        => array());

	function __construct($object, $format) {
		$this->object = $object;
		$this->layout = PXRegistry::getlayout();
		$this->app    = PXRegistry::getApp();

		$this->format = $format;
	}

	function setAction($action) {
		$this->action = $action;
	}

	function setArea($area) {
		$this->area = $area;
	}

	function setChildren($type, $count = null) {
		$this->childTypes = $type;
		$this->childCount = $count;
	}

	function setTitle($title) {
		$this->title = $title;
	}

	function setLinks($linksParam) {
		$this->linksParam = $linksParam;
	}

	function form() {
		$this->mainForm();
	}

	static function EditTextFileForm($file, $dir) {
		return  NLAbstractHTMLForm::BuildHidden('action', 'edit').
			NLAbstractHTMLForm::BuildHidden('area', 'file').
			NLAbstractHTMLForm::BuildHidden('mdir', str_replace(BASEPATH, '', $dir)).
			NLAbstractHTMLForm::BuildHidden('mfile', $file).
			NLAbstractHTMLForm::BuildText('filesource', htmlspecialchars(ReadFileToString($dir.'/'.$file)), '100%');
	}

	function getForm() {
		$this->decorate();
		$this->layout->setOuterForm('action.phtml', 'POST', 'multipart/form-data');

		$html[] =  join($this->blocks[self::BEFORE_CONTENT]);

		switch ($this->action) {
			case 'main':
				$content = $this->mainForm();
				break;

			case 'children':
				$content = $this->childrenForm();
				break;

			case 'remove':
				$content = $this->removeForm();
				break;

			case 'links':
				$content = $this->linksForm();
				break;

			case 'auditlog':
				$content = $this->auditlogForm();
				break;
		}
		
		$this->setMenu();
		$this->setControls();
		$this->showTitle();
		
		if (strlen($_ = join($this->blocks[self::CONTENT]))) {
			$content = $_;
		}
	
		$html[] =  $content;
		$html[] =  join($this->blocks[self::AFTER_CONTENT]);

		$this->layout->append('OUTER.CONTENT', join($html));
	}

	public function leftControls() {
		$control = NLAbstractHTMLForm::BuildSubmit('Сохранить') . NLAbstractHTMLForm::BuildSubmit('Сохранить и закрыть', "close");

		if (strlen($_ = join($this->blocks[self::LEFTCONTROLS]))) {
			$control  = $_;
		}

		PXRegistry::getLayout()->assign('OUTER.LEFTCONTROLS',
			  join($this->blocks[self::PREPEND_LEFTCONTROLS]) 
			. $control 
			. join($this->blocks[self::APPEND_LEFTCONTROLS]));
	}

	public function rightControls() {
		$control = NLAbstractHTMLForm::BuildReset('Отменить') . NLAbstractHTMLForm::BuildClose('Закрыть');
		
		if (strlen($_ = join($this->blocks[self::RIGHTCONTROLS]))) {
			$control  = $_;
		}

		PXRegistry::getLayout()->assign('OUTER.RIGHTCONTROLS',
			join($this->blocks[self::PREPEND_RIGHTCONTROLS]) 
			. $control 
			. join($this->blocks[self::APPEND_RIGHTCONTROLS]));
	}

	private function setControls() {
		$this->leftControls();
		$this->rightControls();

		$control = join('', array(NLAbstractHTMLForm::BuildHidden('id', ((isset($this->object['id'])) ? $this->object['id'] : '')),
	
		NLAbstractHTMLForm::BuildHidden('area', $this->area),
		NLAbstractHTMLForm::BuildHidden('action', $this->action),
		NLAbstractHTMLForm::BuildHidden('format', $this->format->id)));

		$this->layout->append('OUTER.CONTENT', $control);
	}

	private function setMenu() {
		$menuItems = array();
		$menuItems['main'] = 'Основная информация';

		if (isset($this->object['id']) && $this->object['id'] != 0) {
			if (count($this->format->childs)) {
				$menuItems['children'] = 'Разрешенные дети';
			}

			if (!empty($this->format->references)) {
				$menuItems['links'] = 'Связи';
			}

			$menuItems['auditlog'] = 'Журнал';
			$menuItems['remove']   = 'Удаление';
		}
//		$menuItems = join($this->blocks[self::MENU]);
		$this->layout->setMenu($menuItems, $this->action, 'action');
	}

	private function showTitle() {
		if(strlen($_ = join($this->blocks[self::TITLE]))) {
			$this->title = $_;
		}

		$this->layout->assignTitle($this->title);
	}

	private function mainForm() {
		$html        = array();
		$defaultView = 'table';

		foreach($this->format->fieldGroups as $group) {
			$k = key($group);
			$v = current($group);
			unset($v['id']);

			$viewKlass = sprintf('PXWidgetFieldGroups%sView', ucfirst(strtolower($view = (empty($this->format->groups[$k]['view']) ? $defaultView : $this->format->groups[$k]['view']))));
			if(!class_exists($viewKlass)){
				FatalError("Unknown group view '{$view}' (undefined widget class '{$viewKlass}') in datatype '{$this->format->id}'");
			}
			$groupViewer = new $viewKlass($k, $v, $this->format, $this->object);
			$html[] = $groupViewer->html();
	}

		if (isset($this->object['sys_created']) || isset($this->object['sys_modified']) || isset($this->object['ownerlogin'])) {
			$html[] = '<dl class="sysinfo">';
			$html[] = isset($this->object['sys_created'])  ? '<dt>Создан:  </dt><dd>'.substr($this->object['sys_created'],  0, 19).'</dd>' : '';
			$html[] = isset($this->object['sys_modified']) ? '<dt>Изменен: </dt><dd>'.substr($this->object['sys_modified'], 0, 19).'</dd>' : '';
			$html[] = isset($this->object['ownerlogin'])   ? '<dt>Владелец:</dt><dd>'.$this->object['ownerlogin'].'</dd>'                  : '';
			$html[] = '</dl>';
		}

		return join($html);
	}

	private function removeForm() {
		return <<<HTML
			<h2>
				Вы уверенны, что хотите удалить объект &laquo;<strong>{$this->object['title']}</strong>&raquo;
			</h2>
 
			<p class="delete">
				<input name="ack" id="ack" type="checkbox">
				<label for="ack">Да я хочу удалить именно этот объект!</label>
			</p>
HTML;
	}

	private function auditlogForm() {
		require_once BASEPATH . '/libpp/lib/Logger/Audit/wrapper.class.inc';
		
		$this->disable = true; // restrict modify object from audit log view, because it is wrong behaviour

		$audit_wrapper = new PXAdminAuditWrapper();
		$audit_wrapper->frame =  "OUTER.CONTENT";

		$audit_wrapper->filters = array(
			'format' => $this->format->id,
			'id'     => $this->object['id']);

		$audit_wrapper->hasFilterFrom = false;

		$audit_wrapper->init();

		if(!$audit_wrapper->count) return;

		$audit_wrapper->caption  = '<a href="./?area=auditlog&format='.$this->format->id.'&id='.$this->object['id'].'" target="_blank">Журнал аудита</a>, ';
		$audit_wrapper->caption .= $audit_wrapper->count.' '.NumericEndingsRussian($audit_wrapper->count, 'запись', 'записи', 'записей');
		$audit_wrapper->caption .= ' для текущего объекта';

		$audit_wrapper->render();
	}


	private function linksForm() {
		if(empty($this->linksParam)) {
			return;
		}

		$html = array();
		foreach($this->linksParam as $k => $v) {
			$html[] = $this->linksTable($v);
		}
		return join('', $html);
	}

	private function linksTable($params) {
		$displayFields = array();
		extract($params);
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
				$thead .= '<th class="ref-filter-'.( $filterOn ? 'show' : 'hide').'" ondblclick="ShowHideFilter(this)" title="Двоиной клик, чтобы скрыть/показать фильтр по этому полю"><span>'.$f->description.'</span>';
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
				$html .= NLAbstractHTMLForm::BuildHidden($reference->name.'['.$iteration.']['.$v['id'].'][id]', $v['id']);
				$html .= NLAbstractHTMLForm::BuildCheckBox($reference->name.'['.$iteration.']['.$v['id'].'][on]', $linksIsset, NULL, $reference->name.'-'.$iteration.'-'.$v['id']);
 
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
						$html .= '<span title="Нажмите, чтобы добавить еще одну ссылку для этого объекта" class="ref-plus-tr" onclick="appendLinksRow(this)">[ + ]</span>';
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
			} while(!is_null($links[$v['id']] = & $links[$v['id']]['next']));
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
		$html .= '<a style="float: right;" href="'.$this->layout->_BuildHref($formatTo->id.'_exist', 1).'" '.( $onlyExistingLinks ? 'class="selected"': '').'>Привязанные</a>';
		$html .= '<a style="float: right;" href="'.$this->layout->_BuildHref($formatTo->id.'_exist', 0).'" '.(!$onlyExistingLinks ? 'class="selected"': '').'>Все</a>';
		if(isset($currentPager)){
			$layout->getData[$formatTo->id . '_page'] = $currentPager;
		}
 
		if(!is_bool($onlyExistingLinks)){
			$html .= NLAbstractHTMLForm::BuildHidden($formatTo->id.'_exist', $onlyExistingLinks ? 1 : 0);
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
				$html .= '<A href="'.$this->layout->_BuildHref($formatTo->id.'_page', $prev).'">';
				$html .= '<IMG src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
				$html .= '</A>';
			}
 
			for ($i=$start; $i<$max; $i++) {
				$html .= '<A';
 
				if ($i == $page) {
					$html .= ' class="selected"';
				}
 
				$html .= ' href="'.$this->layout->_BuildHref($formatTo->id.'_page', $i).'" title="Страница '.$i.'">'.$i.'</A>';
			}
 
			if (isset($next) && $next > 0) {
				$html .= '<A href="'.$this->layout->_BuildHref($formatTo->id.'_page', $next).'">';
				$html .= '<IMG src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
				$html .= '</A>';
			}
 
		}
		$html .= '</DIV>';
 
		return $html;
	}

	private function childrenForm() {
		$values = array(
			PP_CHILDREN_FETCH_ALL      => 'Все',
			PP_CHILDREN_FETCH_SELECTED => 'Текущий',
			PP_CHILDREN_FETCH_NONE     => 'Никакие',
			PP_CHILDREN_FETCH_PAGED    => 'С постраничкой'
		);
 
		$name = OBJ_FIELD_CHILDREN;
		$html[] = <<<HTML
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
 
		$html[] = '<dl class="allowed">'."\n";
 
		foreach ($this->childTypes as $k=>$v) {
			$quantity = (isset($this->childCount[$k])) ? $this->childCount[$k] : 0;
 
			if (!in_array($k, $this->format->childs)) {
				continue;
			}
 
			$checked  = isset($this->object[OBJ_FIELD_CHILDREN][$k]) ? ' checked'  : '';
			$disabled = $quantity > 0                                ? ' disabled' : '';
			$onClick  = ' onclick="changeChildrenDropDown(this.form, \''.$k.'\', this.checked);"';
 
			$html[] = '<dt>';
			$html[] = '<input type="checkbox"'.$onClick.$checked.$disabled.' id="'.OBJ_FIELD_CHILDREN.'['.$k.']">';
			$html[] = '<label for="'.OBJ_FIELD_CHILDREN.'['.$k.']">'.$v->title.'</label>';
			$html[] = '</dt>';
 
			$value = ($checked) ? $this->object[OBJ_FIELD_CHILDREN][$k] : PXTypeDescription::_mapByDefaults('all');
			$this->disable = !strlen($checked);

			$html[] = '<dd>'.NLAbstractHTMLForm::BuildDropDown(OBJ_FIELD_CHILDREN.'['.$k.']', $value, $values, 0, 0).'</dd>';
			$html[] = '<dd>'.$quantity.' '.NumericEndingsRussian($quantity, 'объект', 'объекта', 'объектов');
 
			$this->disable = false;
 
			if ($quantity > 0) {
				$html[] = ' &#151; отвязка невозможна';
			}
 
			$html[] = '</dd>'."\n";
		}
 
		$html[] = '</dl>';
 
		return join('', $html);
	}
}
?>
