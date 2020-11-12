<?php
/**
 * Базовый класс описывающий сохранение, получение из запроса
 * (и что-то еще) полей объекта с определенным storageType
 */
class PXStorageType
{

	public const defaultSQLType = 'VARCHAR';

	public $field;
	public $name;
	public $triggers;

	public function __construct(&$field, $name)
	{
		$this->field = &$field;
		$this->name = $name;
	}

	/**
	 * Возвращает заголовок таблицы объектов
	 * возможно с ссылкой на изменение порядка
	 * сортировки объектов
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|NULL $param
	 *
	 * @return String
	 */
	public function objectsSortOrder($field, $object, &$param)
	{
		return $field->description;
	}

	public function objectsSortOrderString($field, $object, &$param)
	{
		$html = '<a ';

		switch ($param['order']) {
			case ($field->name . ' DESC') :
				$html .= ' class="down"';
				$order = $field->name;
				$title = '';
				break;

			case $field->name :
				$html .= ' class="up"';
				$order = $field->name . ' DESC';
				$title = ' в обратном порядке';
				break;

			default:
				$order = $field->name;
				$title = '';
				break;
		}

		$html .= ' href="' . $param['table']->_BuildHref($param['dtype'] . '_order', $order) . '"';
		$html .= ' title="Сортировать по полю &bdquo;' . $field->description . '&ldquo;' . $title . '">' . $field->description . '</a>';

		return $html;
	}

	/**
	 * Возвращает нормированное значение поля из запроса
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|null $param
	 *
	 * @return mixed
	 */
	public function getFromRequest($field, $object, &$param)
	{
		return $object[$field->name];
	}

	/**
	 * Возвращает нормированное значение
	 * поля перед сохранением объекта в базу
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|null $param
	 *
	 * @return array
	 */
	public function normalizeObjectAttribute($field, $object, &$param)
	{
		return [NULL, $param['dbFields'], $param['dbValues']];
	}

	public function normalizeObjectAttributeString($field, $object, &$param)
	{
		$k = $field->name;
		$v = $object[$k];

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}

	/**
	 * Возвращает условие поиска (SQL) некоего слова
	 * в поле
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|null $param
	 *
	 * @return string
	 */
	public function searchLikeQuery($field, $object, &$param)
	{
		return '';
	}

	/**
	 * Сохраняет файлы из $_FILES в site/htdocs/ai
	 * Подготавливает данные к сохранению в базу
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|null $param
	 *
	 * @return string
	 */
	public function proceedFile($field, &$object, &$param, $allowed = null)
	{
		return isset($object[$field->name]) ? $object[$field->name] : null;
	}

	/**
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array $param
	 * @return null
	 */
	public function cloneFile($field, $object, &$param)
	{
		return null;
	}

	/**
	 * Возвращает нормированное значение
	 * поля после выборки объекта из базы
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|null $param
	 *
	 * @return mixed|null
	 */
	public function normalize($field, $object, &$param)
	{
		return isset($object[$field->name]) ? $object[$field->name] : null;
	}

	/**
	 * Генерит pathname объекта
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array|NULL $param
	 *
	 * @return String
	 */
	public function createPathname($field, $object, &$param)
	{
		return '';
	}

	/**
	 * Вызывается в PXDatabase::_createSelector при составлении
	 * списка полей объекта извлекаемых из базы
	 * Возвращает флаг, извлекать ли поле из базы
	 *
	 * @return bool
	 */
	public function storedInDb()
	{
		return !$this->notInDb($this->field, null, null);
	}

	/**
	 * @deprecated Use !storedInDb()
	 */
	public function notInDb($field, $object, $param = null)
	{
		return false;
	}

	/**
	 * Вызывается при работе с полем sys_meta
	 * Возвращает необходима ли выгрузка/сохранение значения в sys_meta
	 *
	 * @return Boolean
	 */
	public function storedInMeta()
	{
		return $this->inMeta($this->field, null, null);
	}

	/**
	 * @deprecated Use storedInMeta()
	 */
	public function inMeta($field, $object, $param = null)
	{
		return !$this->storedInDb();
	}

	/**
	 * Определяет возможность фильтрации вывода текущего StorageType в PXAdminObjectForm::LinksForm
	 */
	public function filterable()
	{
		return false;
	}

	/**
	 * @see PXDatabase::_createSelector()
	 */
	public function createSelector($field, $object, $param = null)
	{
		return $field->typeDescription->id . '.' . PXRegistry::getDB()->mapFields($field->name);
	}

	protected static function parseConfigArguments($attrNode, $fieldNode)
	{
		$args = explode("|", $attrNode->value);
		array_shift($args);

		return $args;
	}

	public static function getClassByName($name)
	{
		$baseClass = 'PXStorageType';

		$inputClass = $baseClass . mb_ucfirst(mb_strtolower($name));
		if (!class_exists($inputClass)) {
			trigger_error("Class {$inputClass} not found, use {$baseClass} instead", E_USER_NOTICE);
			return $baseClass;
		}
		return $inputClass;
	}

	public static function parseConfig($attrNode, $fieldNode)
	{
		$name = preg_replace('/\|.+$/' . REGEX_MOD, '', $attrNode->value);

		$cls = self::getClassByName($name);
		$args = call_user_func([$cls, 'parseConfigArguments'], $attrNode, $fieldNode);

		return [$cls, $name, $args];
	}
}
