function startCheckPassword(input, name) {
	setInterval(
		function() {
			checkPassword(input, name);
		},
		500
	);
}

function checkPassword(input, name) {
	var pType   = input.form.elements[name+'[type]'];
	var pReType = input.form.elements[name+'[retype]'];

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

function getViewportHeight() {
	var ua = navigator.userAgent.toLowerCase();
	var isOpera = (ua.indexOf('opera') > -1);
	var isIE = (!isOpera && ua.indexOf('msie') > -1);

	return ((document.compatMode || isIE) && !isOpera) ? (document.compatMode == 'CSS1Compat') ? document.documentElement.clientHeight : document.body.clientHeight : (document.parentWindow || document.defaultView).innerHeight;
}

function getDocumentHeight() {
	return Math.max(document.compatMode != 'CSS1Compat' ? document.body.scrollHeight : document.documentElement.scrollHeight, getViewportHeight());
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

//http://css-tricks.com/snippets/javascript/get-url-variables/
function GetQueryVariable(varName, defValue) {
	var vars = window.location.search.substring(1).split("&");
	for (var i=0; i<vars.length; i++) {
		var pair = vars[i].split("=");
		if (pair[0] == varName) {
		   return pair[1];
		}
	}
	return defValue;
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

function hideShowLeaf(id, format) {
	var position, direction, src, parentDiv, childrenDivs, expandImg, expandLink, href, tree, data;

	format = format || '';
	expandImg  = $('#leafImg' + format + id);
	parentDiv  = $('#leafId' + format + id);

	src       = expandImg.attr('src');
	position  = src.indexOf('close') == -1 ? 'open' : 'close';
	direction = position == 'close' ? 'open' : 'close';

	childrenDivs = parentDiv.children('div');

	if(direction == 'open' && !childrenDivs.length){
		data       = {f: format, id: id};
		tree       = expandImg.closest('div.tree');
		tree.data('cl') && (data.cl = tree.data('cl'));
		expandLink = expandImg.parent('a');
		href       = expandLink.attr('href');
		expandLink.removeAttr('href');
		expandImg.attr('src', src.replace('.gif', '-anim.gif'));
		$.ajax({
			url : '/admin/json.phtml?area=main',
			data: data,
			dataType: 'json',
			type: 'POST',
			success: function(data){
				if(data.branch){
					parentDiv.append(data.branch).children('div').show();
					src = src.replace(position, direction);
					document.cookie = 'leafStatus[leafId' + format + id + ']=' + direction;
				}
			},
			complete: function(){
				expandImg.attr('src', src);
				expandLink.attr('href', href);
			}
		});
	} else {
		expandImg.attr('src', src.replace(position, direction));
		document.cookie = 'leafStatus[leafId' + format + id + ']=' + direction;
		childrenDivs.toggle(direction == 'open');
	}

}

function Popup(url, width, height) {
	if (width  == undefined) width  = 760;
	if (height == undefined) height = 550;

	g = window.open(url, '', 'width='+width+', height='+height+',toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=yes');
	g.focus();
	false;
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
	window.location.href = 'action.phtml?id='+id+'&area=main&action=remove&ask=1&format='+format+'&referer='+escape(window.location.href);
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

		href = 'action.phtml?id='+id+'&area=objects&action=directmove&format='+format+'&shift='+shift+'&referer='+escape(window.location.href);
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
	var tableName = "table" + name;
	var domTable  = document.getElementById(tableName);
	var numRows   = domTable.rows.length;
	var newRow    = domTable.insertRow(numRows);
	var prevRow   = domTable.rows[numRows-1];
	var numCols   = prevRow.getElementsByTagName('td').length;

	for (var i=0;i<numCols;i++) {
		var newCell = newRow.insertCell(0);
		var j = 0;
		while(j < prevRow.cells[i].childNodes.length){
			var control = prevRow.cells[i].childNodes.item(j++);
			if(control.nodeType === 1 /*Node.ELEMENT_NODE; Node is undefined in IE*/ ) break;
		}

		if (control.tagName == 'TEXTAREA') {
			var width = control.style.width;
			var height = control.style.height;
			newCell.innerHTML = '<textarea name="'+name+'['+newRow.rowIndex+'][]" style="height:'+height+';"></textarea>';

		} else {
			var width = control.style.width;
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

	try {
		window.clipboardData.setData("Text", text);
		alert('Done');
	} catch(e) {
		alert('Your browser not support clipboard modify');
	}
}

function FromClipboard() {
	try {
		return window.clipboardData.getData('Text');
	} catch(e) {
		alert('Your browser not support clipboard modify');
	}
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

function renderContextMenu (title, items) {
	var ret = [];
	var area = GetQueryVariable('area', 'main');

	if (title) {
		ret.push('<strong>' + title + '</strong>');
	}

	function link (el) {
		var node = document.createElement(el.disabled? 'span' : 'a');

		var classes = '';
		if (el.block) {
			classes = el.block;
		}
		if (classes) {
			node.className = classes;
		}

		node.innerText = Array(el.content).join('');

		if (el.disabled) {
			return node.outerHTML;
		}

		if (el.url) {
			node.href = el.url;
			if (el.blank) node.target = '_blank';
		}
		else if (el.action) {
			node.href = 'javascript: ' + el.action;
		}

		if (el.confirm) {
			node.confirmText = typeof el.confirm === 'string'? el.confirm : 'Вы уверены?';
			node.onclick = "return window.confirm(this.confirmText);";
		}

		return node.outerHTML;
	}

	var groups = {'': []}, group, el;
	for (var i = 0, l = items.length; i < l; i += 1) {
		el = items[i];
		group = typeof el === 'string'? '' : (el.group || '');
		groups[group] = groups[group] || [];
		groups[group].push(String(
			(el.url || el.action) && el.content? link(el) : el
		));
	}

	ret.push($.map(groups, function (v) { return v.join(''); })
		.filter(function (v) { return v; })
		.join('<div class="hr"></div>'));

	return ret.join('');
}

function _ContextParamsAdd (parent, format, title) {
	var items = [];
	for (var i = 1, l = arguments.length; i < l; i += 2) {
		// each pair or elements are format and title
		items.push({id: arguments[i], title: arguments[i + 1]});
	}
	return {
		id      : Number(parent) || "''",
		formats : items
	};
}

function _ContextParamsEdit (id, status, format, title, alias, level, up, down) {
	return {
		id     : id,
		status : status,
		format : format,
		title  : title,
		alias  : alias,
		level  : level,
		up     : up,
		down   : down
	};
}

function _ContextParamsFile (title, isDir, urlAlias, isWrite, isDelete, isBinary, dir, href, side, isCopy, fileName, isBrokenFilename) {
	return {
		title    : title,
		isDir    : isDir,
		urlAlias : urlAlias,
		isWrite  : isWrite,
		isDelete : isDelete,
		isBinary : isBinary,
		dir      : dir,
		href     : href,
		side     : side,
		isCopy   : isCopy,
		fileName : fileName,

		isBrokenFilename : isBrokenFilename
	};
}

function ContextEdit (id, status, format, title, alias, level, up, down) {
	// format-id-params shortcuts
	var jsFIP = '(\'' + format + '\', ' + id + ')',
		urlFIP = 'action.phtml?area=objects&id=' + id + '&format=' + format + '&';

	return [
		// standard menu items
		{ group: 'standard', block: 'edit', content: 'Изменить', action: 'EditContent' + jsFIP },
		{ group: 'standard', block: 'copy', content: 'Клонировать', action: 'CloneContent' + jsFIP },
		{ group: 'standard', block: 'del',  content: 'Удалить', url: urlFIP + 'action=directremove',
			disabled: !level, confirm: 'Вы действительно хотите удалить ' + title + '?' },

		// status
		{ group: 'visibility', block: 'on', content: 'Опубликовать', url: urlFIP + 'action=directstatus',
			disabled: !(!status && level) },
		{ group: 'visibility', block: 'off', content: 'Скрыть', url: urlFIP + 'action=directstatus',
			disabled: !(status && level) },

		// ordering objects
		{ group: 'order', block: 'up', content: 'Поднять выше', url: urlFIP + 'action=directup',
			disabled: !up },
		{ group: 'order', block: 'down', content: 'Опустить ниже', url: urlFIP + 'action=directdown',
			disabled: !down },
		{ group: 'order', block: 'updown', content: 'Изменить позицию на …', action: 'MoveContent' + jsFIP,
			disabled: !(down || up) },

		// preview plugin ?
		{ group: 'preview', block: 'alias', content: 'Показать на сайте', url: '/admin/preview.phtml?q='+alias,
			disabled: !(alias.length), blank: true }
	];
}

function ContextFile (title, isDir, urlAlias, isWrite, isDelete, isBinary, dir, href, side, isCopy, fileName, isBrokenFilename) {
	var ret;
	var name = GetQueryVariable('name', 0);
	var outside = GetQueryVariable('action', 0);

	urlAlias = urlAlias !== '0'? urlAlias : '';

	// title-blabla-params shortcuts
	var jsP  = '(\'' + title + '\', \'' + dir + '\', \'' + href + '\', \'' + side + '\', \'' + outside + '\'); return false;',
		jsP2 = '(\'' + dir + '\', \'' + title + '\', \'' + href + '\', \'' + side + '\', \'' + outside + '\', \'' + fileName + '\'); return false;',
		urlP = 'action.phtml' + href + '&' + side + 'dir=' + dir + '&mdir=' + dir + '&mfile=' + fileName + '&side=' + side + '&outside=' + outside + '&',
		urlDP = href + '&' + side + 'dir=' + dir + fileName + '&side=' + side + '&outside=' + outside + '&',

		copyMoveDisabled = !isCopy || name || outside === 'filesarray' || isBrokenFilename,
		copyMoveConfirm = isCopy === 2 && [
			'В каталоге назначения уже есть файл/кататог с таким именем. ',
			'Вы действительно хотите переписать существующий файл/каталог ' + title + '?'
		].join('');

	return [
		// standard menu items
		{ group: 'standard', block: 'edit', content: 'Зайти в каталог', url: urlDP, visible: isDir },
		{ group: 'standard', block: 'edit', content: 'Изменить', action: 'EditFile' + jsP,
			visible: !isDir, disabled: isWrite === 0 || isBinary || isBrokenFilename },
		{ group: 'standard', block: 'unzip', content: 'Разархивировать', url: urlP + 'action=unzip',
			visible: !isDir, disabled: !isDelete || !isBinary || isBrokenFilename },

		// copy/move
		{ group: 'standard', block: 'move', content: 'Переместить', url: urlP + 'action=move',
			confirm: copyMoveConfirm, disabled: copyMoveDisabled && !isDelete },
		{ group: 'standard', block: 'copy', content: 'Скопировать', url: urlP + 'action=copy',
			confirm: copyMoveConfirm, disabled: copyMoveDisabled },

		{ group: 'standard', block: 'rename', content: 'Переименовать', action: 'RenameFile' + jsP2, disabled: !isDelete },
		{ group: 'standard', block: 'del', content: 'Удалить', action: 'RemoveFile' + jsP2, disabled: !isDelete },

		{ group: 'alias', block: 'alias', content: 'Скачать/Показать', url: urlAlias || '#', blank: true, disabled: !urlAlias },
		{ group: 'alias', block: 'memory', content: 'В буфер обмена', action: 'return InMemory(\'' + urlAlias + '\');', disabled: !urlAlias }
	];
}

/**
 * @param {Number|null} parent
 * @param {String} format
 * @param {String} title
 */
function ContextAdd (parent, format, title /* [, format, title]* */) {
	var params = _ContextParamsAdd.apply(this, arguments),
		items = [],
		format, i, l;

	for (i = 0, l = params.formats.length; i < l; i += 1) {
		format = params.formats[i];
		items.push({
			block: 'add',
			action: 'AddContent(\'' + format.id + '\', ' + params.id + ')',
			content: format.title
		});
	}

	return items;
}

/**
 * function Context (event: Event[, menuType: string[, args...]])
 * @param {Event} event
 */
function Context (event) {
	var args = Array.prototype.slice.call(arguments, 2);
	menu = document.getElementById('ContextMenu');
	menu.innerHTML = '';

	var menuType = arguments[1],
		pParsers = {
			def  : function () { return {}; },
			add  : _ContextParamsAdd,
			edit : _ContextParamsEdit,
			file : _ContextParamsFile
		},
		funcs    = {
			def  : ContextEdit,
			add  : ContextAdd,  // ContextAdd used by PXAdminHTMLLayout and PXAdminObjects
			edit : ContextEdit,
			file : ContextFile  // ContextFile function placed in /admin/js/filemanager.js
		},
		items    = [],
		params   = (pParsers[menuType] || pParsers.def).apply(this, args),
		mTitle   = menuType === 'add'? 'Добавить' : (params.title || args[0]);

	/** @todo rework it to have more flexible ctxMenu generator */
	if (menuType !== undefined) {
		var res = (funcs[menuType] || funcs.def).apply(null, args);
		items.push.apply(items, res);

		// plugins
		var additional = Context.fetch(menuType, args);
		items.push.apply(items, additional);
	}

	items.push({ group: 'last', action: 'ContextHide();', content: 'Отмена' });

	menu.innerHTML = renderContextMenu(mTitle, items.filter(function (v) {
		return !v.hasOwnProperty('visible') || v.visible;
	}));

	menu.style.display = 'block';
	x = _mousex + 12;
	y = _mousey + 12;
	h = menu.offsetHeight;
	w = menu.offsetWidth;

	dh = getDocumentHeight();
	dw = $(document).width();

	menu.style.left = x + w > dw ? dw - w - 24 : x;
	menu.style.top  = y + h > dh ? dh - h - 24 : y;
	menu.style.visibility = 'visible';

	if (window.event) {
		window.event.cancelBubble = true;
	}
	if (event.stopPropagation) {
		event.stopPropagation();
	}
}

/**
 * Storage for additional menu items for each menu type
 */
Context._addons = {};

/**
 * Registers menu item
 * @param {String} menuType
 * @param {String|String[]} item or items
 */
Context.register = function (menuType, item) {
	if (Array.isArray(item)) {
		for (var i = 0, l = item.length; i < l; i += 1) {
			Context.register(menuType, item[i]);
		}
		return;
	}

	var collection = Context._addons[menuType] = Context._addons[menuType] || [];
	collection.push(item);

	return Context;
};

/**
 * Prepare and fetch additional registered menu items
 * @param {String} menuType
 * @returns {Array}
 */
Context.fetch = function (menuType, args) {
	var funcs = {
			edit : _ContextParamsEdit,
			add  : _ContextParamsAdd,
			file : _ContextParamsFile,
			def  : function () { console.error('unknown menu type', arguments); return {}; }
		},
		params = (funcs[menuType] || funcs.def).apply(this, args);

	function fill (el) {
		switch (true) {
		case el === undefined:
			el = '';
		case el === false || el === true:
		case typeof el === 'Number' || el instanceof Number:
			break;
		case el && el.call && true:
			el = el.call(el, params, fill);
			break;
		case Array.isArray(el):
			el = el.map(fill);
			break;
		case typeof el === 'object':
			el = fillObject(el);
			break;
		case typeof el === 'string' || el instanceof String:
			el = String(el).replace(/:([\w\d]+)/g, function (m, k) {
				return params[k] || ('?' + k);
			});
			break;
		default:
			console.log('unhandled', typeof el, el);
		}
		return el;
	}

	function fillObject (obj) {
		var keys = Object.keys(obj), i, k, l, res = {};
		for (i = 0, l = keys.length; i < l; i += 1) {
			k = keys[i];
			res[k] = fill(obj[k]);
		}
		return res;
	}

	return (Context._addons[menuType] || [])
		.map(fill)
		.filter(function (v) {
			return v;
		});
};

function GetParentOffsetTop(obj) {
	if (!obj.offsetParent || obj.offsetParent.tagName === 'body') {
		return 0;
	}
	return obj.offsetTop + GetParentOffsetTop(obj.offsetParent) - obj.scrollTop;
}

function GetParentOffsetLeft(obj) {
	if (!obj.offsetParent || obj.offsetParent.tagName === 'body') {
		return 0;
	}
	return obj.offsetLeft + GetParentOffsetLeft(obj.offsetParent) - obj.scrollLeft;
}

function ContextHide() {
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
		'<object type="application/x-shockwave-flash" ' +
		'data="'+url+'" ' +
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

function ShowHideFilter(container){
	function isHidden(){
		return /ref-filter-hide/.test(this.className)
	}
	function isVisible(){
		return /ref-filter-show/.test(this.className)
	}
	function switchVisibility(){
		this.className = this.className.replace(/(ref-filter-)\w+/, '$1' + (isHidden.call(this) ? 'show' : 'hide'));
	}

	switchVisibility.call(container)
	var left = 0
	var th = container.parentNode.getElementsByTagName('th')
	for(var c in th){
		if(isVisible.call(th[c])) {
			left++
		}
	}
	if(left == 1){
		switchVisibility.call(th[0])
	}
}

var possibleNewRows = {}

function appendLinksRow(container){
	var selfTr = container.parentNode.parentNode;
	container.parentNode.removeChild(container);
	var newTr = document.createElement('tr');

	newTr.className = "newRow";

	var regx     = /((\w+)(\[|-))(\d+)((\]|-)\[?(\d+))/g

	regx.test(selfTr.innerHTML);
	var currentRefName = RegExp.$2;
	var currentRefId   = RegExp.$7;
	possibleNewRows[currentRefName][currentRefId]++;

	var selfCols = selfTr.childNodes;
	var htmlData = '';
	for(var td = 0; td < selfCols.length; td++){
		htmlData = selfCols.item(td).innerHTML;
		htmlData = htmlData.replace(regx, "$1"+possibleNewRows[currentRefName][currentRefId]+"$5");
		htmlData = htmlData.replace(/(selected|checked)/gi, '');
		htmlData = htmlData.replace(/(ref-linked)/gi, 'ref-unlinked');
		htmlData = htmlData.replace(/(<OPTION value="")/gi, '$1 selected="selected"');
		newTr.appendChild(document.createElement('td'));
		newTr.childNodes.item(td).innerHTML = htmlData;
	}

	if(neighbour = selfTr.nextSibling){
		selfTr.parentNode.insertBefore(newTr, neighbour)
	} else {
		selfTr.parentNode.appendChild(newTr);
	}
}
