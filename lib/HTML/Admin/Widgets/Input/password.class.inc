<?php

class PXInputPassword extends PXAdminWidget
{
    public $canDelete = true;

    public function __construct(public $name, public $value)
    {
    }

    public function canDelete($canDelete)
    {
        $this->canDelete = $canDelete;
    }

    public function html()
    {
        $name = $this->name;

        if (mb_strlen((string) $this->value) && $this->canDelete) {
            $deleteHTML = <<<HTML
				<input type="checkbox" name="{$name}[delete]" id="{$name}[delete]" class="checkbox" {$this->disable}>
				<label for="{$name}[delete]">удалить пароль</label>
HTML;
        } else {
            $deleteHTML = '';
        }

        return <<<HTML
			<div class="password">
				<input type="password" name="{$name}[type]"   {$this->disable} onfocus="startCheckPassword(this, '{$name}')">
				<input type="password" name="{$name}[retype]" {$this->disable} onfocus="startCheckPassword(this, '{$name}')">

				{$deleteHTML}

				<span>Длина пароля должна быть не менее 6 символов. Хороший пароль должен включать в себя строчные и заглавные буквы латинского алфавита, а также цифры.</span>
			</div>
HTML;
    }
}

