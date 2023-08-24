<?php

class PXInputRichEdit extends PXAdminComplexWidget
{
    public $container;
    public $toLayout;

    protected $editors = [];

    protected $favoriteEditor = null;

    public const AFTER_TEXTAREA = 0;

    public function __construct(public $name, public $value, public $height)
    {
        $this->container = $name . '_id';
        $this->toLayout = PXRegistry::getLayout();
        $this->blocks = [self::AFTER_TEXTAREA => []];
    }

    public function addEditor($name)
    {
        $this->editors[] = $name;
    }

    public function setFavoriteEditor($redactor)
    {
        if (!is_null($redactor)) {
            $this->favoriteEditor = $redactor;
        }
    }

    protected function getTypesDict()
    {
        $embedding = [
            "types" => $this->editors,
            "current" => null,
        ];

        $embedding['current'] = is_null(
            $this->favoriteEditor
        ) || !isset($embedding['types'][$this->favoriteEditor]) ? key($embedding['types']) : $this->favoriteEditor;

        return $embedding;
    }

    protected function initiate(PXAdminWidget $editor)
    {
        $editor->initiate();
    }

    public function html()
    {
        $this->decorate('RICHEDITORS');

        array_walk($this->blocks[self::AFTER_TEXTAREA], $this->initiate(...));
        $this->addEditor('Без редактора');

        $embedding = $this->getTypesDict();
        $types = new PXInputDropDown(null, $embedding['current'], $embedding['types']);
        $types = $types->html();

        $richEditor = isset($this->blocks[self::AFTER_TEXTAREA][$embedding['current']]) ? $this->blocks[self::AFTER_TEXTAREA][$embedding['current']]->html() : '';

        $escapedValue = "\n" . htmlspecialchars((string) $this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);

        return <<<HTML
			<div class="redactor">
				<label>Выберите редактор</label>
				{$types}
			</div>
			<textarea name="{$this->name}" id="{$this->container}" style="width: 100%; height: {$this->height}px;" {$this->disable}>{$escapedValue}</textarea>
			{$richEditor}
HTML;
    }
}
