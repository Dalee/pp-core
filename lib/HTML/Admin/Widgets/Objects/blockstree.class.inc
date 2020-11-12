<?php

class PXAdminBlocksTree extends PXAdminTreeObjects {

	protected $parentFormat;
	protected $parentField;

	/**
	 *
	 * @param mixed $format
	 * @param string $fieldName
	 * @param int $objectId
	 * @param bool $withoutExternals
	 */
	public /*override*/ function __construct($format, $fieldName, $objectId, $withoutExternals = false) {
		$app = PXRegistry::getApp();
		$this->parentFormat = $format;
		$this->parentField = $fieldName;
		$this->setControlParent($objectId);

		parent::__construct(
			$app->types['block'],
			[
				'parent_type' => $format->id,
				'parent_field' => $fieldName,
				'parent_id' => $objectId
			],
			null,
			$withoutExternals
		);
	}

	public /*override*/ function loadObjects() {
		$cacheDataKey = self::makeDataKey($this->loadWhere);
		if ($this->tree  = self::retrieveData($cacheDataKey)) {
			$this->count = sizeof($this->tree->leafs) - 1;
			return;
		}

		$this->tree = $this->db->getObjectsByFields($this->format, null, $this->loadWhere, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0) );

		$this->db->filterByAccess('admin', $this->format, $this->tree);
		$this->count = sizeof($this->tree->leafs) - 1;

		self::storeData($cacheDataKey, $this->tree);
	}

	protected /*override*/ function addControls() {
		$parent = $this->getControlParent();

		if(!PXRegistry::getUser()->can(['write', 'add'], $this->app->types['block'], ['parent' => $parent, 'id' => null])) {
			return;
		}

		$button = new PXControlButton($this->parentFormat->fields[$this->parentField]->description);
		$button->setClickCode(sprintf('AddBlockContent(\'%s\', \'%s\', %d)', $this->parentFormat->id, $this->parentField, $parent));
		$button->setClass('add');

		$button->addToParent($this->layoutButtonCell);
	}

	protected function htmlCaption() {
		$caption = '';
		if($this->isShowCaption) {
			$caption = $this->parentFormat->fields[$this->parentField]->description;

			if($this->count > 0) {
				$caption .= ' (' . $this->count . ')';
			}
		}
		return '<h2>' . $caption. '</h2>';
	}

	public /*override*/ function html() {
		return $this->htmlCaption() . $this->widget->html();
	}
}

