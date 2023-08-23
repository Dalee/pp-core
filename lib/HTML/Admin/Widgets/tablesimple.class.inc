<?php

class PXAdminTableSimple extends PXAdminWidget
{

	public const DEFAULT_MODE = 0;
	public const HIDEHEADER_MODE = 1;
	public const HIDEFOOTER_MODE = 2;
	public const BODY_MODE = 3;

	protected $widths;

	protected $dict;

	protected $data;

	protected $footer;

	protected $controls;

	/**
	 * @var string
	 */
	protected $nullText;

	/**
	 * @var string
	 */
	protected $caption;

	/**
	 * @var string
	 */
	protected $tableId;

	/**
	 * @var bool
	 */
	protected $showEven;


	/**
	 * PXAdminTableSimple constructor.
	 * @param array $cols
	 */
	public function __construct(protected $cols)
	{
		$this->widths = [];

		$this->dict = [];
		$this->data = [];
		$this->footer = [];
		$this->controls = [];

		$this->nullText = '';
		$this->caption = '';

		$this->showEven = false;
	}

	/**
	 * @param array $data
	 * @return $this
	 */
	public function setData($data)
	{
		if (!is_array($data)) {
			$data = [];
		}

		$this->data = $data;

		return $this;
	}

	/**
	 * @return $this
	 */
	public function setWidths()
	{
		$this->widths = func_get_args();

		return $this;
	}

	/**
	 * @param string $text
	 * @return $this
	 */
	public function setCaption($text)
	{
		$this->caption = (string)$text;

		return $this;
	}

	/**
	 * @param $cells
	 * @return $this
	 */
	public function setFooter($cells)
	{
		$this->footer = $cells;

		return $this;
	}

	/**
	 * @param string|null $id
	 * @return $this
	 */
	public function setTableId($id = null)
	{
		$this->tableId = $id ?: md5(microtime(1));

		return $this;
	}

	/**
	 * @return string
	 */
	public function htmlCaption()
	{
		if (!mb_strlen($this->caption)) {
			return '';
		}

		return '<caption>' . $this->caption . '</caption>';
	}

	/**
	 * @return string
	 */
	public function htmlBody()
	{
		$_ = '';
		foreach ($this->data as $row) {
			$id = $row['id'] ?? 0;

			$attrs = $row['__row_meta']['attrs'] ?? [];
			$attrsString = '';
			foreach ($attrs as $k => $v) {
				$v = quot($v);
				$attrsString .= " {$k}=\"$v\"";
			}

			$_ .= '<tr' . ($this->tableId && $id ? ' id="' . $this->tableId . '-' . $id . '"' : '') . $attrsString . '>';

			if ($id) {
				$_ .= $this->cellCTRL($id);
			}

			foreach ($this->cols as $col => $title) {
				$_ .= $this->cell($row, $col);
			}

			$_ .= '</tr>';
		}

		return $_;
	}

	/**
	 * @param $col
	 * @param $dictArray
	 * @return $this
	 */
	public function setDict($col, $dictArray)
	{
		$this->dict[$col] = $dictArray;

		return $this;
	}

	/**
	 * @param $url
	 * @param $title
	 * @param $icon
	 * @param bool $confirm
	 * @param bool $popup
	 * @return $this
	 */
	public function setControls($url, $title, $icon, $confirm = false, $popup = false)
	{
		$this->controls[] = [
			'title' => $title,
			'url' => $url,
			'icon' => $icon,
			'confirm' => $confirm,
			'popup' => $popup,
		];

		return $this;
	}

	/**
	 * @param string $text
	 * @return $this
	 */
	public function setNullText($text)
	{
		$this->nullText = $text;

		return $this;
	}

	/**
	 * @return $this
	 */
	public function showEven()
	{
		$this->showEven = true;

		return $this;
	}

	public function cellCTRL($id)
	{
		if (!sizeof($this->controls)) {
			return '';
		}

		$_ = '<td class="c">';

		foreach ($this->controls as $c) {
			$url = $c['url'] . $id;

			$_ .= '<a href="' . $url . '"';

			if ($c['confirm']) {
				$_ .= ' onclick="return window.confirm(\'Вы действительно решили ' . $c['title'] . '?\');"';

			} elseif ($c['popup']) {
				$_ .= ' onclick="Popup(this.href, 760, 550); return false;"';
			}

			$_ .= '>';
			$_ .= '<img src="/admin/i/icon/' . $c['icon'] . '.gif" alt="' . $c['title'] . '"></a>';
		}

		$_ .= '</td>';

		return $_;
	}

	public function cell($row, $col)
	{
		$val = $row[$col] ?? null;

		$class = [];

		if (is_null($val)) {
			$class[] = 'null';
		}

		if (isset($this->dict[$col]) && !(is_callable($this->dict[$col]) || isset($this->dict[$col][$val]))) {
			$class[] = 'not-in-dict';
		}

		$attrs = [];

		if (isset($this->dict[$col]) && is_callable($this->dict[$col])) {
			$val = $this->dict[$col]($row, $val, $attrs, $class);
		} elseif (isset($this->dict[$col][$val])) {
			$val = $this->dict[$col][$val];
		}

		if (is_null($val)) {
			$val = $this->nullText;
		}

		$attrsString = '';
		foreach ($attrs as $k => $v) {
			$v = quot($v);
			$attrsString .= " {$k}=\"$v\"";
		}
		$_ = '<td' . (sizeof($class) ? ' class="' . implode(' ', $class) . '"' : '') . $attrsString . '>';

		$_ .= $val;

		$_ .= '</td>';

		return $_;
	}

	public function html($mode = self::DEFAULT_MODE)
	{

		$_ = '<table class="objects">';

		$_ .= $this->htmlCaption();

		switch ($mode) {
			default:
			case self::DEFAULT_MODE:
				$_ .= $this->htmlCaption();
				$_ .= $this->htmlHeader();
				$_ .= $this->htmlBody();
				$_ .= $this->htmlFooter();
				break;
			case self::HIDEHEADER_MODE:
				$_ .= $this->htmlBody();
				$_ .= $this->htmlFooter();
				break;
			case self::HIDEFOOTER_MODE:
				$_ .= $this->htmlCaption();
				$_ .= $this->htmlHeader();
				$_ .= $this->htmlBody();
				break;
			case self::BODY_MODE:
				$_ .= $this->htmlBody();
				break;
		}

		$_ .= '</table>';

		return $_;
	}

	public function htmlHeader()
	{
		$_ = '';

		if (sizeof($this->widths)) {
			$_ .= '<colgroup>';

			foreach ($this->widths as $width) {
				$_ .= '<col' . ($width > 0 ? ' width="' . $width . '"' : '') . '>';
			}

			$_ .= '</colgroup>';
		}

		$_ .= '<thead><tr>';

		if (sizeof($this->controls)) {
			$_ .= '<th class="c c' . sizeof(
					$this->controls
				) . '"><img src="/admin/i/icon/order.gif" /></th>';   // !!! FIXME !!!
		}

		foreach ($this->cols as $colTitle) {
			$_ .= '<th>' . $colTitle . '</th>' . "\n";
		}

		$_ .= '</tr></thead>';

		return $_;
	}

	public function htmlFooter()
	{
		$_ = '';

		if (sizeof($this->footer)) {
			$_ .= '<tfoot><tr>';

			foreach ($this->cols as $col => $title) {
				$_ .= $this->cell($this->footer, $col);
			}

			$_ .= '</tr></tfoot>';
		}

		return $_;
	}
}
