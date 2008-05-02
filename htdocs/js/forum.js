// (C) Index 20, 2005
var message$body = null;

function hotKeys(e) {
	e = e || window.event;
	if (!e.ctrlKey) return;
	if (e.keyCode == 13) {
		this.form.submit();
	} else if (e.keyCode == 66 || e.charCode == 98) {
		document.getElementById('button-bold').onclick();
	} else if (e.keyCode == 73 || e.charCode == 105) {
		document.getElementById('button-italic').onclick();
	} else {
		return;
	}
	if (e.preventDefault) {
		e.preventDefault();
	} else {
		e.returnValue = false;
	}
}

function forum_bbTag(start, end) {
	if ('number' == typeof message$body.selectionEnd) {
		var s = message$body.selectionEnd + start.length + ((message$body.selectionStart == message$body.selectionEnd && start != '') ? 0 : end.length);
		var x = message$body.value.substr(0, message$body.selectionStart) +
			start + message$body.value.substring(message$body.selectionStart, message$body.selectionEnd) +
			end + message$body.value.substr(message$body.selectionEnd);
		message$body.value = x;
		message$body.setSelectionRange(s,s);
	} else if (typeof message$body._sel == 'object') {
		var sel = message$body._sel;
		if (sel.parentElement() == message$body) {
			var move_back = (sel.text == '' && start != '') ? - end.length : 0;
			sel.text = start + sel.text + end;
			sel.collapse();
			sel.move('character', move_back);
			sel.select();
		} else {
			message$body.value += start + end;
		}
	}
	message$body.focus();
}

function forum_init() {
	if (message$body == null) return;
	if ('number' != typeof message$body.selectionStart) {
		message$body._sel = message$body.createTextRange();
		message$body.onfocus = message$body.onselect = message$body.onclick = message$body.onkeyup = function() { this._sel = document.selection.createRange().duplicate(); };
		message$body.onkeydown = hotKeys;
	} else {
		message$body.onkeypress = hotKeys;
	}
	message$body.focus();
}

function forum_makeButtons(buttons) {
	if (message$body == null) return;
	var container = document.getElementById('forum-buttons');
	var b;
	for (var i = 0; i < buttons.length; i++) {
		var b = buttons[i];
		var button = document.createElement('button');
		if (button.type == 'submit') button.type = 'button';
		button.id = 'button-' + b.id;
		if (b.accesskey) button.accessKey = b.accesskey;
		button.appendChild(document.createTextNode(b.title));
		switch (b.type) {
			case 0:
				button.onclick = function (x, y) { return function(e) {forum_bbTag(x, y)} }(b.start, b.end);
				break;
			default:
				continue;
		}
		container.appendChild(button);
	}
}
