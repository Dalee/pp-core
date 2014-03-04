<?php
class PXModuleAdvert extends PXModule {
	var $dtPlace;
	var $dtCampaign;
	var $dtBanner;
	var $engine;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);
		$this->dtPlace    = $settings['place'];
		$this->dtCampaign = $settings['campaign'];
		$this->dtBanner   = $settings['banner'];
		$this->engine     = $settings['engine'];
	}

	function adminIndex() {
		$this->onPage = $this->app->getProperty('CHILDREN_ON_PAGE', 5);
		$sid = $this->request->getVar('sid');

		$this->controls = array();

		$campaigns = $this->adminCampaigns($sid);

		if (is_numeric($sid) && $campaigns->has($sid)) {
			$this->adminBanners($sid, $campaigns);
		}

		$this->adminPlaces();

		foreach($this->controls as $dt=>$parents) {
//			$this->layout->appendContextMenu('INNER.0.0.CONTEXT', $parents[1], array($dt));
		}
	}

	function adminCampaigns($sid) {
		$dtCampaign =& $this->app->types[$this->dtCampaign];

		$tree = new PXAdminTreeObjects($dtCampaign);
		$tree->setControlParent($sid);
		$tree->showChildren('sid');
		$tree->addToParent('INNER.0.0');

		if ($this->user->can('write', $dtCampaign, array())) {
			$this->controls[$this->dtCampaign] = array($sid, 0, 0);
		}

		return $tree;
	}

	function adminBanners($sid, $campaigns = null) {
		$dtBanner =&  $this->app->types[$this->dtBanner];

		$table = new PXAdminTableObjects($dtBanner, $sid, 'ByParent');
		$table->setControlParent($sid);
		$stats = '<a href="javascript:AdminPopup(\''.$this->area.'\',\'{{format}}\',{{id}}, \'monthly\',990,600);"><img border="0" src="i/icon/stat.gif" align="middle" hspace="3" alt="{{title}}"></a>';
		// FIXME
		$table->appendControl('stats', $stats);
		$table->addToParent('INNER.1.0');

		if ($this->user->can('write', $dtBanner, array('id' => 0, 'parent' => $sid))) {
			$this->controls[$this->dtBanner] = array($sid, $sid, 1);
		}

		// Load additional children tables
		if (!is_a($campaigns, 'PXAdminTreeObjects')){
			return;
		}

		$campaign      = $campaigns->get($sid);
		$allowedChilds = $this->app->getAllowedChildsKeys($this->dtCampaign, $campaign);

		if(count($allowedChilds) > 1) {
			foreach(array_diff($allowedChilds, array($this->dtBanner)) as $extra_dt) {
				if (!isset($this->app->types[$extra_dt])) {
						$this->layout->append('INNER.1.0', '<h1 class="error">Формат '.$extra_dt.' не описан</h1>');
						continue;
					}
					$objects = new PXAdminTableObjects($extra_dt, $sid, 'ByParent');
					$objects->setControlParent($sid);
					$objects->addToParent('INNER.1.0');
			}
		}
	}

	function adminPlaces() {
		$dtPlace = $this->app->types[$this->dtPlace];

		if($this->user->can('admin', $dtPlace)) {
			$table = new PXAdminTableObjects($dtPlace);
			$table->addToParent('INNER.0.0');

			if ($this->user->can('write', $dtPlace, array())) {
				$this->controls[$this->dtPlace] = array(0, 0, 0);
			}
		}
	}

	function adminPopup() {
		$layout  = $this->layout;
		$app     = $this->app;
		$db      = $this->db;
		$request = $this->request;
		
		$months = array(
			0  => '',
			1  => 'Январь',
			2  => 'Февраль',
			3  => 'Март',
			4  => 'Апрель',
			5  => 'Май',
			6  => 'Июнь',
			7  => 'Июль',
			8  => 'Август',
			9  => 'Сентябрь',
			10 => 'Октябрь',
			11 => 'Ноябрь',
			12 => 'Декабрь'
		);

		$format = $request->GetVar('format');

		$mode   = ($request->GetVar('mode') != '') ? $request->GetVar('mode') : 'shows' ; //shows, clicks, ctr
		$mode   = ($mode === 'shows' || $mode === 'clicks' || $mode === 'ctr') ? $mode : 'shows'; //double check

		$year   = (int)$request->GetVar('year')  > 0 ? (int)$request->GetVar('year')  : date("Y"); //stat year
		$month  = ((int)$request->GetVar('month') > 0 && (int)$request->GetVar('month') <= 12 ) ? (int)$request->GetVar('month') : date("n"); //stat month w/o zero

		$id     = (int)$request->GetId(); //banner id

		if (!isset($app->types[$format])) {
			FatalError("Unknown format type. Check \"format\" variable.");
		}

		switch($request->getAction()) {
			//show image and exit
			case 'image':
				$response = PXResponse::getInstance();
				$response->downloadFile('stat_'.$id.'_'.$year.'_'.$month.'.png', 'image/png', 'inline');
				$response->send();

				if ($id !== 0) {
					$lastDayOfMonth = 27;
					while (checkdate($month, ++$lastDayOfMonth + 1, $year));
					//jpGraph не понимает массивы начинающиеся не с нулевого эл-та, не стоит его расстраивать
					$datay = array_fill(0, $lastDayOfMonth, 0);

					$sql = 'SELECT EXTRACT(DAY FROM ts) AS days, SUM(show) as shows, SUM(click) AS clicks
							FROM adstat WHERE adbanner=\''.$id.'\'
							AND EXTRACT(MONTH FROM ts) = '.$month.' AND EXTRACT(YEAR FROM ts) = '.$year.'
							GROUP BY days ORDER BY days';
					$res = $db->Query($sql);

					$object = $db->GetObjectByID($this->app->types[$format], $id);

					if (!empty($object)) {
						$bannerTitle  = ($mode == 'ctr' ? 'CTR' : ($mode == 'clicks' ? 'Клики' : 'Показы')) .  ' для ';
						$bannerTitle .= '"'. mb_substr(strip_tags($object['title']), 0, 32) . '" ('.$months[$month].' '.$year.')';
					} else {
						$bannerTitle = "Баннер не найден";
					}

					$sum       = 0;      //сумма показов или кликов
					$sumClicks = 0;      //сумма кликов для CTR
					$sumShows  = 0;      //сумма показов для CTR
					$xTitleOverall = ''; //нижний заголовок с кол-вом кликов/показов/CTR

					foreach ($res as $row) {
						if ($mode !== 'ctr') {
							//начинаем с нулевого элемента, смотри выше array_fill
							$datay[$row['days'] -1 ] = $row[$mode];
							$sum += $row[$mode];
						} else {
							$datay[$row['days'] -1 ] = number_format(100*($row['clicks']/$row['shows']),2);
							$sumClicks += $row['clicks'];
							$sumShows  += $row['shows'];
						}

					}//foreach
					if ($mode !== 'ctr')
						$xTitleOverall = 'Всего ' . ($mode == 'clicks' ? 'кликов: '  : 'показов: ' ) . number_format($sum,0,'.',' ');
					else
						$xTitleOverall = 'Средний CTR: '. number_format(100*$sumClicks/$sumShows,2);

					define("TTF_DIR", PPPATH . "share/fonts/");
					require_once PPPATH . 'vendor/Jpgraph/jpgraph.php';
					require_once PPPATH . 'vendor/Jpgraph/jpgraph_bar.php';

					$graph = new Graph(950,400,"auto");
					$graph->SetScale("textlin");
					$graph->img->SetMargin(50,20,20,60);
					$graph->setMarginColor("#C9CED6");

					$graph->setFrame(0);
					$bplot = new BarPlot($datay);
					$bplot->SetColor("#656565");
					$bplot->SetFillColor('#009900@0.30');
					$bplot->SetWidth(0.85);
					$bplot->value->Show();
					if ($mode == 'ctr')
						$bplot->value->SetFormat('%.2f');
					else
						$bplot->value->SetFormat('%d');
					$bplot->value->setColor("black");
					$bplot->value->SetFont(FF_ARIAL, FS_NORMAL, 7);
					$graph->Add($bplot);

					$graph->xaxis->SetColor("#656565", "black");
					$graph->xaxis->scale->ticks->SupressTickMarks(true);
					$graph->xaxis->title->SetFont(FF_VERDANA, FS_NORMAL, 8);
					$graph->xaxis->SetTitle($xTitleOverall);
					$graph->xaxis->title->SetMargin(10,0,0,0);

					$graph->yaxis->SetColor("#656565", "black");
					$graph->yaxis->scale->ticks->SupressMinorTickMarks(true);
					$graph->yaxis->scale->SetGrace(5);
					$graph->yaxis->SetTickSide(SIDE_LEFT);

					$graph->title->SetFont(FF_VERDANA, FS_NORMAL, 10);
					$graph->title->Set($bannerTitle);

					//FIXME: just stop using JpGraph for php 4 ...
					//too much strict notices, so I've disabled all by The Dog (c)
					@$graph->Stroke();


					exit; /*<====EXIT !!!*/
				}//if

				$im = ImageCreateTruecolor('900', '400');

				imagePng($im);
				imageDestroy($im);
				exit; /* <====EXIT !!! */
					  /* break;        */

			//отдаем csv файл
			case 'csv':
				$response =& PXResponse::getInstance();
				$response->downloadFile('stat_'.$id.'_'.$year.'_'.$month.'.csv', 'text/csv');
				$response->send();

				$sql = <<<SQL
					SELECT
						EXTRACT(DAY FROM ts) AS day,
						SUM(show) as shows,
						SUM(click) AS clicks

					FROM
						adstat

					WHERE
						adbanner = '{$id}' AND
						EXTRACT(MONTH FROM ts) = {$month} AND
						EXTRACT(YEAR  FROM ts) = {$year}

					GROUP BY
						EXTRACT(DAY FROM ts)

					ORDER BY day
SQL;

				$res = $db->Query($sql);

				echo '"date";"shows";"clicks";"ctr"'. "\r\n";
				foreach ($res as $row) {
					$ctr = $row['shows'] > 0 ? $row['clicks']/$row['shows'] : 0;
					echo '"'.$year.'-'.$month.'-'.$row['day'].'";"'.$row['shows']. '";"' .$row['clicks']. '";"' .number_format(100 * ($ctr), 4, ',', '') .'"'. "\r\n";
				}
				exit; /*<====EXIT !!!*/
				break;

			//show textual content
			default:
				$object = $db->GetObjectByID($app->types[$request->GetFormat()], $request->GetId());
				$title = isset($object['title']) ? mb_substr(strip_tags($object['title']), 0, 32):'баннер не найден';


				$layout->SetOuterForm('action.phtml', 'POST', 'multipart/form-data', true);
				$layout->Assign('OUTER.RIGHTCONTROLS', NLAbstractHTMLForm::BuildClose('Закрыть'));
				$years = $db->Query('SELECT EXTRACT(YEAR FROM ts) as year FROM adstat GROUP BY year ORDER BY year ASC;');
				//DD-MM-YYYY
				$html = '<DIV style="padding: 10px 0; width: 100%;">
						 <select name="stat[year]">';
				foreach($years as $y) {
					$html .= '<option value="' . $y['year'] . '" ' . ($y['year'] == $year ? 'SELECTED' : '') . '>' . $y['year'] . '</option>';
				}
				$html.= '</select>
						 <select name="stat[month]">';
				foreach($months as $k=>$m) {
					$html .= '<option value="' . $k . '" ' . ($k == $month ? 'SELECTED' : '') . '>' . $m . '</option>';
				}
				$html .= '</select>

						 <input type="hidden" name="id"    value="'.$id.'">
						 <input type="hidden" name="area"  value="advert">

						 <input type="radio" id="shows" name="mode" value="shows" '.($mode == "shows" ? 'checked':'').'>
						 <label for="shows">Показы</label>

						 <input type="radio" id="clicks" name="mode" value="clicks" '.($mode == "clicks" ? 'checked':'').'>
						 <label for="clicks">Клики</label>

						 <input type="radio" id="ctr" name="mode" value="ctr" '.($mode == "ctr" ? 'checked':'').'>
						 <label for="ctr">CTR</label>

						 <input type="submit" value="Показать"  style="width: 80px">
						 <input type="button" style="width: 80px"
								onClick="javascript:window.open(\'popup.phtml?area=advert&format=adbanner&id='.$id.'&year='.$year.'&month='.$month.'&action=csv\'); return false;"
								value="CSV">
						 </DIV>';

				$html.= '<img src="?area=advert&format=adbanner&action=image&mode='.$mode.'&id='.$id;
				if ($year != 0 && $month != 0)
					$html .= '&year='.$year.'&month='.$month.'';
				$html .= '">';

				$layout->assign('OUTER.TITLE', 'Статистика для баннера &laquo;'. $title .'&raquo;');
				$layout->assign('OUTER.CONTENT', $html);

				break;
		}

	}//function

	function adminAction() {
		$request = $this->request;

		//$action = $request->getVar("action");
		$stat   = $request->getVar('stat');

		$mode   = $request->getVar('mode');
		$format = $request->getFormat();

		$id     = (int)$request->getId();
		$year   = (int)$stat['year'];
		$month  = (int)$stat['month'];

		return 'popup.phtml?area=advert&format=adbanner&mode='.$mode.'&id='.$id.'&year='.$year.'&month='.$month;
	}
}
?>
