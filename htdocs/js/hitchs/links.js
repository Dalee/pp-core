$(function() {
	$('a').each(function() {
		a = this;

		if(!a.href || (a.protocol !== 'http:' && a.protocol !== 'https:')) {
			return;
		}

		if(a.href.search(/#$/) !== -1 && a.onclick === null) {
			a.onclick = function() {
				window.scrollTo(0,0);
				return false;
			}
		}

		if(a.href && a.hostname != window.location.host && !a.target.length) {
			try {
				if(a.hostname.match(/(\w+\.\w+)$/)[0] !== window.location.host.match(/(\w+\.\w+)$/)[0]) {
					a.setAttribute('target', '_blank');
					a.setAttribute('title', a.hostname+' in new window');
				}

			} catch(e) {

			}
		}

		a.popup = a.getAttribute('popup');

		if(a.popup != null && a.popup != '') {
			a.onclick = function(e) {
				var popup = this.popup.split(' ');

				var w = popup[0];
				var h = typeof popup[1] != 'undefined' ? popup[1] : popup[0];

				var href = this.href;

				if(typeof popup[2] != 'undefined') {
					switch(popup[2]) {
						case 'image':
							if(popup.length > 3) {
								var t = '';
								for(k=3; k<popup.length; k++) {
									t += ' '+popup[k];
								}

								return openImagePopupWindow(href, w, h, t);

							} else {
								return openImagePopupWindow(href, w, h, href);
							}
							break;
					}

				} else {
					return openPopupWindow(href, w, h);
				}
			}
		}
	});
});

function openPopupWindow(url, width, height) {
	justPopupWindow(url, width, height);
	return false;
}

function justPopupWindow(url, width, height) {
	var w = window.open(url,'_blank','toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=yes,width='+width+',height='+height);
	w.focus();
}

function openImagePopupWindow(url, width, height, title) {
	var w = window.open(url, null, 'width='+width+', height='+height+', toolbar=no, status=no, location=no, menubar=no, resizable=yes, scrollbars=no');
	w.document.open();

	w.document.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">'+"\n");
	w.document.write('<html><head><title>'+title+'</title>');
	w.document.write('<style>html, body { padding: 0; margin: 0; background: #FFFFFF;  height: 100%; position: relative; cursor: hand;}');
	w.document.write(' img {position: absolute; left: 50%; top: 50%; margin-left: -'+Math.ceil(width / 2)+'px; margin-top: -'+Math.ceil(height / 2)+'px;} </style>');
	w.document.write('</head><body onclick="window.close();" title="'+title+"\n\n"+'Нажмите на картинку'+"\n"+'чтобы закрыть окно">');
	w.document.write('<img src="'+url+'" width="'+width+'" height="'+height+'" border="0" alt="'+title+"\n\n"+'Нажмите на картинку'+"\n"+'чтобы закрыть окно"></body></html>');

	w.document.close();
	w.focus();
	return false;
}
