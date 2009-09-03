function startCheckPassword(input, name) {
	checkPassord = setInterval(
		function() {
			checkPassword(input, name);
		},
		500
	);
}

function checkPassword(input, name) {
	pType   = input.form.elements[name+'[type]'];
	pReType = input.form.elements[name+'[retype]'];

	if(input === pType) {
		if(input.value.length == 0) {
			input.className = '';

		} else if(input.value.length < 6) {
			input.className = 'bad';

		} else {
			input.className = 'good';

			if(input.value.search(/[0-9]/) < 0) {
				input.className = 'notbad';
			}

			if(input.value.search(/[a-z]/) < 0) {
				input.className = 'notbad';
			}

			if(input.value.search(/[A-Z]/) < 0) {
				input.className = 'notbad';
			}
		}
	}

	if(input === pReType) {
		if(input.value.length) {
			input.className = (pType.value == pReType.value) ? 'good' : 'bad';
		} else {
			input.className = '';
		}
	}
}

function ShowUnLinking(name, a) {
	links = document.getElementsByName(name);
	for(var i=0; i<links.length; i++) {
		links[i].style.display = 'block';
	}

	a.outerHTML = '<a href="hide_unlinked" onClick="HideUnLinking(\''+name+'\', this); return false;" class="white">&#x2193;</a>';
}

function HideUnLinking(name, a) {
	links = document.getElementsByName(name);
	for(var i=0; i<links.length; i++) {
		links[i].style.display = 'none';
	}

	a.outerHTML = '<a href="show_unlinked" onClick="ShowUnLinking(\''+name+'\', this); return false;" class="white">&#x2191;</a>';
}


function GetQueryVariable(varName, defValue) {
	var begin = location.search.indexOf(varName) + varName.length + 1;
	if (begin != 4) {
		var string = location.search.substring(begin);
		var end = string.indexOf("&");
		if (end != -2) {
			string = string.substring(0, end);
		}
		return string;
	} else {
		return defValue;
	}
}

function ChangeRichEdit(radio, textedit, text, richedit, edit) {
	if (radio.checked) {
		// TEXT to HTML
		textedit.style.visibility = 'hidden';
		richedit.style.visibility = 'visible';
		edit.innerHTML = text.value;

	} else {
		// HTML to TEXT
		// richedit.style.visibility = 'hidden';
		textedit.style.visibility = 'visible';
		text.value = edit.innerHTML;
	}
}

function OpenClose(id) {
	document.all[id].style.display = (document.all[id].style.display == 'block') ? 'none' : 'block';
}

function hideShowLeaf(id) {
	var direction = (document.getElementById('leafImg'+id).src.indexOf('close') != -1) ? 'open' : 'close';

	document.getElementById('leafImg'+id).src = 'i/tree/'+direction+'.gif';
	document.cookie = 'leafStatus[leafId'+id+']='+direction;

	parentDiv = document.getElementById('leafId'+id);

	for (i=0; i<parentDiv.childNodes.length; i++) {
		if (parentDiv.childNodes[i].tagName == 'DIV') {
			parentDiv.childNodes[i].style.display = (direction == 'open') ? 'block' : 'none';
		}
	}
} 

function Popup(url, width, height) {
	if (width  == undefined) width  = 760;
	if (height == undefined) height = 550;

	g = window.open(url, '', 'width='+width+', height='+height+',toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=yes');
	g.focus();
}


function AdminPopup(area, format, id, action, width, height) {
	if (action == undefined) action = 'main';
	if (width  == undefined) width  = 760;
	if (height == undefined) height = 550;

	Popup('popup.phtml?area='+area+'&format='+format+'&id='+id+'&action='+action, width, height);
}

function EditContent(format, id, action) {
	if (action == undefined) action = 'main';
	Popup('popup.phtml?area=objects&format='+format+'&id='+id+'&action='+action);
}

function EditLinkToFile(name, dir) {
	Popup('popup.phtml?area=file&mdir='+dir+'&ldir='+dir+'&rdir='+dir+'&side=l&name='+name+'&action=link');
}

function DelContent(format, id) {
	window.location.href = 'action.phtml?id='+id+'&area=main&action=remove&ask=1&format='+format+'&referer='+window.location.href;
}

function AddContent(format, parent) {
	Popup('popup.phtml?area=objects&format='+format+'&id=0&action=main&parent='+parent);
}

function CloneContent(format, donor) {
	Popup('action.phtml?area=objects&format='+format+'&id='+donor+'&action=clone');
}


function MoveContent(format, id) {
	try {
		var shift = window.prompt('Укажите сдвиг позиции (отрицательные вниз, положительные вверх)', -1);

		if(shift === 0) {
			return false;
		}

		href = 'action.phtml?id='+id+'&area=objects&action=directmove&format='+format+'&shift='+shift+'&referer='+window.location.href.replace('&', '%26');
		window.location.href = href;
	} catch(e) {
	}
}

// obsolete??
function AddRow(table, name, width, height, cols) {
	domTable = document.getElementById(table);
	row = domTable.insertRow();
	for (i=0;i<cols;i++) {
		cell = row.insertCell();
		if (height == 0) {
			cell.innerHTML = '<input type="text" name="'+name+'['+row.rowIndex+'][]" value="">';
		} else {
			cell.innerHTML = '<textarea name="'+name+'['+row.rowIndex+'][]" style="height:'+height+'px;"></textarea>';
		}
	}
}

function AddRowNew(name) {
	tableName = "table" + name;
	domTable = document.getElementById(tableName);
	numRows  = domTable.rows.length;
	numCols  = domTable.cells.length/numRows;
	newRow = domTable.insertRow();
	prevRow = domTable.rows[numRows-1];
	for (i=0;i<numCols;i++) {
		newCell = newRow.insertCell();
		control = prevRow.cells[i].childNodes.item(0);

		if (control.tagName == 'TEXTAREA') {
			width = control.style.width;
			height = control.style.height;
			newCell.innerHTML = '<textarea name="'+name+'['+newRow.rowIndex+'][]" style="height:'+height+';"></textarea>';

		} else {
			width = control.style.width;
			newCell.innerHTML = '<input type="text" name="'+name+'['+newRow.rowIndex+'][]" value="">';
		}
	}
}

function UncheckByValue(status, value) {
	for (i=0; i<document.forms[0].elements.length; i++) {
		if (document.forms[0].elements[i].value == value) {
			if (status == false && document.forms[0].elements[i].checked == true) {
				document.forms[0].elements[i].checked = false;
			}
		}
	}
}

function SelectAll(f) {
	for (i=0; i<document.forms[f].elements.length; i++) {
		if (document.forms[f].elements[i].type == 'checkbox') {
			document.forms[f].elements[i].checked = true;
		}
	}
	return false;
}

function SelectAllDiff(s, f) {
	var t;
	for (i=0; i<document.forms[f].elements.length; i++) {
		if (document.forms[f].elements[i].type == 'checkbox') {
			t = ',' + document.forms[f].elements[i].value + ',';
			if (s.indexOf(t) != -1) {
				document.forms[f].elements[i].checked = true;
			}
		}
	}
	return false;
}

function ToClipboard(src, width, height) {
	return ToClipboardMulti(src, width, height, 'image');
}

function ToClipboardMulti(src, width, height, type) {
	text = '';

	if(type == 'file') {
		text = '<a href="'+src+'">Скачать</a>';
	}

	if(type == 'image') {
		text = '<img src="'+src+'" width="'+width+'" height="'+height+'" alt="" />';
	}

	if(type == 'flash') {
		text += '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"';
		text += ' codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=4,0,2,0"';
		text += ' width="'+width+'" height="'+height+'">';
		text += '<param name="movie" value="'+src+'">';
		text += '<param name="menu" value="false">';
		text += '<param name="quality" value="high">';
		text += '<param name="wmode" value="transparent">';
		text += '<embed src="'+src+'" menu="false" quality="high" wmode="transparent"';
		text += ' swliveconnect="false" width="'+width+'" height="'+height+'"';
		text += ' type="application/x-shockwave-flash"';
		text += ' pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash">';
		text += '</embed>';
		text += '</object>';
	}

	window.clipboardData.setData("Text", text);
	alert('Готово');
}

function FromClipboard() {
	return window.clipboardData.getData('Text');
}

function nl2p(text) {
	elem = text.value;
	while(elem.match(/\r?\n\r?\n/)) {
		elem = elem.replace(/\r?\n\r?\n/, '</p><p>');
	}
	text.value = elem;
}

function ContextCall(id, status, format, title) {
	return Context('Edit', format, id, 'Изменить ' + title);
}

function ContextEdit(id, status, format, title, alias, level, up, down) {
	var ret;
	var area = GetQueryVariable('area', 'main');

	ret  = '<strong>'+title+'</strong>';
	ret += '<a class="edit" href="javascript: EditContent(\''+format+'\', '+id+')">Изменить</a>';
	ret += '<a class="copy" href="javascript: CloneContent(\''+format+'\', '+id+')">Клонировать</a>';

	if(level) {
		ret += '<a class="del" href="action.phtml?id='+id+'&area=objects&action=directremove&format='+format+'" onclick="return window.confirm(\'Вы дейтвительно хотите удалить '+title+'?\');">Удалить</a>';
	} else {
		ret += '<span class="del">Удалить</span>';
	}

	ret += '<div class="hr"></div>';

	if (status != 1 && level) {
		ret += '<a class="on" href="action.phtml?id='+id+'&area=objects&action=directstatus&format='+format+'">Опубликовать</a>';
	} else {
		ret += '<span class="on">Опубликовать</span>';
	}

	if(status == 1 && level) {
		ret += '<a class="off" href="action.phtml?id='+id+'&area=objects&action=directstatus&format='+format+'">Скрыть</a>';
	} else {
		ret += '<span class="off">Скрыть</span>';
	}

	ret += '<div class="hr"></div>';

	if(up) {
		ret += '<a class="up" href="action.phtml?id='+id+'&area=objects&action=directup&format='+format+'">Поднять выше</a>';
	} else {
		ret += '<span class="up">Поднять выше</span>';
	}

	if(down) {
		ret += '<a class="down" href="action.phtml?id='+id+'&area=objects&action=directdown&format='+format+'">Опустить ниже</a>';
	} else {
		ret += '<span class="down">Опустить ниже</span>';
	}

	if(up || down) {
		ret += '<a href="javascript:MoveContent(\''+format+'\', '+id+');">Изменить позицию на&hellip;</a>';
	} else {
		ret += '<span>Изменить позицию на&hellip;</span>';
	}

	ret += '<div class="hr"></div>';

	if (alias.length) {
		ret += '<a class="alias" href="../'+alias+'" target="_blank">Показать на сайте</a>';
	} else {
		ret += '<span class="alias">Показать на сайте</span>';
	}

	return ret;
}

function ContextAdd(parent, format, title) {
	return '<a class="add" href="javascript: AddContent(\''+format+'\', '+parent+')">'+title+'</a>';
}

function Context(div) {
	menu = document.getElementById('ContextMenu');
	menu.innerHTML = '';

	if(arguments.length > 0) {
		if (arguments[0] == 'add') {
			menu.innerHTML += '<strong>Добавить</strong>';
			for(i=2; i<arguments.length; i+=2) {
				menu.innerHTML += ContextAdd(arguments[1], arguments[i], arguments[i+1]);
			}

		} else if (arguments[0] == 'file') {
			menu.innerHTML += ContextFile(arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7], arguments[8], arguments[9], arguments[10]);

		} else {
			menu.innerHTML += ContextEdit(arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7], arguments[8]);
		}
	}

	menu.innerHTML += '<div class="hr"></div>';
	menu.innerHTML += '<a href="javascript: ContextHide();">Отмена</a>';

	menu.style.display = 'block';
	x = _mousex + 12;
	y = _mousey + 12;
	h = menu.offsetHeight;
	w = menu.offsetWidth;

	dh = document.body.clientHeight;
	dw = document.body.clientWidth;

	menu.style.left = x + w > dw ? dw - w - 24 : x;
	menu.style.top  = y + h > dh ? dh - h - 24 : y;
	menu.style.visibility = 'visible';
}

function GetParentOffsetTop(obj) {
	if(obj.offsetParent && obj.offsetParent.tagName != 'body') {
		return obj.offsetTop + GetParentOffsetTop(obj.offsetParent) - obj.scrollTop;
	} else {
		return  0;
	}
}

function GetParentOffsetLeft(obj) {
	if(obj.offsetParent && obj.offsetParent.tagName != 'body') {
		return obj.offsetLeft + GetParentOffsetLeft(obj.offsetParent) - obj.scrollLeft;
	} else {
		return  0;

	}
}
function ContextHide () {
	menu = document.getElementById('ContextMenu');
	menu.style.display    = 'none';
	menu.style.visibility = 'hidden';
}


_mousex = 0
_mousey = 0

if (document.getElementById && navigator.appName=="Netscape") {
	document.onmousemove=function(e){
		_mousex = e.pageX
		_mousey = e.pageY
		return true
	}
} else {
	document.onmousemove=function(){
		_mousex=event.clientX + document.body.scrollLeft
		_mousey=event.clientY + document.body.scrollTop
		return true
	}
}

_editor_url = "tools/htmlarea/";
var win_ie_ver = parseFloat(navigator.appVersion.split("MSIE")[1]);
if (navigator.userAgent.indexOf('Mac')        >= 0) { win_ie_ver = 0; }
if (navigator.userAgent.indexOf('Windows CE') >= 0) { win_ie_ver = 0; }
if (navigator.userAgent.indexOf('Opera')      >= 0) { win_ie_ver = 0; }
if (win_ie_ver >= 5.5) {
	document.write('<scr' + 'ipt src="' +_editor_url+ 'editor.js"');
	document.write(' language="Javascript1.2"></scr' + 'ipt>');
} else {
	document.write('<scr'+'ipt>function editor_generate() { return false; }</scr'+'ipt>');
}


// Flash detection
var plugin = (navigator.mimeTypes && navigator.mimeTypes["application/x-shockwave-flash"]) ? navigator.mimeTypes["application/x-shockwave-flash"].enabledPlugin : 0;

if (plugin) {
	plugin = parseInt(plugin.description.substring(plugin.description.indexOf(".")-1)) >= 6;
} else if (navigator.userAgent && navigator.userAgent.indexOf("MSIE")>=0 && (navigator.userAgent.indexOf("Windows 95")>=0 || navigator.userAgent.indexOf("Windows 98")>=0 || navigator.userAgent.indexOf("Windows NT")>=0)) {
	document.write('<script language="VBScript"\> \n');
	document.write('on error resume next \n');
	document.write('plugin = ( IsObject(CreateObject("ShockwaveFlash.ShockwaveFlash.6")))\n');
	document.write('</script\> \n');
}

function ShowFlash(url, w, h, id) {
	document.write(
		'<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" ' +
		'codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" ' +
		'width="'+w+'" height="'+h+'" id="'+id+'">' +

		'<param name="movie"   value="'+url+'">'    +
		'<param name="menu"    value="false">'      +
		'<param name="quality" value="high">'       +
		'<param name="wmode"   value="transparent">'+
		'<param name="volume"  value="mute">'       +

		'</object>'
	);
}

$(function() {
	$('select[name=objecttype]').change(function() {
		if ($('input[name=area]').val() == 'macl') {
			var currentSelect = $(this).val();
		  	var nextSelect = $('select[name=what]');
	
			$.getJSON('/admin/json.phtml?area=macl', {currentModule:currentSelect}, function(data) {
				$('select[name=what] option').remove();
				
				$.each(data, function(index, value){
					$("<option></option>").attr("value", index).html(value).appendTo(nextSelect); 
					$('select[name=what] option:first').attr('selected', 'yes');
		    	});
			});	
		}
		return false;		
	});
});
