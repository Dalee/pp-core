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
		$layout  = $this->layout;
		$app     = $this->app;
		$db      = $this->db;
		$request = $this->request;
		$user    = $this->user;

		$this->onPage = $this->app->getProperty('CHILDREN_ON_PAGE', 5);
		$sid = $this->request->getVar('sid');

		$this->controls = array();

		$campaigns = $this->adminCampaigns($sid);

		if (is_numeric($sid) && isset($campaigns->leafs[$sid])) {
			$this->adminBanners($sid);
		}

		$this->adminPlaces();

		foreach($this->controls as $dt=>$parents) {
			$this->layout->appendControls('INNER.'.$parents[2].'.1', $parents[0], array($dt));
//			$this->layout->appendContextMenu('INNER.0.0.CONTEXT', $parents[1], array($dt));
		}
	}

	function adminCampaigns($sid) {
		$dtCampaign =& $this->app->types[$this->dtCampaign];

		$tree = $this->db->getObjects($dtCampaign, NULL, DB_SELECT_TREE);
		$this->db->filterByAccess('admin', $dtCampaign, $tree);

		$dtCampaign->makeTreeWithSid = true;

		$this->layout->append('INNER.0.0', '<h2>'.$dtCampaign->title.' ('.sizeof($tree->leafs).')</h2>');
		$this->layout->appendTree('INNER.0.0', $dtCampaign, $tree, $sid, $this->request->getLeafStatus());

		if ($this->user->can('write', $dtCampaign, array())) {
			$this->controls[$this->dtCampaign] = array($sid, 0, 0);
		}

		return $tree;
	}

	function adminBanners($sid) {
		$dtBanner =&  $this->app->types[$this->dtBanner];

		$this->layout->setGetVarToSave($dtBanner->id.'_page', $this->request->getVar($dtBanner->id.'_page', 1));
		$currentPage = $this->request->getVar($dtBanner->id.'_page', 1);

		$currentOrder = $this->request->getOrderVar($dtBanner->id, $dtBanner->order, $dtBanner->fields);
		$this->layout->setGetVarToSave($dtBanner->id.'_order', $currentOrder);

		$objects = $this->db->getObjectsByParentLimited($dtBanner, NULL, $sid, $this->onPage, $this->onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);
		$count   = $this->db->getObjectsByParent($dtBanner, NULL, $sid, DB_SELECT_COUNT);

		$table = new PXAdminTable($objects, $dtBanner, $this->layout->getData);
		$stats = '<a href="javascript:AdminPopup(\''.$this->area.'\',\'{{format}}\',{{id}}, \'monthly\',990,600);"><img border="0" src="i/icon/stat.gif" align="middle" hspace="3" alt="{{title}}"></a>';
		$table->appendControl('stats', $stats);
		$table->setPager($currentPage, $this->onPage, $count);
		$table->loadDropdownValues($this->db);

		$table->addToParent('INNER.1.0');

		if ($this->user->can('write', $dtBanner, array('id' => 0, 'parent' => $sid))) {
			$this->controls[$this->dtBanner] = array($sid, $sid, 1);
		}
	}

	function adminPlaces() {
		$dtPlace = $this->app->types[$this->dtPlace];

		if($this->user->can('admin', $dtPlace)) {
			$currentOrder = $this->request->GetOrderVar($dtPlace->id, $dtPlace->order, $dtPlace->fields);
			$this->layout->setGetVarToSave($dtPlace->id.'_order', $currentOrder);
			$this->layout->setGetVarToSave($dtPlace->id.'_page', $this->request->getVar($dtPlace->id.'_page', 1));
			$currentPage = $this->request->getVar($dtPlace->id.'_page', 1);

			$objects = $this->db->getObjectsLimited($dtPlace, NULL, $this->onPage, $this->onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);
			$this->db->filterByAccess('admin', $dtPlace, $objects);

			$count   = $this->db->getObjects($dtPlace, NULL, DB_SELECT_COUNT);
			$this->layout->appendTable('INNER.0.0', $dtPlace->id, $objects, null, null, $currentPage, $this->onPage, $count);

			if ($this->user->can('write', $dtPlace, array())) {
				$this->controls[$this->dtPlace] = array(0, 0, 0);
			}
		}
	}

	function adminPopup() {
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
				$response =& PXResponse::getInstance();
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

					$object = $db->GetObjectByID($app->types[$format], $id);

					if (!empty($object)) {
						$bannerTitle  = ($mode == 'ctr' ? 'CTR' : ($mode == 'clicks' ? 'Клики' : 'Показы')) .  ' для ';
						$bannerTitle .= '"'. substr(strip_tags($object['title']), 0, 32) . '" ('.$months[$month].' '.$year.')';
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

					DEFINE("TTF_DIR", BASEPATH . "/local/share/fonts/");
					require_once 'Jpgraph/jpgraph.php';
					require_once 'Jpgraph/jpgraph_bar.php';

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

					$graph->Stroke();
					

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
				$response->downloadFile('stat_'.$id.'_'.$year.'_'.$month.'.csv', 'image/png');
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
				$title = isset($object['title']) ? substr(strip_tags($object['title']), 0, 32):'баннер не найден';


				$layout->SetOuterForm('action.phtml', 'POST', 'multipart/form-data', true);
				$layout->Assign('OUTER.RIGHTCONTROLS', PXAdminObjectForm::BuildClose('Закрыть'));
				$years = $db->Query('SELECT EXTRACT(YEAR FROM ts) as year FROM adstat GROUP BY year ORDER BY year ASC;');
				//DD-MM-YYYY
				$html = '<DIV style="padding: 10px 0; width: 100%;">
						 <select name="stat[year]">';
				foreach($years as $y) {
					$html .='<option value="'.$y['year'].'" ' . ($y['year'] == $year ? 'SELECTED' : '') . '>'.$y['year'].'</option>';
				}
				$html.= '</select>
						 <select name="stat[month]">';
				foreach($months as $k=>$m) {
					$html .='<option value="'.$k.'" ' . ($k == $month ? 'SELECTED' : '') . '>'.$m.'</option>';
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
		//$action = $request->getVar("action");
		$stat   = $request->getVar('stat');

		$mode   = $request->getVar('mode');
		$format = $request->getFormat();

		$id     = (int)$request->getId();
		$year   = (int)$stat['year'];
		$month  = (int)$stat['month'];

		return 'popup.phtml?area=advert&format=adbanner&mode='.$mode.'&id='.$id.'&year='.$year.'&month='.$month;
	}

	function userIndex() {
		$error = 0;
		$refName = $this->dtBanner.'2'.$this->dtPlace;
		if ($rqPlaceId = $this->request->GetVar('p')) {
			$adPlace = $this->db->GetObjectById($this->app->types[$this->dtPlace], $rqPlaceId);
			$attached = $this->db->GetLinks($this->app->references[$refName], $this->dtPlace, $rqPlaceId);
			if (count($attached)) {
				$banners = $this->db->GetObjectsByIdArray($this->app->types[$this->dtBanner], "TRUE", array_keys($attached));
			} else {
				$banners = $this->db->_GetData($this->app->types[$this->dtBanner], DB_SELECT_TABLE, "{$this->dtBanner}.type='{$adPlace['type']}' AND {$this->dtBanner}.dynamic = TRUE AND {$this->dtBanner}.status = TRUE");
			}
			if (count($banners)) {
				$distrib = array();
				foreach ($banners as $banner) {
					for ($i=0; $i<$banner['weight']; $i++) {
						$distrib[] = $banner['id'];
					}
				}
				$toShow = $banners[$distrib[array_rand($distrib)]];
				if ($this->request->GetVar('d') == 'gif') {
					setcookie('place'.$request->GetVar('p'), $toShow['id']);

					$response =& PXResponse::getInstance();
					$response->redirect($toShow['image']['path']);

				} else {
					$this->ShowAd($this->request->GetVar('p'), $toShow);
				}
				$toShow['shows'] = (is_numeric($toShow['shows'])) ? $toShow['shows']+1 : 1;
				$this->db->user->level = 16384;
				$this->db->ModifyContentObject($this->app->types[$this->dtBanner], $toShow);
			} else {
				$error = 2; //  ERROR: no banners to show
			}
		} else {
			$error = 1; //  ERROR: no pid
		}
		if ($error && DEBUG) {
			FatalError("Ошибка <STRONG>#{$error}</STRONG> модуля advert");
		}
		exit();
	}

	function _ParseBannerType($string) {
		return explode("x", $string);
	}

	function ShowAd($placeId, $toShow) {
		list($width, $height) = $this->_ParseBannerType($toShow['type']);
		$link = '/action.phtml?area='.$this->area.'&p='.$placeId.'&b='.$toShow['id'];
		$escLink = '/action.phtml?area='.$this->area.'%26p='.$placeId.'%26b='.$toShow['id'];
		if (!empty($toShow['image']['path'])) {
			if (!empty($toShow['flash']['path'])) {
				$html = '
				<SCRIPT language="JavaScript">
				<!--
				var plugin = (navigator.mimeTypes && navigator.mimeTypes["application/x-shockwave-flash"]) ? navigator.mimeTypes["application/x-shockwave-flash"].enabledPlugin : 0;
				if (plugin) {
					plugin = parseInt(plugin.description.substring(plugin.description.indexOf(".")-1)) >= 5;
				}
				else if (navigator.userAgent && navigator.userAgent.indexOf("MSIE")>=0
						&& (navigator.userAgent.indexOf("Windows 95")>=0 || navigator.userAgent.indexOf("Windows 98")>=0 || navigator.userAgent.indexOf("Windows NT")>=0)) {
					document.write(\'<SCRIPT LANGUAGE=VBScript\> \n\');
					document.write(\'on error resume next \n\');
					document.write(\'plugin = ( IsObject(CreateObject("ShockwaveFlash.ShockwaveFlash.5")))\n\');
					document.write(\'</SCRIPT\> \n\');
				}
				if (plugin) {
					document.write(\'<OBJECT classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"\');
					document.write(\'  codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=4,0,2,0" \');
					document.write(\' id="inside" width="'.$width.'" height="'.$height.'">\');
					document.write(\' <PARAM name="movie" value="'.$toShow['flash']['path'].'?link1='.$escLink.'"> <PARAM name="menu" value="false"> <PARAM name="quality" value="high"> <PARAM name="bgcolor" value="white"><PARAM name="wmode" value="opaque"> \');
					document.write(\' <EMBED src="'.$toShow['flash']['path'].'?link1='.$escLink.'" menu="false" quality="high" bgcolor="white" wmode="opaque"\');
					document.write(\' swLiveConnect="false" width="'.$width.'" height="'.$height.'"\');
					document.write(\' TYPE="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash">\');
					document.write(\' </EMBED>\');
					document.write(\' </OBJECT>\');
				} else if (!(navigator.appName && navigator.appName.indexOf("Netscape")>=0 && navigator.appVersion.indexOf("2.")>=0)){
					document.write(\'<A href="'.$link.'" target="_blank"><IMG src="'.$toShow['image']['path'].'" width="'.$width.'" height="'.$height.'" border="0" alt=""></A>\');
				}
				//-->
				</SCRIPT><NOSCRIPT><A href="'.$link.'" target="_blank"><IMG src="'.$toShow['image']['path'].'" width="'.$width.'" height="'.$height.'" border="0" alt=""></A></NOSCRIPT>
				';
			} else {
				$html  = '<A href="'.$link.'" target="_blank"><IMG src="'.$toShow['image']['path'].'" width="'.$width.'" height="'.$height.'" border="0" alt=""></A>';
			}
		} else {
			if (!empty($toShow['flash']['path'])) {
				$html = '
				<SCRIPT language="JavaScript">
				<!--
				var plugin = (navigator.mimeTypes && navigator.mimeTypes["application/x-shockwave-flash"]) ? navigator.mimeTypes["application/x-shockwave-flash"].enabledPlugin : 0;
				if (plugin) {
					plugin = parseInt(plugin.description.substring(plugin.description.indexOf(".")-1)) >= 5;
				}
				else if (navigator.userAgent && navigator.userAgent.indexOf("MSIE")>=0
						&& (navigator.userAgent.indexOf("Windows 95")>=0 || navigator.userAgent.indexOf("Windows 98")>=0 || navigator.userAgent.indexOf("Windows NT")>=0)) {
					document.write(\'<SCRIPT LANGUAGE=VBScript\> \n\');
					document.write(\'on error resume next \n\');
					document.write(\'plugin = ( IsObject(CreateObject("ShockwaveFlash.ShockwaveFlash.5")))\n\');
					document.write(\'</SCRIPT\> \n\');
				}
				if (plugin) {
					document.write(\'<OBJECT classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"\');
					document.write(\'  codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=4,0,2,0" \');
					document.write(\' id="inside" width="'.$width.'" height="'.$height.'">\');
					document.write(\' <PARAM name="movie" value="'.$toShow['flash']['path'].'?link1='.$escLink.'"> <PARAM NAME=menu VALUE=false> <PARAM NAME=quality VALUE=high> <PARAM NAME=bgcolor VALUE=white> <PARAM name="wmode" value="opaque">\');
					document.write(\' <EMBED src="'.$toShow['flash']['path'].'?link1='.$escLink.'" menu="false" quality="high" bgcolor="white" wmode="opaque"\');
					document.write(\' swLiveConnect=FALSE width="'.$width.'" height="'.$height.'"\');
					document.write(\' TYPE="application/x-shockwave-flash" PLUGINSPAGE="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash">\');
					document.write(\' </EMBED>\');
					document.write(\' </OBJECT>\');
				}
				//-->
				</SCRIPT>
				';
			} else {
				$html = '';
			}
		}
		echo '<HTML><HEAD><TITLE></TITLE></HEAD><BODY style="margin:0;padding:0;">';
		echo $html;
		echo '</BODY></HTML>';
	}


	function userAction() {
		if ($request->GetVar('b')) {
			$bannerId = $request->GetVar('b');
		} else {
			if ($request->GetVar('p')) {
				$bannerId = $request->GetCookieVar('place'.$request->GetVar('p'));
			}
		}
		if (isset($bannerId) && !empty($bannerId)) {
			$adBanner = $db->GetObjectById($app->types[$this->dtBanner], $bannerId);
			$retValue = $adBanner['reference'];
			$adBanner['clicks'] = (is_numeric($adBanner['clicks'])) ? $adBanner['clicks']+1 : 1;
			$db->user->level = 16384;
			$db->ModifyContentObject($app->types[$this->dtBanner], $adBanner);
		} else {
			$retValue = '/';
		}
		return $retValue;
	}

}

?>
