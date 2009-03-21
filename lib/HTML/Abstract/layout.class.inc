<?
include_once('Common/functions.array.inc');
include_once('Common/functions.string.inc');

define('TABLECOLOR1',       '#E7E9ED');
define('TABLECOLOR2',       '#C9CED6');
define('TABLECOLOR3',       '#385A94');

define('OUTER_INDEX', <<<HTML
<!-- DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" -->
<HTML>

	<HEAD>

		<TITLE>{OUTER.TITLE}</TITLE>
		<LINK href="css/default.css" rel="stylesheet">
		<SCRIPT type="text/javascript" src="js/default.js"></SCRIPT>
		<SCRIPT type="text/javascript" src="js/filemanager.js"></SCRIPT>
	</HEAD>

	<BODY bgcolor="#FFFFFF" text="#000000" link="#385A94" alink="#385A94" vlink="#385A94" onClick="javascript: ContextHide();">
		<TABLE width="100%" border="0" cellpadding="0" cellspacing="0" style="height: 100%;">
			<TR>
				<TD style="height: 45px;" valign="top">
					<TABLE border="0" width="100%" cellpadding="0" cellspacing="0" style="background-color:#385A94; background-image: url(i/head-bg.gif); background-repeat: repeat-x; height: 45px;">
						<TR>
							<TD>{OUTER.LOGO}</TD>
							<TD align="right" valign="bottom">{OUTER.MENU}</TD>
							<TD width="1%" nowrap>{OUTER.EXIT}</TD>
						</TR>
					</TABLE>
					<DIV class="context" id="ContextMenu" onClick="javascript: ContextHide();"></DIV>
				</TD>
			</TR>

			<TR>
				<TD style="height: 100%;" valign="top" height="100%">
					{OUTER.FORMBEGIN}
					{OUTER.MAINAREA}
					{OUTER.FORMEND}
				</TD>
			</TR>

			<TR>
				<TD style="height: 40px;" valign="bottom">
					<TABLE border="0" width="100%" cellpadding="0" cellspacing="0" style="background-color:#385A94; background-image: url(i/bottom-bg.gif); background-repeat: repeat-x;">
						<TR>
							<TD align="left">
								<DIV style="margin: 8px;color: #FFFFFF;">
								</DIV>
							</TD>

							<TD align="right">
								<TABLE border="0" cellpadding="0" cellspacing="0">
									<TR>
										<TD>
											<A href="/" target="_blank" class="button-exit">
												<IMG src="i/icon/alias.gif" border="0" width="11" height="11" alt="Перейти на сайт" align="absmiddle">&nbsp;Перейти на сайт
											</A>
										</TD>
									</TR>
								</TABLE>
							</TD>
						</TR>
					</TABLE>
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
HTML
);

define('OUTER_POPUP', <<<HTML
<!-- DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" -->
<HTML>

	<HEAD>

		<TITLE>{OUTER.TITLE}</TITLE>
		<LINK href="css/default.css" rel="stylesheet">
		<SCRIPT type="text/javascript" src="js/default.js"></SCRIPT>
		<SCRIPT type="text/javascript" src="js/preview.js"></SCRIPT>
		<SCRIPT type="text/javascript" src="js/filemanager.js"></SCRIPT>
	</HEAD>

	<BODY bgcolor="#D0D0D0" text="#000000" link="#385A94" alink="#385A94" vlink="#385A94" onClick="javascript: ContextHide();">
		<TABLE border="0" width="100%" cellpadding="0" cellspacing="0" style="height: 100%; background-color:#385A94;">
			<tr>
				<TD>
					<H1 style="color: #FFFFFF; margin: 10px;">{OUTER.TITLE}</H1>
				</TD>
			</tr>

			<TR style="background-image: url(i/head-bg.gif); background-repeat: repeat-x; background-position: 0 100%;">
				<TD valign="bottom" align="right">
					{OUTER.MENU}
				</TD>
			</TR>

			<TR>
				<TD style="height: 100%;">
					<DIV class="context" id="ContextMenu" onClick="javascript: ContextHide();"></DIV>
					{OUTER.FORMBEGIN}
					<TABLE border="0" width="100%" cellpadding="0" cellspacing="8" bgcolor="#C9CED6" style="height: 100%;">
						<TR>
							<TD align="left" colspan="2" style="height: 100%;">
								<DIV class="content">
									<TABLE width="100%" border="0" cellpadding="0" cellspacing="0" style="height: 100%;">
										<TR>
											<TD valign="top">
											{OUTER.CONTENT}
											</TD>
										</TR>
									</TABLE>
								</DIV>
							</TD>
						</TR>
						<TR id="popupcontrols">
							<TD align="left">{OUTER.LEFTCONTROLS}</TD>
							<TD align="right">{OUTER.RIGHTCONTROLS}</TD></TD>
						</TR>
					</TABLE>
					{OUTER.FORMEND}
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
HTML
);


define('OUTER_SIMPLE_POPUP', <<<HTML
<!-- DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" -->
<HTML>

	<HEAD>
		<TITLE>{OUTER.TITLE}</TITLE>
		<LINK href="css/default.css" rel="stylesheet">
		<SCRIPT type="text/javascript" src="js/default.js"></SCRIPT>
		<SCRIPT type="text/javascript" src="js/preview.js"></SCRIPT>
		<SCRIPT type="text/javascript" src="js/filemanager.js"></SCRIPT>
	</HEAD>

	<BODY bgcolor="#C9CED6" text="#000000" link="#385A94" alink="#385A94" vlink="#385A94" onClick="javascript: ContextHide();">
		<TABLE border="0" width="100%" cellpadding="0" cellspacing="8" style="height: 100%;">
			<TR>
				<TD style="height: 100%;" colspan="2">
					<DIV class="context" id="ContextMenu" onClick="javascript: ContextHide();"></DIV>
					{OUTER.FORMBEGIN}
					<TABLE border="0" width="100%" cellpadding="0" cellspacing="0" style="height: 100%;">
						<TR>
							<TD align="left" colspan="2" style="height: 100%;">
								<DIV class="content">
									<TABLE width="100%" border="0" cellpadding="0" cellspacing="0" style="height: 100%;">
										<TR>
											<TD valign="top">
											{OUTER.CONTENT}
											</TD>
										</TR>
									</TABLE>
								</DIV>
							</TD>
						</TR>
						<TR id="popupcontrols">
							<TD align="left">{OUTER.LEFTCONTROLS}</TD>
							<TD align="right">{OUTER.RIGHTCONTROLS}</TD></TD>
						</TR>
					</TABLE>
					{OUTER.FORMEND}
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
HTML
);

define('FORM_LOGIN', <<<HTML
<TABLE border="0" cellpadding="3" cellspacing="0" align="center" style="height: 100%;">
	<TR>
		<TD valign="middle">
			<DIV style="border: 1px solid #D2D2D2; border-bottom-color: #404040; border-right-color: #404040;">
				<DIV style="border: 1px solid #ECECEC; border-bottom-color: #919191; border-right-color: #919191;">
					<TABLE border="0" cellpadding="3" cellspacing="1" align="center" bgcolor="#C9CED6">
						<TR>
							<TD bgcolor="#385A94">
								<STRONG style="color: #FFFFFF;">Авторизация</STRONG>
							</TD>
						</TR>

						<TR>
							<TD>
								<TABLE border="0" cellpadding="0" cellspacing="10" width="325">
									<COLGROUP>
										<COL width="20%"/>
										<COL width="20%"/>
										<COL width="60%"/>
									</COLGROUP>

									<TR>
										<TD rowspan="3" valign="top">
											<IMG src="i/auth.gif" width="30" height="32" alt="Пожалуйста введите свой логин и пароль" hspace="10"><!--
										--></TD>

										<TD colspan="2">
											{LOGIN.ERROR}
											Пожалуйста, введите свой логин и пароль
										</TD>
									</TR>

									<FORM action="{LOGIN.FORMACTION}" method="{LOGIN.FORMMETHOD}">
										<INPUT type="hidden" name="{LOGIN.REFERERNAME}" value="{LOGIN.REFERERVALUE}">
										<INPUT type="hidden" name="{LOGIN.AREANAME}"    value="{LOGIN.AREAVALUE}">

										<TR>
											<TD>логин</TD>

											<TD width="100%">
												<INPUT type="text" name="{LOGIN.LOGINNAME}" value="{LOGIN.LOGINVALUE}" style="width:100%;">
											</TD>
										</TR>

										<TR>
											<TD>пароль</TD>

											<TD>
												<INPUT type="password" name="{LOGIN.PASSWDNAME}" style="width:100%;">
											</TD>
										</TR>

										<TR>
											<TD align="right" colspan="3">
												<INPUT type="submit" value="войти" class="button">
											</TD>
										</TR>
									</FORM>
								</TABLE>
							</TD>
						</TR>
					</TABLE>
				</DIV>
			</DIV>
		</TD>
	</TR>
</TABLE>
HTML
);

define('FORM_LOGOUT', <<<HTML
<A href="{LOGOUT.HREF}" class="button-exit">
	<IMG src="i/icon/exit.gif" border="0" width="11" height="11" alt="Выйти" align="absmiddle">&nbsp;Выйти
</A>
HTML
);

class NLAbstractLayout {
	var $root;
	var $objects;
	var $getData;
	var $disableForms;

	function NLAbstractLayout() {
		$this->root         = '<html></html>';
		$this->objects      = array();
		$this->getData      = array();
		$this->disableForms = false;
	}

	function SetOuterLayout($type) {
		switch ($type) {
			case 'index':
				$this->root = OUTER_INDEX;
				break;
			case 'popup':
				$this->root = OUTER_POPUP;
				break;
			case 'simple':
				$this->root = OUTER_SIMPLE_POPUP;
				break;
		}
	}

	function SetInnerLayout($table) {
		$html  = '<TABLE border="0" width="100%" cellpadding="0" cellspacing="8" bgcolor="#C9CED6" style="height: 100%;">';
		foreach ($table as $rk => $row) {
			$html .= '<TR valign="top">';
			foreach ($row as $ck => $col) {
				$html .= '<TD';
				if (!empty($col[0])) $html .= ' width="'.$col[0].'"';
				if (!empty($col[1])) $html .= ' style="height:'.$col[1].'"';
				if (!empty($col[2])) $html .= ' colspan="'.$col[2].'"';
				if (!empty($col[3])) $html .= ' rowspan="'.$col[3].'"';
				$html .= '><DIV {INNER.'.$ck.'.'.$rk.'.CONTEXT}';
				if (!empty($col[1])) $html .= ' class="content"';
				$html .= '>{INNER.'.$ck.'.'.$rk.'}';
				$html .= '</DIV></TD>';
			}
			$html .= '</TR>';
		}
		$html .= '</TABLE>';
		$this->Assign('OUTER.MAINAREA', $html);
	}

	function SetSimpleInnerLayout($widthArray, $heightArray) {
		$table = array();
		foreach ($heightArray as $hk => $height) {
			$table[$hk] = array();
			foreach ($widthArray as $wk => $width) {
				$table[$hk][$wk] = array($width, $height, NULL, NULL);
			}
		}
		$this->SetInnerLayout($table);
	}

	function SetOuterForm($action, $method, $enctype, $autoHeight=false) {
		$this->Assign('OUTER.FORMBEGIN', '<FORM action="'.$action.'" method="'.$method.'" name="outer" enctype="'.$enctype.'" class="edit'.($autoHeight ? ' autoheight' : '').'">');
		$this->Assign('OUTER.FORMEND', '</FORM>');
	}


	function SetOuterLogo($image, $text, $width, $height, $href='') {
		$html  = '';
		if (!empty($image)) {
			$html .= '<A href="'.$href.'"><IMG src="'.$image.'" width="'.$width.'" height="'.$height.'" border="0" alt="'.$text.'"></A>';
		} else {
			$pad = round($height-22)/2;
			$html .= '<DIV style="width:'.$width.';height:'.$height.';text-align:center;padding:'.$pad.';"><H1><A href="'.$href.'" style="color:#FFFFFF;">'.$text.'</A></H1></DIV>';
		}
		$html .= '';
		$this->Assign('OUTER.LOGO', $html);
	}

	function SetMenu($array, $current, $getParam='area') {
		$html = '<TABLE border="0" cellpadding="0" cellspacing="0"><TR>';
		$i = 0;
		foreach ($array as $k=>$v) {
			if ($i == 0) {
				if ($current == $k) {
					$html .= '<TD><IMG src="i/tab/1-on.gif" width="4" height="27" alt=""></TD>';
					$currentI = 1;
				} else {
					$html .= '<TD><IMG src="i/tab/1-off.gif" width="4" height="27" alt=""></TD>';
				}
			} else {
				if ($current == $k) {
					$html .= '<TD><IMG src="i/tab/n.gif" width="4" height="27" alt=""></TD>';
					$currentI = $i+1;
				} else {
					if (isset($currentI) && $currentI == $i) {
						$html .= '<TD><IMG src="i/tab/p.gif" width="4" height="27" alt=""></TD>';
					} else {
						$html .= '<TD><IMG src="i/tab/off.gif" width="4" height="27" alt=""></TD>';
					}
				}
			}
			if ($current == $k) {
				$bg = 'on';
			} else {
				$bg = 'off';
			}
			$html   .= '<TD nowrap align="center" class="tabs" background="i/tab/bg-'.$bg.'.gif"><A href="'.$this->_BuildHref($getParam, $k).'" class="tabs">';
			$html   .= $v;
			$html   .= '</A>';
			$html   .= '</TD>';
			$i++;
		}
		if (isset($currentI) && $currentI == sizeof($array)) {
			$html .= '<TD><IMG src="i/tab/l-on.gif" width="4" height="27" alt=""></TD>';
		} else {
			$html .= '<TD><IMG src="i/tab/l-off.gif" width="4" height="27" alt=""></TD>';
		}
		$html .= '</TR></TABLE>';
		$this->Assign('OUTER.MENU', $html);
	}

	function SetLogoutForm($href) {
		$this->Assign('OUTER.EXIT',  FORM_LOGOUT);
		$this->Assign('LOGOUT.HREF', $href);
	}

	function SetLoginForm($formAction, $formMethod, $namesArray, $valuesArray) {
		$this->Assign('OUTER.MAINAREA',   FORM_LOGIN);
		$this->Assign('LOGIN.FORMACTION', $formAction);
		$this->Assign('LOGIN.FORMMETHOD', $formMethod);

		$this->Assign('LOGIN.LOGINNAME',    GetFromArray($namesArray, 'login'));
		$this->Assign('LOGIN.PASSWDNAME',   GetFromArray($namesArray, 'passwd'));
		$this->Assign('LOGIN.LOGINVALUE',   GetFromArray($valuesArray, 'login'));
		$this->Assign('LOGIN.REFERERNAME',  GetFromArray($namesArray, 'referer'));
		$this->Assign('LOGIN.REFERERVALUE', GetFromArray($valuesArray, 'referer'));
		$this->Assign('LOGIN.AREANAME',     GetFromArray($namesArray, 'area'));
		$this->Assign('LOGIN.AREAVALUE',    GetFromArray($valuesArray, 'area'));
	}

	function SetGetVarToSave($key, $value) {
		$this->getData[$key] = $value;
		$this->objects[strtoupper($key)] = (is_array($value)) ? 'ARRAY' : $value;
	}

	function ClearGetVar($key) {
		unset($this->getData[$key]);
		$this->objects[strtoupper($key)] = '';
	}

	function Assign($name, $value) {
		$this->objects[$name] = $value;
	}

	function Append($name, $value) {
		if (!isset($this->objects[$name])) $this->objects[$name] = NULL;
		$this->objects[$name] .= $value;
	}

	function AssignKeyValueList($label, $list, $varName, $selected) {
		$list = new PxAdminList($list);
		$list->setVarName($varName);
		$list->setSelected($selected);
		$list->setGetData($this->getData);
		$this->Assign($label, $list->html());
	}

	function Flush($charset = NULL) {
		$result = $this->_DeleteLabels($this->_Proceed($this->root, $this->objects));

		if($charset) {
			if(defined('DEFAULT_CHARSET') && $charset != DEFAULT_CHARSET) {
				$result = myconv(DEFAULT_CHARSET, $charset, $result);

			} else {
				$charset = null;
			}
		}

		$response =& PXResponse::getInstance();
		$response->setCharset($charset);
		$response->send($result);
	}

	function _Proceed($template, $items) {
		foreach ($items as $k=>$v) {
			if(is_array($v)) {
				d2($v);
			}

			$template = str_replace("{".$k."}", $v, $template);
		}
		return $template;
	}

	function _BuildHref($key, $value) {
		$href = "?";
		foreach ($this->getData as $k=>$v) {
			if (!empty($v) && $k != $key) {
				if (is_array($v)) {
					foreach ($v as $sk=>$sv) {
						$href .= $k.'[]='.urlencode($sv).'&';
					}
				} else {
					$href .= $k.'='.urlencode($v).'&';
				}
			}
		}
		$href .= $key.'='.$value.'&';
		$href = substr($href, 0, -1);
		return $href;
	}

	function _DeleteLabels($template) {
		return preg_replace("/\{\w[\w\d_\.]+[\w\d]\}/", "", $template);
	}

}

?>
