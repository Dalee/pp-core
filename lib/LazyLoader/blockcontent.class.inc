<?php

class PXBlockContentLazyLoader implements \Stringable  {

	protected $tree;
	protected $blocks;

	protected $mediaFields;

	protected static $TYPE = 'block';

	protected static $MEDIA_FIELDS = ['images'];

	public function __construct(protected $ownerType, protected $ownerId, protected $ownerField) {
		$this->tree = null;
		$this->blocks = null;

		$this->valid = $this->__isValid();
	}

	protected function __isValid() {
		$app = PXRegistry::getApp();
		$format = &$app->types[$this->ownerType];
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
		$db  = PXRegistry::getDb();
		$app = PXRegistry::getApp();
		$this->tree = $db->getObjectsByFields(
				$app->types[self::$TYPE],
				true,
				[
						'parent_type' => $this->ownerType,
						'parent_id' => $this->ownerId,
						'parent_field' => $this->ownerField
                ],
				DB_SELECT_TREE
		);

		$this->blocks = $this->tree->map(fn($leaf, $content) => $this->__blockMap($leaf, $content));
	}

	protected function __blockMap ($leaf, $content) {
		$content = (array)(empty($content) ? [] : $content);
		if (!empty($leaf->content['data']['content'])) {
			array_unshift($content, $leaf->content['data']['content']);
		}

		$media = array_filter(array_intersect_key($leaf->content, self::$MEDIA_FIELDS));

		if (!empty($media)) {
			$leaf->content['data']['__media'] = $media;
		}

		return [
				'block' => $leaf->content['type'],
				'mods' => $leaf->content['mods'],
				'ibem' => [
						'id' => $leaf->content['id'],
						'title' => $leaf->content['title']
                ],
				'data' => $leaf->content['data'],
				'content' => $content
        ];
	}

	public function __toString(): string {
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
