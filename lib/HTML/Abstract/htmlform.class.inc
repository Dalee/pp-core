<?php

define('IMAGE_DEFAULT', 'i/v.gif');
define('FLASH_DEFAULT', 'i/v.swf');
define('FLASH_DEFAULT_FULL', '/admin/i/v.swf');

class NLAbstractHTMLForm {
	var $disable;

	function NLAbstractHTMLForm() {
		$this->disable = false;
	}

	function _DisabledStatus() {
		return isset($this->disable) && $this->disable ? ' disabled' : '';
	}

	static function BuildFormBegin($action='action.phtml', $method='POST', $enctype='multipart/form-data') {
		return '<form action="'.$action.'" method="'.$method.'" enctype="'.$enctype.'" class="edit">';
	}

	static function BuildFormEnd() {
		return '</form>';
	}

	static function BuildInputCheckBox($name, $on, $value=NULL, $id=NULL) {
		$html = '<input type="checkbox" class="checkbox" name="'.$name.'"';

		$html .= $id    ? ' id="'.$id.'"'       : '';
		$html .= $on    ? ' checked'            : '';
		$html .= $value ? ' value="'.$value.'"' : '';

		$html .= '>';
		return $html;
	}

	static function BuildCheckBox($name, $on, $value=NULL, $id=NULL) {
		return NLAbstractHTMLForm::BuildInputCheckBox($name, $on, $value, $id);
	}

	function BuildSubmit($value, $name=NULL) {
		return '<input type="submit"'.(!empty($name) ? ' name="'.$name.'"' : '').' value="'.$value.'" accesskey="s"'.$this->_DisabledStatus().'>';
	}

	function BuildButton($value, $onClickCode = '') {
		return '<button '.($onClickCode ? 'onclick="'.$onClickCode.'"' : '').$this->_DisabledStatus().'>'.$value.'</button>';
	}

	function BuildReset($value) {
		return '<input type="reset" value="'.$value.'" '.$this->_DisabledStatus().'>';
	}

	static function BuildClose($value) {
		return '<button onclick="window.close()">'.$value.'</button>';
	}

	static function BuildHref($key, $value) {
		$href = "?";

		foreach ($this->getData as $k=>$v) {
			$v = urlencode($v);
			if (!empty($v) && $k != $key) $href .= $k.'='.$v.'&';
		}

		$href .= $key.'='.$value.'&';
		$href = substr($href, 0, -1);

		return $href;
	}

	static function BuildHidden($name, $value) {
		return '<input type="hidden" name="'.$name.'" value="'.$value.'">';
	}

	static function BuildText($name, $value, $height) {
		if ($height == 0) {
			return '<input type="text" name="'.$name.'" value="'.$value.'">';
		} else {
			$height = strpos($height, '%') ? $height : $height.'px';
			return '<textarea name="'.$name.'" wrap="physical" style="height:'.$height.';">'.$value.'</textarea>';
		}
	}

	static function BuildPassword($name, $value, $canDelete=true) {
		$html  = '<div class="password">';
		$html .= '<input type="password" name="'.$name.'[type]"   onfocus="startCheckPassword(this, \''.$name.'\')">';
		$html .= '<input type="password" name="'.$name.'[retype]" onfocus="startCheckPassword(this, \''.$name.'\')">';
		if(strlen($value) == 32 && $canDelete) {
			$html .= '<input type="checkbox" name="'.$name.'[delete]" id="'.$name.'[delete]" class="checkbox"><label for="'.$name.'[delete]">удалить пароль</label>';
		}
		$html .= ' <span>Длина пароля должна быть не менее 6 символов. Хороший пароль должен включать в себя строчные и заглавные буквы латинского алфавита, а также цифры.</span>';
		$html .= '</div>';

		return $html;
	}

	static function BuildColor($name, $value) {
		$html  = '<div class="colorselect"><div id="colorBG'.$name.'" style="background-color: #'.$value.';"></div>';
		$html .= '<input type="text" name="'.$name.'" id="colorData'.$name.'" value="'.$value.'" readonly>';

		$html .= '<input type="hidden" id="colorRed'.$name.'"    value="'.hexdec(substr($value, 0, 2)).'">';
		$html .= '<input type="hidden" id="colorGreen'.$name.'"  value="'.hexdec(substr($value, 2, 2)).'">';
		$html .= '<input type="hidden" id="colorBlue'.$name.'"   value="'.hexdec(substr($value, 4, 2)).'">';

		$html .= '<button title="изменить цвет"';
		$html .= ' onclick="with(this.form)ctrlColorer.DoModal(this.form.colorData'.$name.', this.form.colorRed'.$name.', this.form.colorGreen'.$name.', this.form.colorBlue'.$name.', document.getElementById(\'colorBG'.$name.'\'));"';
		$html .= ' onFocus="this.blur();"></button>';

		$html .= '<iframe src="tools/colorer/index.html" name="color'.$name.'" id="color'.$name.'" scrolling="no" frameborder="0"></iframe>';
		$html .= '</div>';

		return $html;
	}

	static function BuildRichEdit($name, $value, $height) {
		$html  = '<textarea name="'.$name.'" style="width: 100%; height:'.$height.'px;">'.$value.'</textarea>';
		$html .= '<script type="text/javascript">editor_generate(\''.$name.'\');</script>';
		return $html;
	}

	static function BuildDateInline($name, $value) {
		return NLAbstractHTMLForm::BuildDateTime($name, $value, false, true, false, true);
	}

	static function BuildDateTimeInline($name, $value) {
		return NLAbstractHTMLForm::BuildDateTime($name, $value, true, true, false, true);
	}

	static function BuildDateTime($name, $value, $showTime, $showDate, $showCalendar, $valueNotNull = false) {
		$monthes = array(
			'',
			'января',
			'февраля',
			'марта',
			'апреля',
			'мая',
			'июня',
			'июля',
			'августа',
			'сентября',
			'октября',
			'ноября',
			'декабря'
		);

		if (!is_null($value)) {
			$matches = array();

			$value = trim($value);
			$value = substr($value, 0, 19);

			preg_match("/^((\d{1,2}).(\d{1,2}).(\d{4}))?\s?((\d{1,2})(:(\d{1,2})(:(\d{1,2}))?)?)?/", trim($value), $matches);

			if(isset($matches[1]) && strlen($matches[1])) {
				$day    = $matches[2];
				$month  = $matches[3];
				$year   = $matches[4];
			} else {
				$year   = date("Y");
				$month  = date("n");
				$day    = date("j");
			}

			if(isset($matches[5])) {
				$hour   = $matches[6];
				$minute = $matches[8];
				$second = $matches[10];
			} else {
				$hour   = $minute = $second = 0;
			}

		} elseif($valueNotNull) {
			$year   = date("Y");
			$month  = date("n");
			$day    = date("j");

			$hour   = date("G");
			$minute = date("i")*1;
			$second = date("s")*1;

		} else {
			$year   = NULL;
			$month  = NULL;
			$day    = NULL;

			$hour   = NULL;
			$minute = NULL;
			$second = NULL;
		}

		$html = '<div class="datetime">';

		if ($showTime) {
			$html .= '<select name="'.$name.'[hour]">';
			for ($i=0; $i<24;$i++) {
				$html .= '<option value="'.$i.'"'.($i == $hour ? ' selected' : '').'>'.sprintf("%02d", $i).'</option>';
			}
			$html .= '</select>';

			$html .= '<span>:</span>';

			$html .= '<select name="'.$name.'[minute]">';
			for ($i=0; $i<60;$i++) {
				$html .= '<option value="'.$i.'"'.($i == $minute ? ' selected' : '').'>'.sprintf("%02d", $i).'</option>';
			}
			$html .= '</select>';

			$html .= '<span>:</span>';

			$html .= '<select name="'.$name.'[second]">';
			for ($i=0; $i<60;$i++) {
				$html .= '<option value="'.$i.'"'.($i == $second ? ' selected' : '').'>'.sprintf("%02d", $i).'</option>';
			}
			$html .= '</select>';

			$html .= '<span></span>';
		} else {
			$html .= '<input type="hidden" name="'.$name.'[hour]"   value="'.sprintf("%02d", $hour)  .'">';
			$html .= '<input type="hidden" name="'.$name.'[minute]" value="'.sprintf("%02d", $minute).'">';
			$html .= '<input type="hidden" name="'.$name.'[second]" value="'.sprintf("%02d", $second).'">';
		}

		if ($showCalendar) {
			$html .= '<input type="hidden" name="'.$name.'[day]"   id="returnDay'.$name.'"   value="'.sprintf("%02d", $day)  .'">';
			$html .= '<input type="hidden" name="'.$name.'[month]" id="returnMonth'.$name.'" value="'.sprintf("%02d", $month).'">';
			$html .= '<input type="hidden" name="'.$name.'[year]"  id="returnYear'.$name.'"  value="'.$year                  .'">';

			$html .= '<input type="text" id="returnDate'.$name.'" value="'.(is_null($value) && !$valueNotNull ? '' : sprintf("%02d", $day).'.'.sprintf("%02d", $month).'.'.$year).'"  readonly>';
			$html .= '<iframe src="tools/calendar/index.html" name="calendar'.$name.'" id="calendar'.$name.'" scrolling="no" frameborder="0"></iframe>';

			$html .= '<button title="календарь" class="calendar" onFocus="this.blur();"
				  onclick="with(this.form)ctrlCalendar.DoModal(this.form.returnDate'.$name.', this.form.returnDay'.$name.', this.form.returnMonth'.$name.', this.form.returnYear'.$name.');"';
			$html .= '></button>';

			$html .= '<button onclick="with(this.form)ctrlCalendar.SetNull(this.form.returnDate'.$name.', this.form.returnDay'.$name.', this.form.returnMonth'.$name.', this.form.returnYear'.$name.');"';
			$html .= '>обнулить</button>';

		} elseif ($showDate) {
			$html .= '<select name="'.$name.'[day]">';
			for ($i=1; $i<=31;$i++) {
				$html .= '<option value="'.$i.'"'.($i == $day? ' selected' : '').'>'.sprintf("%02d", $i).'</option>';
			}
			$html .= '</select>';

			$html .= '<span>/</span>';

			$html .= '<select name="'.$name.'[month]">';
			for ($i=1; $i<=12;$i++) {
				$html .= '<option value="'.$i.'"'.($i == $month ? ' selected' : '').'>'.$monthes[$i].'</option>';
			}
			$html .= '</select>';

			$html .= '<span>/</span>';

			$html .= '<select name="'.$name.'[year]">';
			for ($i=date("Y")+1; $i >= 1970; $i--) {
				$html .= '<option value="'.$i.'"'.($i == $year ? ' selected' : '').'>'.$i.'</option>';
			}
			$html .= '</select>';

		} else {
			$html .= '<input type="hidden" name="'.$name.'[day]"   value="'.sprintf("%02d", $day).'">';
			$html .= '<input type="hidden" name="'.$name.'[month]" value="'.sprintf("%02d", $month).'">';
			$html .= '<input type="hidden" name="'.$name.'[year]"  value="'.$year.'">';
		}

		$html .= '</div>';

		return $html;
	}

	static function BuildMultimedia($name, $value, $type='image', $entry=NULL) {
		$distension = is_null($entry) ? '' : '['.$entry.']';

		if($type !== 'file') {
			$w = isset($value['width'])  ? $value['width']  : '100';
			$h = isset($value['height']) ? $value['height'] : '100';
			$p = isset($value['path'])   ? $value['path']   : ($type == 'image' ? IMAGE_DEFAULT : FLASH_DEFAULT);

			if($w > 100 || $h > 100) {
				$width  = ($w - $h) > 0 ? 100 : floor(100 * $w/$h);
				$height = ($w - $h) > 0 ? floor(100 * $h/$w) : 100;
			} else {
				$width  = $w;
				$height = $h;
			}
		}

		$html  = '<div class="imagepreview">';
		if($type === 'flash') {
			$html .= '<div><script type="text/javascript">ShowFlash(\'i/l.swf?path='.$p.'\', 100, 100, \''.$name.$distension.'src\');</script></div>';

		} elseif($type === 'image') {
			$html .= '<div><img src="'.$p.'" id="'.$name.$distension.'src" width="'.$width.'" height="'.$height.'"></div>';
		}

		$html .= '<input class="file" type="file" id="'.$name.$distension.'" name="'.$name.(!is_null($entry) && strlen($entry) ? '_edit' : '').$distension.'" onpropertychange="Preview(this.value, \''.$type.'\', \''.$name.$distension.'\');"><br>';

		if($type == 'file') {
			$html .= '<input type="text" readonly value="'.$value['filename'].'" >';
			$html .= '<span>имя</span>';

			$html .= '<input type="text" readonly value="'.$value['size'].'" >';
			$html .= '<span>размер</span>';

			$html .= '<input type="text" readonly value="'.$value['type'].'" >';
			$html .= '<span>тип</span>';

			$value['path']   = $value['fullpath'];
			$value['width']  = 0;
			$value['height'] = 0;
		} else {
			$html .= '<input id="'.$name.$distension.'width"  type="text" readonly value="'.$value['width'].'" >';
			$html .= '<span>ширина</span>';
			$html .= '<input id="'.$name.$distension.'height" type="text" readonly value="'.$value['height'].'">';
			$html .= '<span>высота</span>';
			$html .= '<input id="'.$name.$distension.'size"   type="text" readonly value="'.($value['path'] ? filesize(BASEPATH.'/site/htdocs/'.$value['path']) : NULL).'">';
			$html .= '<span>размер</span>';
		}

		if (!is_null($value['path'])) {
			$html .= '<br>';

			$html .= '<button onclick="window.open(\''.$value['path'].'\'); return false;">просмотр</button>';
			$html .= '<button onclick="return ToClipboardMulti(\''.$value['path'].'\', '.(int)$value['width'].', '.(int)$value['height'].', \''.$type.'\')">HTML в буфер</button>';

			$html .= '<br>';

			$picName = substr($value['path'], strrpos($value['path'], '/')+1);
			$html .= '<input class="checkbox" type="checkbox" id="'.$name.'_remove'.$distension.'" name="'.$name.'_remove['.$picName.']">';
			$html .= '<label for="'.$name.'_remove'.$distension.'">удалить файл</label>';
		}

		$html .= '</div>';
		return $html;
	}

	static function BuildLinkToFile($name, $value) {
		$lastDir = isset($_COOKIE['fileManagerDir']) && strlen(trim($_COOKIE['fileManagerDir'])) ? 'site/htdocs/'.trim($_COOKIE['fileManagerDir']) : '';

		$html  = '<div class="imagepreview">';
		$html .= '<input type="hidden" name="'.$name.'[filename]" id="'.$name.'_filename" value="'.$value['filename'].'">';
		$html .= '<input type="hidden" name="'.$name.'[dir]"      id="'.$name.'_dir"      value="'.$value['dir'].'">';

		$html .= '<input id="'.$name.'_fullpath" type="text" value="'.(is_null($value['filename']) ? '' : $value['dir'].$value['filename']).'" readonly/>';
		$html .= '<span>файл</span>';

		$html .= '<input id="'.$name.'_size"     type="text" value="'.(is_null($value['filename']) ? '' : $value['size']).'"                       readonly name="'.$name.'[size]" />';
		$html .= '<span>размер</span>';

		$html .= '<button type="button" onclick="EditLinkToFile(\''.$name.'\', (this.form.'.$name.'_dir.value.length ? \'htdocs\'+this.form.'.$name.'_dir.value : \''.$lastDir.'\'));">изменить</button>';

		if (!is_null($value['filename'])) {
			$html .= '<br>';
			$html .= '<button onclick="javascript: window.open(this.form.'.$name.'_fullpath.value)">просмотр</button>';
			$html .= '<button onclick="return UnLinkFile(\''.$name.'\', 1);"                             >отвязать</button>';
		}

		return $html;
	}

	static function BuildImage($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'image');
	}

	static function BuildFlash($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'flash');
	}

	static function BuildFile($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'file');
	}

	static function BuildDropDown($name, $value, $values, $disable = false) {

        $html = '<select name="'.$name.'"'.($disable ? ' disabled' : ' ').'>';

        foreach ($values as $k=>$v) {
			$html .= '<option value="'.$k.'"'.($k == $value ? ' selected' : '').'>'.$v.'</option>';
		}

		if(!in_array($value, array_keys($values))) {
			$html .= '<option value="'.$value.'" selected>[нет в списке - '.$value.']</option>';
		}
		$html .= '</select>';

		return $html;
	}


	static function BuildTable($name, $value, $cols, $height) {
		$colsWidth = array();
		for ($i=0; $i<$cols; $i++) {
			$colsWidth[] = 1;
		}

		return NLAbstractHTMLForm::BuildTableByCols($name, $value, $colsWidth, $height);
	}

	static function BuildTableByCols($name, $value, $colsWidth, $height) {
		$idx     = 0;
		$value[] = NULL;
		$html  = '<table id="table'.$name.'" class="datagrid">';

		$html .= '<colgroup>';
		for ($i=0; $i<count($colsWidth); $i++) {
			$width = ceil(100*$colsWidth[$i]/array_sum($colsWidth));
			$html .= '<col width="'.$width.'%">';
		}
		$html .= '</colgroup>';

		foreach ($value as $row) {
			$html .= '<tr>';

			for ($i=0; $i<count($colsWidth); $i++) {
				$html .= '<td>';

				$currentValue = (isset($row[$i])) ? $row[$i] : NULL;
				$html .= NLAbstractHTMLForm::BuildText($name.'['.$idx.'][]', $currentValue, $height);

				$html .= '</td>';
			}
			$html .= '</tr>';

			$idx++;
		}
		$html .= '</table>';

		$html .= '<button class="addrow" onclick="AddRowNew(\''.$name.'\')">Добавить</button>';
		return $html;
	}

	static function BuildIPAddr($name, $value) {
		$html = '<div class="ip">';
		$html .= '<input type="text" maxlength="3" name="'.$name.'[0]" value="'.$value[0].'" class="first">';
		$html .= '<input type="text" class="delim" value="." readonly tabindex="-1">';
		$html .= '<input type="text" maxlength="3" name="'.$name.'[1]" value="'.$value[1].'" class="middle">';
		$html .= '<input type="text" class="delim" value="." readonly tabindex="-1">';
		$html .= '<input type="text" maxlength="3" name="'.$name.'[2]" value="'.$value[2].'" class="middle">';
		$html .= '<input type="text" class="delim" value="." readonly tabindex="-1">';
		$html .= '<input type="text" maxlength="3" name="'.$name.'[3]" value="'.$value[3].'" class="last">';
		$html .= '</div>';
		return $html;
	}

	static function BuildUserType($name, $value, $color, &$field) {
		return $field->displayType;
	}
/*
 *  todo: remove this shit outta of here
 */
	// static function for request!!!
	function RequestGetUserVar($name, &$request, &$field) {
		return $request->postData->_GetVar($name);
	}
}

?>
