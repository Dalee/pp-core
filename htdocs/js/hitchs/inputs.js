$(function() { 
	$('input').each(function() {
		className = '';

		switch(this.type) {
			case 'text':
			case 'password':
				className = 'textfield';
				break;

			case 'radio':
			case 'checkbox':
				className = 'radiobutton';
				break;

			case 'image':
				className = 'image';
				break;

			case 'submit':
			case 'button':
			case 'reset':
				className = 'submit';
				break;
		}

		if(className.length) {
			$(this).addClass(className);
		}
	});
});