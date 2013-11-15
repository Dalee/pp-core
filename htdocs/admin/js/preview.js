function Preview(file, type, name) {
	if(file.length) {
		if(type == 'image') {
			img = new Image();
			img.setAttribute('deal', name);
			img.onload = function () { ImagePreview(this.getAttribute('deal'), this.src, this.width, this.height, this.fileSize); this.clearAttributes();};
			img.onerror = function () { ImageError(this.getAttribute('deal')); this.clearAttributes();};
			img.src = file;
		}

		if(type == 'flash') {
			flash = document.getElementById(name+'src');
			flash.LoadMovie(0, file);

			flash.OnProgress = IsValidFlash(name, file);
		}
	}
}

function IsValidFlash(name, file) {
	flash = document.getElementById(name+'src');

	if(flash.PercentLoaded() == 100) {
		FlashPreview(name, file);
	} else {
		FlashError(name);
	}
}

function FlashError(name) {
	flash = document.getElementById(name+'src');
	flash.LoadMovie(0, 'i/v.swf');

	document.getElementById(name+'width').value  = '';
	document.getElementById(name+'height').value = '';
	document.getElementById(name+'size').value   = '';

	alert('Вы выбрали неправильный тип файла');
}

function FlashPreview(name, file) {
	document.getElementById(name+'width').value  = Math.ceil(flash.GetVariable("_width"));
	document.getElementById(name+'height').value  = Math.ceil(flash.GetVariable("_height"));
//	document.getElementById(name+'size').value   = size;
}

function ImageError(name) {
	image = document.getElementById(name+'src');
	image.src = 'i/v.gif';
	image.style.width = '100px';
	image.style.height = '100px';

	document.getElementById(name+'width').value  = '';
	document.getElementById(name+'height').value = '';
	document.getElementById(name+'size').value   = '';

	alert('Вы выбрали неправильный тип файла');
}

function ImagePreview(name, file, width, height, size) {
	image = document.getElementById(name+'src');

	if (width > 100 || height > 100) {
		w = ((width - height) > 0) ? 100 : Math.floor(100 * width/height);
		h = ((width - height) > 0) ? Math.floor(100 * height/width) : 100;
	} else {
		w = width;
		h = height;
	}
	image.style.width = w + 'px';
	image.style.height = h + 'px';
	image.src = file;

	document.getElementById(name+'width').value  = width;
	document.getElementById(name+'height').value = height;
	document.getElementById(name+'size').value   = size;
}
