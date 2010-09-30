<?php
define("MASK_MIDDLE_PAGE",   0);

class PXAdminPager extends PXAdminWidget {
	var $_currentPage;
	var $_rowsPerPage;
	var $_rowsCount;
	var $_pagerName;
	var $_getData;

	/**
	 * Конструктор класса PXAdminPager.
	 * Инициализирует поля класса. необходимые для построения постраничной навигации.
	 * @param Integer $cur - текущая страница, нумерация начинается с 1 !
	 * @param Integer $rows - по сколько записей выводить на страницу
	 * @param Integer $count - общее количество записей
	 * @param String  $pager_name - имя переменной для постраничной навигации, конкатенируется с '_page'
	 * @param Array   $getData - массив с GET переменными, которые надо добавлять в ссылки постраничной навигации, обычно берётся из поля $layout->getData
	 */
	function PxAdminPager($cur, $rows = 1, $count = 1, $pager_name = '', $get_data = null){
		if($pager_name instanceof PXTypeDescription) {
			$pager_name = $pager_name->id;
		}

		$this->_currentPage = (int)$cur > 0 ? $cur : '1';
		$this->_rowsPerPage = $rows;
		$this->_rowsCount   = $count;
		$this->_pagerName   = $pager_name;
		$this->_getData     = $get_data;
	}

	function html(){
		return $this->_buildPager();
	}

	function _buildPager() {

		$html = '';

		if ($this->_rowsCount > $this->_rowsPerPage && $this->_rowsPerPage > 0 && $this->_currentPage > 0) {

			$html .= '<div class="perpage">';

			$allPages = ceil($this->_rowsCount/$this->_rowsPerPage)+1;
			$start    = (ceil($this->_currentPage/10)-1)*10+1;
			$max      = $start + 10;

			if ($max > $allPages) {
				$max = $allPages;
			}

			if ($this->_currentPage > 10) {
				$prev = (ceil($start/10)-1)*10; //-9
			}

			$last = $allPages - $start - 10;

			if ($last > 0) {
				$next = (ceil($start/10)-1)*10 + 11;
			}

			if (isset($prev)) {
				$html .= '<a href="'.$this->_BuildHref($this->_pagerName.'_page', $prev).'">';
				$html .= '<img src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
				$html .= '</a>';
			}

			for ($i=$start; $i<$max; $i++) {
				$html .= '<a';

				if ($i == $this->_currentPage) {
					$html .= ' class="selected"';
				}

				$html .= ' href="'.$this->_BuildHref($this->_pagerName.'_page', $i).'" title="Страница '.$i.'">'.$i.'</a>';
			}

			if (isset($next) && $next > 0) {
				$html .= '<a href="'.$this->_BuildHref($this->_pagerName.'_page', $next).'">';
				$html .= '<img src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
				$html .= '</a>';
			}

			$html .= '</div>';
		}
		return $html;
	}

	function _buildHref($key, $val = NULL) {
		$href = "?";
		if (!is_null($key) && !is_null($val)) {
			foreach ($this->_getData as $gdK => $gdV)
				if ($key != $gdK && isset($gdV))
					$href .= $gdK.'='.urlencode($gdV).'&';

			$href .= $key.'='.urlencode($val).'&';
		} else {
			$href = " ";
		}
		$href = substr($href, 0, -1);
		return $href;
	}

	function getPosition () {
		$pagePosition = MASK_MIDDLE_PAGE;

		if ($this->_currentPage == 1) {
			$pagePosition = $pagePosition | MASK_FIRST_PAGE;
		}
		if ($this->_currentPage >= ceil($this->_rowsCount/$this->_rowsPerPage)) {
			$pagePosition = $pagePosition | MASK_LAST_PAGE;
		}

		return $pagePosition;
	}
}
?>