function getCookie(cookieName) {
	var cname = cookieName + "=";
	var i = 0;
	while (i < document.cookie.length) {
		var j = i + cname.length;
		if (document.cookie.substring(i, j) == cname){
			var leng = document.cookie.indexOf (";", j);
			if (leng == -1) {
				leng = document.cookie.length;
			}
			return unescape(document.cookie.substring(j, leng));
		}
		i = document.cookie.indexOf(" ", i) + 1;
		if (i == 0) {
			break;
		}
	}
	return 0;
}

function setCookie(cookieName, cookieValue, expire) {
	if (typeof expire == 'undefined') {
		document.cookie = cookieName + ' = ' + cookieValue;
	} else {
		var today = new Date();
		var expiry = new Date(today.getTime() + expire);
		document.cookie = cookieName + ' = ' + cookieValue;  + "; expires=" + expiry.toGMTString();
	}
}

function deleteCookie(cookieName) {
	var expire = new Date();
	expire.setTime (expire.getTime() - 2 * 86400001);
	document.cookie = cookieName + "=*; expires=" + expire.toGMTString();
}
