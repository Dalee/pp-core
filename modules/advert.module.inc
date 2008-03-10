<?

class PXModuleAdvert extends PXAbstractModule {
	var $dtPlace;
	var $dtCampaign;
	var $dtBanner;
	var $engine;

	function PXModuleAdvert($area, $settings) {
		$this->PXAbstractModule($area, $settings);
		$this->dtPlace    = $settings['place'];
		$this->dtCampaign = $settings['campaign'];
		$this->dtBanner   = $settings['banner'];
		$this->engine     = $settings['engine'];
	}

	function adminIndex($app, $request, $user, $db, $layout) {
		$layout->SetInnerLayout(
			array(
				array(array('25%', '85%', NULL, NULL), array('75%', '15%', NULL, 2)),
				array(array('25%', '100%', NULL, NULL)),
				array(array('25%', NULL, NULL, NULL), array('75%', NULL, NULL, NULL))
			)
		);
		$tree = $db->GetObjects($app->types[$this->dtCampaign], NULL, DB_SELECT_TREE);
		$layout->AssignTree('INNER.0.0', $app->types[$this->dtCampaign], $tree, $request->GetSid(), $request->GetLeafStatus());
		if ($user->level >= $app->types[$this->dtCampaign]->nullrole) {
			$layout->AssignControls('INNER.0.2', $request->GetSid(), array($this->dtCampaign));
			$layout->AssignContextMenu('INNER.0.0.CONTEXT', 0, array($this->dtCampaign));
		}

		$addMenu = array(
			'adplace' => 'Баннерные позиции'
		);

		if ($this->engine == 'external') {
			$addMenu['adinterchange'] = 'Обновить данные';
		}

		$layout->AssignKeyValueList('INNER.0.1', $addMenu, $request->GetSid());
		$onPage = $app->GetProperty('CHILDREN_ON_PAGE', 5);

		if (is_numeric($request->GetSid())) {
			$parentObject = $db->GetObjectByID($app->types[$this->dtCampaign], $request->GetSid());
			if ($parentObject) {
				$layout->SetGetVarToSave($this->dtBanner.'_page', $request->GetVar($this->dtBanner.'_page', 1));
				$currentPage = $request->GetVar($this->dtBanner.'_page', 1);
				$objects = $db->GetObjectsByParentLimited($app->types[$this->dtBanner], NULL, $request->GetSid(), $onPage, $onPage*($currentPage-1));
				$count   = $db->GetObjectsByParent($app->types[$this->dtBanner], NULL, $request->GetSid(), DB_SELECT_COUNT);

				$table = new PXAdminTable($objects, $app->types[$this->dtBanner], $layout->getData);
				$stats = '<a href="javascript:AdminPopup(\''.$this->area.'\',\'{{format}}\',{{id}}, \'monthly\',990,600);"><img border="0" src="i/icon/stat.gif" align="middle" hspace="3" alt="{{title}}"></a>';
				$table->appendControl('stats', $stats);
				$table->setPager($currentPage, $onPage, $count);
				$layout->append('INNER.1.0', $table->getTable());

				$allowedChilds = $app->GetAllowedChildsKeys($this->dtCampaign, $parentObject);
	 			if ($user->can('write', $app->types[$this->dtBanner], array('id' => 0, 'parent' => $parentObject['id']))) {
					$layout->AssignControls('INNER.1.2', $request->GetSid(), $allowedChilds);
					$layout->AssignContextMenu('INNER.1.0.CONTEXT', $request->GetSid(), $allowedChilds);
				}

			} else {
				$layout->AssignError('INNER.1.0', "Некорректный параметр <EM>sid</EM>");
			}

		} elseif ($request->GetSid()) {
			switch ($request->GetSid()) {
				case 'adplace':
					$layout->SetGetVarToSave($this->dtPlace.'_page', $request->GetVar($this->dtPlace.'_page', 1));
					$currentPage = $request->GetVar($this->dtPlace.'_page', 1);
					$objects = $db->GetObjectsLimited($app->types[$this->dtPlace], NULL, $onPage, $onPage*($currentPage-1));
					$count   = $db->GetObjects($app->types[$this->dtPlace], NULL, DB_SELECT_COUNT);
					$layout->AppendTable('INNER.1.0', $this->dtPlace, $objects, $request->GetCid(), NULL, $currentPage, $onPage, $count);
					if ($user->level >= $app->types[$this->dtPlace]->nullrole) {
						$layout->AssignControls('INNER.1.2', 0, array($this->dtPlace));
						$layout->AssignContextMenu('INNER.1.0.CONTEXT', 0, array($this->dtPlace));
					}
					break;
				case 'adinterchange':
					if ($this->engine == 'external') {
						$this->adInterchange($app, $db);
						$layout->AssignError('INNER.1.0', "Данные обновлены");
					} else {
						$layout->AssignError('INNER.1.0', "Некорректный параметр <EM>sid</EM>");
					}
					break;
				default:
					$layout->AssignError('INNER.1.0', "Некорректный параметр <EM>sid</EM>");
			}

		} else {
			$layout->Assign('INNER.1.0', "<H2>Выберите кампанию</H2>");
		}
	}


	function adminPopup(&$app, &$request, &$user, &$db, &$layout) {
		$months = array(0 => '', 1=>'Январь', 2=>'Февраль', 3=>'Март', 4=>'Апрель', 5=>'Май', 6=>'Июнь',
								 7=>'Июль', 8=>'Август', 9=>'Сентябрь', 10=>'Октябрь', 11=>'Ноябрь', 12=>'Декабрь');

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

				Header("Content-type: image/png");
				header('Content-Disposition: inline; filename="stat_'.$id.'_'.$year.'_'.$month.'.png"');

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
				header('Content-type: application/csv');
				header('Content-Disposition: attachment; filename="stat_'.$id.'_'.$year.'_'.$month.'.csv"');

				$sql = 'SELECT TO_DATE(ts, \'DD-MM-YYYY\') AS date, SUM(show) as shows, SUM(click) AS clicks
						FROM adstat WHERE adbanner=\''.$id.'\'
						AND EXTRACT(MONTH FROM ts) = '.$month.' AND EXTRACT(YEAR FROM ts) = '.$year.'
						GROUP BY TO_DATE(ts, \'DD-MM-YYYY\') ORDER BY date';

				$res = $db->Query($sql);

				echo '"date";"shows";"clicks";"ctr"'. "\r\n";
				foreach ($res as $row) {
					echo '"'.$row['date'].'";"'.$row['shows']. '";"' .$row['clicks']. '";"' .number_format(100*($row['clicks']/$row['shows']),4,',','') .'"'. "\r\n";
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

	function adminAction(&$app, &$request, &$user, &$db) {

		//$action = $request->getVar("action");
		$stat   = $request->getVar('stat');

		$mode   = $request->getVar('mode');
		$format = $request->getFormat();

		$id     = (int)$request->getId();
		$year   = (int)$stat['year'];
		$month  = (int)$stat['month'];

		return 'popup.phtml?area=advert&format=adbanner&mode='.$mode.'&id='.$id.'&year='.$year.'&month='.$month;
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, $currentCid, $currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {
		$error = 0;
		$refName = $this->dtBanner.'2'.$this->dtPlace;
		if ($rqPlaceId = $request->GetVar('p')) {
			$adPlace = $db->GetObjectById($app->types[$this->dtPlace], $rqPlaceId);
			$attached = $db->GetLinks($app->references[$refName], $this->dtPlace, $rqPlaceId);
			if (count($attached)) {
				$banners = $db->GetObjectsByIdArray($app->types[$this->dtBanner], "TRUE", array_keys($attached));
			} else {
				$banners = $db->_GetData($app->types[$this->dtBanner], DB_SELECT_TABLE, "{$this->dtBanner}.type='{$adPlace['type']}' AND {$this->dtBanner}.dynamic = TRUE AND {$this->dtBanner}.status = TRUE");
			}
			if (count($banners)) {
				$distrib = array();
				foreach ($banners as $banner) {
					for ($i=0; $i<$banner['weight']; $i++) {
						$distrib[] = $banner['id'];
					}
				}
				$toShow = $banners[$distrib[array_rand($distrib)]];
				if ($request->GetVar('d') == 'gif') {
					setcookie('place'.$request->GetVar('p'), $toShow['id']);
					header('Location: '.$toShow['image']['path']);
				} else {
					$this->ShowAd($request->GetVar('p'), $toShow);
				}
				$toShow['shows'] = (is_numeric($toShow['shows'])) ? $toShow['shows']+1 : 1;
				$db->currentUser->level = 16384;
				$db->ModifyContentObject($app->types[$this->dtBanner], $toShow);
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


	function userAction(&$app, &$request, &$user, &$db) {
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
			$db->currentUser->level = 16384;
			$db->ModifyContentObject($app->types[$this->dtBanner], $adBanner);
		} else {
			$retValue = '/';
		}
		return $retValue;
	}

}

?>
