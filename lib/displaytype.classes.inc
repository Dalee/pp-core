<?php

/**
 * Базовый класс описывающий ввод, вывод
 * (и что-то еще) полей объекта с определеннм
 * displayType
 *
 * Типичный способ обращения к статическим методам класса:
 * PXDisplayType::callMethod($method, $field, $object, $param);
 *
 * Описывается displayType обычно в таком формате ИМЯ|ПАРАМЕТР1|ПАРАМЕТР2
 * В ПАРАМЕТР1 обычно указывается ширина контрола,
 * в настоящее время она нигде не используется, сохраняется
 * совместимости для
 */
class PXDisplayType {
	/**
	 * Вызывает нужный метод нужного класса
	 *
	 * @param String             $method
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function callMethod($method, $field, $object, $param=NULL) {
		return PXFieldUtil::callMethod($method, $field, $object, $param, __CLASS__, 'displayType');
	}

	/**
	 * Вызывается в PXRequest::GetContentObject при
	 * извлечении полей объекта из запроса
	 * Возвращает значение поля
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->getVar($field->name);
	}


	/**
	 * Вызывается в PXDatabase::ModifyContentObject перед
	 * нормализацией полей объекта
	 * Возвращает флаг, пропустить ли нормализацию поля
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Boolean
	 */
	static function preModifyObject($field, $object, $param=NULL) {
		return false;
	}

	/**
	 * Возвращает html для строки с полем объекта в
	 * главной форме PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildRow($field, $object, $param=NULL) {
		$even = PXDisplayType::__even($param['inputClass']);

		$html = "\n";
		$html .= '<tr'.(!$even ? ' class="even"' : '' ).'>';

		$html .= '<th>'.$field->description.': </th>';
		$html .= '<td>';

		$html .= PXDisplayType::callMethod('buildInput', $field, $object, $param);

		$html .= '</td>';
		$html .= '</tr>';

		return $html;
	}

	/**
	 * Возвращает html для форм Картинки/Файлы PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildArray($field, $object, $param=NULL) {
    }

	/**
	 * Возвращает html для строки с полем объекта в
	 * формах Картинки/Файлы PXAdminObjectForm
	 *
	 * @param String             $name
	 * @param Array              $value
	 *
	 * @return String
	 */
	static function buildArrayItem($name, $value) {
	}

	static function __even($className) {
		static $even = false;

		$even = call_user_func(
			array(
				$className,
				'getRow'
			),
			$even
		);

		return $even;
	}

	/**
	 * Возвращает чет/нечет строки с полем объекта в
	 * формах PXAdminObjectForm
	 *
	 * @param Boolean            $even
	 *
	 * @return Boolean
	 */
	static function getRow($even) {
		return !$even;
	}

	/**
	 * Возвращает html с элементами ввода для поля объекта в
	 * формах PXAdminObjectForm
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildInput($field, $object, $param=NULL) {
		return '<span class="error">Вывод поля типа '.$field->displayType.' еще не описан</span>';
	}

	/**
	 * Возвращает html с выводом значения поля объекта в
	 * таблицах PXAdminTable
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $field->displayType.' - '.$value;
	}
}


/**
 * Скрытое поле
 *
 * storageType может быть любым, но как правило integer
 * displayType HIDDEN
 */
class PXDisplayTypeHidden extends PXDisplayType {
	static function buildRow($field, $object, $param=NULL) {
		$value = (isset($object[$field->name]) && !is_array($object[$field->name])) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildHidden($field->name, $value);
	}

	static function getRow($even) {
		return $even;
	}

	static function preModifyObject($field, $object, $param=NULL) {
		return (bool)$param['preserveHidden'];
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $value;
	}
}


/**
 * Нередактируемое поле
 *
 * storageType может быть любым, но как правило integer или string
 * displayType STATIC
 */
class PXDisplayTypeStatic extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = isset($object[$k]) ? htmlspecialchars(is_array($object[$k]) ? print_r($object[$k], true) : $object[$k]) : NULL;

		$values = isset($field->values) ? $field->values->GetList() : array();
		return NLAbstractHTMLForm::BuildHidden($field->name, $value).(isset($values[$value]) ? $values[$value] : $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return isset($field->values->values[$value]) && isset($field->values->values[$value]['title']) ? $field->values->values[$value]['title'] : $value;
	}

	static function preModifyObject($field, $object, $param=NULL) {
		return (bool)$param['preserveHidden'];
	}
}



/**
 * Текстовое поле
 *
 * storageType string, integer или float
 * displayType TEXT|width|height
 *             width не используется
 *             если height = 0, то строится <input type="text">
 *             если height > 0, то строится <textarea> и height это ее высота в пикселях
 */
class PXDisplayTypeText extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildText($field->name, $value, $field->displayTypeArgs[1]);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = strip_tags($object[$field->name]);
		return strlen($value) > 50 ? substr($value, 0, 50).'&hellip;' : $value;
	}
}


/**
 * Поле для смены пароля
 *
 * storageType string
 * displayType PASSWORD
 */
class PXDisplayTypePassword extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) && strlen($object[$field->name]) == 32 ? $object[$field->name] : NULL;
		return NLAbstractHTMLForm::BuildPassword($field->name, $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		return strlen($object[$field->name]) == 32 ? PXDisplayTypeCheckbox::buildCell($field, array($field->name => true), $param) : '';
	}

	static function getFromRequest($field, $object, $param=NULL) {
		$tmp = $param['request']->postData->_GetArrayVar($field->name);

		$value = (isset($tmp['type']) && isset($tmp['retype']) && strlen($tmp['type']) && ($tmp['type'] === $tmp['retype'])) ? trim($tmp['type']) : null;

		if(strlen($value) && !is_null($value)) {
			$value = md5($value);
		}

		if(isset($tmp['delete']) && $tmp['delete'] === 'on') {
			$value = "\n\t\n";
		}

		return $value;
	}

	static function preModifyObject($field, &$object, $param=NULL) {
		if(!strlen($object[$field->name])) {
			return true;
		}

		if($object[$field->name] === "\n\t\n") {
			$object[$field->name] = null;
		}
	}
}


/**
 * Визуальный редактор
 *
 * storageType string
 * displayType RICHEDIT|width|height
 *             width не используется
 *             height это высота в пикселях
 */
class PXDisplayTypeRichedit extends PXDisplayTypeText {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildRichEdit($field->name, $value, $field->displayTypeArgs[1]);
	}
}


/**
 * Таблица
 *
 * storageType serialized
 * displayType TABLE|col1width-..-colNwidth|height
 *             col1width-..-colNwidth относительная ширина столбцов, например 1-2-1-1
 *             если height = 0, то в каждой ячейке таблицы будет использоваться <input type="text">
 *             если height > 0, то в каждой ячейке таблицы будет использоваться <textarea> и height это ее высота в пикселях
 */
class PXDisplayTypeTable extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialcharsArray($object[$field->name]) : array();
		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, explode("-", $field->displayTypeArgs[0]), $field->displayTypeArgs[1]);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}
}


/**
 * ip адрес
 *
 * storageType integer
 * displayType IPADDR
 */
class PXDisplayTypeIpaddr extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = explode(".", long2ip(isset($object[$k]) ? $object[$k] : 0));

		return NLAbstractHTMLForm::BuildIPAddr($k, $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		return long2ip(isset($object[$field->name]) ? $object[$field->name] : 0);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		if ($param['request']->getHttpMethod() == 'POST') {
			return $param['request']->postData->_GetIPAddrVar($field->name);
		} else {
			return $param['request']->getData->_GetIPAddrVar($field->name);
		}
	}
}


/**
 * Дата и время (с календарем)
 *
 * storageType timestamp
 * displayType TIMESTAMP
 */
class PXDisplayTypeTimestamp extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, true, true, true);
	}

	static function buildCell($field, $object, $param=NULL) {
		return $object[$field->name];
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetTimeStampVar($field->name);
	}
}


/**
 * Время (дата скрыта, но как правило текущая,
 * поэтому делать сортировку по этому полю не стоит)
 *
 * storageType timestamp
 * displayType TIME
 */
class PXDisplayTypeTime extends PXDisplayTypeTimestamp {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, true, false, true);
	}

	static function buildCell($field, $object, $param=NULL) {
		return substr($object[$field->name], 11);
	}
}


/**
 * Дата (время скрыто, но как правило текущее)
 *
 * storageType timestamp
 * displayType DATE
 */
class PXDisplayTypeDate extends PXDisplayTypeTimestamp {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, false, true, true);
	}

	static function buildCell($field, $object, $param=NULL) {
		return substr($object[$field->name], 0, 10);
	}
}


/**
 * Checkbox
 *
 * storageType boolean
 * displayType CHECKBOX
 */
class PXDisplayTypeCheckbox extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = (isset($object[$field->name])  && $object[$field->name] == true) ? true : false;
		return NLAbstractHTMLForm::BuildInputCheckBox($field->name, $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return $value ? '<div class="truecheck" title="да"></div>' : '';
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetCheckBoxVar($field->name);
	}
}


/**
 * Выбора цвета
 *
 * storageType string / VARCHAR(6)
 * displayType COLOR
 */
class PXDisplayTypeColor extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildColor($field->name, $value);
	}
	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return '<div class="rgbcolor"><div style="background-color: #'.$value.';" title="#'.$value.'"></div></div>';
	}
}


/**
 * Выпадающий список
 *
 * storageType как правило integet, иногда string
 * displayType DROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeDropdown extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $field->values->GetList());
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return isset($field->values->values[$value]) && isset($field->values->values[$value]['title']) ? $field->values->values[$value]['title'] : '';
	}
}
/**
 * Выпадающий список с деревом разделов (как правило поле parent)
 *
 * storageType integet
 * displayType PARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeParentdropdown extends PXDisplayTypeDropdown {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $param['parents']);
	}
}
/**
 * Выпадающий список с деревом объектов этого PXTypeDescription типа (как правило поле pid)
 *
 * storageType integet
 * displayType SELFPARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeSelfparentdropdown extends PXDisplayTypeParentdropdown {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $param['selfParents']);
	}
}





/**
 * Список картинок
 *
 * storageType imagesarray
 * displayType IMAGESARRAY
 */
class PXDisplayTypeImagesarray extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$object[$field->name] = isset($object[$field->name]) ? $object[$field->name] : array();

		$items = array_merge(
			$object[$field->name],
			array(
				'null' => array(
					'width'  => null,
					'height' => null,
					'path'   => null,
					'entry'  => ''
				)
			)
		);

		$html = '';

		foreach($items as $value) {
			$html .= call_user_func(
				array(
					$param['inputClass'],
					'buildArrayItem'
				),
				$field->name,
				$value
			);
		}


		return $html;
	}

	static function buildArrayItem($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'image', $value['entry']);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetImageVar($field->name, TRUE);
	}

	static function buildCell($field, $object, $param=NULL) {
		$html = '';

		foreach($object[$field->name] as $k=>$value) {
			$html .= call_user_func(
				array(
					$param['inputClass'],
					'buildCellHtml'
				),
				$value,
				$field->name.$object['id'].$k
			);
		}

		return $html;
	}

	static function buildCellHtml($value) {
		return PXDisplayTypeImage::buildCellHtml($value);
	}
}
/**
 * Список файлов
 *
 * storageType filesarray
 * displayType FILESARRAY
 */
class PXDisplayTypeFilesarray extends PXDisplayTypeImagesarray {
	static function buildArrayItem($name, $value) {
		if(!strlen($value['entry'])) {
			$value = array(
				'filename' => null,
				'size'     => null,
				'fullpath' => null,
				'type'     => null,
				'entry'    => ''
			);
		}

		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'file', $value['entry']);
	}

	static function buildCell($field, $object, $param=NULL) {
		$html = '';

		foreach($object[$field->name] as $item) {
			$html .= PXDisplayTypeFile::buildCellHtml($item);
		}

		return $html;
	}
}
/**
 * Список флэшей
 *
 * storageType flashsarray
 * displayType FLASHSARRAY
 */
class PXDisplayTypeFlashsarray extends PXDisplayTypeImagesarray {
	static function buildArrayItem($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'flash', $value['entry']);
	}

	static function buildCellHtml($value, $id) {
		return PXDisplayTypeFlash::buildCellHtml($value, $id);
	}
}




/**
 * Картинка
 *
 * storageType image
 * displayType IMAGE|width|height
 *             width и height не используются
 */
class PXDisplayTypeImage extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: array('width'=>null, 'height'=>null, 'path'=>null);

		return NLAbstractHTMLForm::BuildImage($k, $value);
	}


	static function buildCell($field, $object, $param=NULL) {
		return PXDisplayTypeImage::buildCellHtml($object[$field->name]);
	}

	static function buildCellHtml($value) {
		$html  = '<div class="imagepreview image-in-cell"><div class="small">';

		if(!is_null($value['path'])) {
			$size = 50;

			$w = isset($value['width'])  ? $value['width']  : $size;
			$h = isset($value['height']) ? $value['height'] : $size;

			if($w > $size || $h > $size) {
				$width  = ($w - $h) > 0 ? $size : floor($size * $w/$h);
				$height = ($w - $h) > 0 ? floor($size * $h/$w) : $size;
			} else {
				$width  = $w;
				$height = $h;
			}

			$html .= '<a href="'.$value['path'].'" target="_blank"><img src="'.$value['path'].'" width="'.$width.'" height="'.$height.'"/></a>';
		}

		$html .= '</div>'.($value['path'] ? $value['width'].'x'.$value['height'] : '').'</div>';
		return $html ;
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetImageVar($field->name);
	}
}
/**
 * Флешка
 *
 * storageType flash
 * displayType FLASH|width|height
 *             width и height не используются
 */
class PXDisplayTypeFlash extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: array('width'=>null, 'height'=>null, 'path'=>null);

		return NLAbstractHTMLForm::BuildFlash($k, $value);
	}


	static function buildCell($field, $object, $param=NULL) {
		return PXDisplayTypeFlash::buildCellHtml($object[$field->name], $field->name.$object['id']);
	}

	static function buildCellHtml($value, $id) {
		if(!isset($value['path']) ) {
			return '';
		}

		$html  = '<div class="imagepreview image-in-cell"><div class="small">';
		$html .= '<script type="text/javascript">ShowFlash(\'i/l.swf?path='.$value['path'].'\', 50, 50, \''.$id.'src\');</script>';
		$html .= '</div>'.$value['width'].'x'.$value['height'].'</div>';

		return $html;
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}
}
/**
 * Файл
 *
 * storageType file
 * displayType FILE
 */
class PXDisplayTypeFile extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'fullpath'=>null, 'size'=>null, 'type'=>null);

		return NLAbstractHTMLForm::BuildFile($k, $value);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}

	static function buildCell($field, $object, $param=NULL) {
		return PXDisplayTypeFile::buildCellHtml($object[$field->name]);
	}

	static function buildCellHtml($fileArray) {
		if(!isset($fileArray['fullpath'])) {
			return '';
		}

		return '<a href="'.$fileArray['fullpath'].'" class="file '.preg_replace('/^.+\./', '', $fileArray['filename']).'">'.$fileArray['filename'].'</a> '.number_format($fileArray['size']/1024, 0, ',', ' ').'Кб</br>';
	}
}
/**
 * Ссылка на файл, лежащий где то еще (например в /download/)
 * для выбора файла используется модуль PXModuleFile
 *
 * storageType serialized
 * displayType LINKTOFILE
 */
class PXDisplayTypeLinktofile extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;

		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'dir'=>null, 'size'=>null);

		return NLAbstractHTMLForm::BuildLinkToFile($k, $value);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		if(!isset($value['filename'])) {
			return '';
		}

		$tmp = array(
			'filename' => $value['filename'],
			'fullpath' => $value['dir'].'/'.$value['filename'],
			'size'     => $value['size']
		);

		return PXDisplayTypeFile::buildCellHtml($tmp);
	}
}
?>
