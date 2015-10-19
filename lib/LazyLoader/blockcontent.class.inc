<?php

class PXBlockContentLazyLoader  {

	protected $app;
	protected $db;

	protected $ownerType;
	protected $ownerId;
	protected $ownerField;

	protected $tree;
	protected $blocks;

	protected $mediaFields;

	protected static $TYPE = 'block';

	protected static $MEDIA_FIELDS = array('images');

	public function __construct($app, $db, $ownerType, $ownerId, $ownerField) {
		$this->app = $app;
		$this->db = $db;

		$this->ownerType = $ownerType;
		$this->ownerId = $ownerId;
		$this->ownerField = $ownerField;

		$this->tree = null;
		$this->blocks = null;

		$this->valid = $this->__isValid();
	}

	protected function __isValid() {
		$format = &$this->app->types[$this->ownerType];
		if(!isset($format)) {
			return false;
		}

		$field = &$format->fields[$this->ownerField];
		if(!isset($field)) {
			return false;
		}

		return is_a($field->storageType, 'PXStorageTypeBlockcontent');
	}

	protected function __init() {
		if(!$this->valid || $this->tree !== null) {
			return;
		}
		$this->__initOnce();
	}

	protected function __initOnce(){
		$this->tree = $this->db->getObjectsByFields(
				$this->app->types[self::$TYPE],
				true,
				array(
						'parent_type' => $this->ownerType,
						'parent_id' => $this->ownerId,
						'parent_field' => $this->ownerField
				),
				DB_SELECT_TREE
		);

		$this->blocks = $this->tree->map(function($leaf, $content){
			return $this->__blockMap($leaf, $content);
		});
	}

	protected function __blockMap ($leaf, $content) {
		$content = (array)(empty($content) ? array() : $content);
		if (!empty($leaf->content['data']['content'])) {
			array_unshift($content, $leaf->content['data']['content']);
		}

		$media = array_filter(array_intersect_key($leaf->content, self::$MEDIA_FIELDS));

		if (!empty($media)) {
			$leaf->content['data']['__media'] = $media;
		}

		return array(
				'block' => $leaf->content['type'],
				'mods' => $leaf->content['mods'],
				'ibem' => array(
						'id' => $leaf->content['id'],
						'title' => $leaf->content['title']
				),
				'data' => $leaf->content['data'],
				'content' => $content
		);
	}

	public function __toString() {
		return '';
	}

	public function tree() {
		$this->__init();
		return $this->tree;
	}

	public function blocks() {
		$this->__init();
		return $this->blocks;
	}

	public function __toBemJSON() {
		return $this->blocks();
	}
}
