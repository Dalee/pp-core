$(function() {
	ValidateForms();
});

function ValidateForms() {
	for (i = 0; i < document.forms.length; i++) {
		document.submits = new Array();

		if(document.forms[i].onsubmit) {
			continue;
		}

		document.forms[i].onsubmit = function(e) {
			return TypicalValidateForm(this, true);
		}
	}

	document.onstop = function() {
		for(var i in document.submits) {
			document.submits[i].disabled = false;
		}
	}
}

function TypicalValidateForm(form, disable) {
	for(var i=0; i<form.elements.length; i++) {
		if(!TypicalValidateInput(form.elements[i])) {
			return ValidateNotice(form.elements[i]);
		}
	}

	if(disable === true) {
		for(var i in document.submits) {
			document.submits[i].disabled = true;
		}
	}

	return true;
}

function TypicalValidateInput(input) {
	//don't check invisible elements
	if (input.offsetWidth == 0) {
		return true
	}

	var value = input.value;

	switch(input.type) {
		case 'text':
		case 'password':
		case 'textarea':
			pattern = input.getAttribute('pattern');

			if(pattern) {
				switch(pattern) {
					case 'string':
						pattern = ".+";
						break;

					case 'number':
						pattern = "^[0-9]+$";
						break;

					case 'url':
						pattern = "^https?:\/\/(?:[a-z0-9_-]{1,32}(?::[a-z0-9_-]{1,32})?@)?(?:(?:[a-z0-9-]{1,128}\.)+(?:com|net|org|mil|edu|arpa|gov|biz|info|aero|inc|name|[a-z]{2})|(?!0)(?:(?!0[^.]|255)[0-9]{1,3}\.){3}(?!0|255)[0-9]{1,3})(?:\/[a-z0-9.,_@%&?+=\~\/-]*)?(?:#[^ '\"&<>]*)?$";
						value =  value.toLowerCase();
						break;

					case 'email':
						pattern = "^([a-z0-9_-]+)(\\.[a-z0-9_-]+)*@((([a-z0-9-]+\\.)+(com|net|org|mil|edu|gov|arpa|info|biz|inc|name|[a-z]{2}))|([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}))$";
						value =  value.toLowerCase();
						break;

					case 'mobile':
						value = value.replace(/[^0-9]/g, '');
						pattern = "^7?[0-9]{10}$";
						break;
				}

				input.setAttribute('pattern', pattern);
				input.value = value;

				if(!isPattern(pattern, value)) {
					return false;
				}
			}
			break;

		case 'radio':
		case 'checkbox':
			min = input.getAttribute('min') ? input.getAttribute('min') : 0;
			max = input.getAttribute('max') ? input.getAttribute('max') : document.getElementsByName(input.getAttribute('name')).length;

			if(min || max) {
				var items = document.getElementsByName(input.getAttribute('name'));
				var count = 0;

				for(var l=0; l<items.length; l++){
					if(items[l].checked) {
						count++;
					}
				}

				if(count < min || count > max) {
					return false;
				}
			}
			break;

		case 'select-one':
		case 'select-multiple':
			selected = input.options[input.selectedIndex];
			if(selected && selected.getAttribute('notselected')) {
				return false;
			}
			break;

		case 'file':
			break;

		case 'image':
		case 'submit':
			document.submits[document.submits.length] = input;
			break;

		case 'button':
		case 'reset':
			break;

		default:
			break;
	}

	return true;
}

function ValidateNoticeAlert() {
	alert.apply(arguments);
}

function ValidateNotice(input) {
	ValidateNoticeAlert(input.getAttribute('notice'));
	input.focus();
	return false;
}

function isPattern(pattern, str) {
	var re = new RegExp(pattern, "g");
	return re.test(str);
}
