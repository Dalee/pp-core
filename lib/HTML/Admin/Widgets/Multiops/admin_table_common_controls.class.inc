<?php

class PXAdminTableCommonControls extends PXAdminWidget {

	function html() {
		return <<<HTML
				<div class="multiple-bottom-controls">
					<a href="#doCommonMultipleDelete" confirm="Вы уверены что хотите удалить отмеченные объекты?">Удалить</a>
					<a href="#doCommonMultipleStatusChange" helper="true">Изменить статус</a>
					<div class="helper">
						<div class="status-changer">
							<button class="on"  assign='{"status": "true"}'>Опубликовать</button>
							<button class="off" assign='{"status": "false"}'>Скрыть</button>
						</div>
					</div>
				</div>
HTML;
	}
}
