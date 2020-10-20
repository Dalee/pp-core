function LinkFile(name, dir, file, size) {
	opener.document.getElementById(name+'_filename').value = file;
	opener.document.getElementById(name+'_dir').value      = dir;
	opener.document.getElementById(name+'_size').value     = size;
	opener.document.getElementById(name+'_fullpath').value = dir + file;
	document.cookie = 'fileManagerDir' + '=' + dir;
	window.close();
	return false;
}

function InMemory(url) {
	try {
		window.clipboardData.setData('Text', url);
		alert('Готово');
	} catch (e) {
		//noop
	}
	return false;
}

function UnLinkFile(name, selfChange) {
	if(selfChange) {
		document.getElementById(name+'_filename').value = '';
		document.getElementById(name+'_dir').value      = '';
		document.getElementById(name+'_size').value     = '';
		document.getElementById(name+'_fullpath').value = '';

	} else {
		opener.document.getElementById(name+'_filename').value = '';
		opener.document.getElementById(name+'_dir').value      = '';
		opener.document.getElementById(name+'_size').value     = '';
		opener.document.getElementById(name+'_fullpath').value = '';
		window.close();
	}
	return false;
}

function ShowUploadForm(id) {
	t = document.getElementById(id);
	t.className = t.className.indexOf('hide') > 0 ? t.className.replace('hide', '') : t.className + ' hide';
}

function CreateDir(dir, href, side) {
	if(dir !== undefined) {
		newName = prompt('Введите имя нового каталога', '');

		var outside = GetQueryVariable('action', 0);

		if(newName !== null && newName.length && newName.indexOf('/') == -1) {
			window.location.href = 'action.phtml'+href+'&action=createdir&mdir=' + dir + '&ndir=' + newName + '&side='+side+'&outside='+outside;
		}
	}
}

function RenameFile(dir, file, href, side, outside, fileName) {
	if(dir !== undefined && file != undefined) {
		newName = prompt('Введите новое имя файла', file);

		if(newName !== null && newName.length) {
			window.location.href = 'action.phtml'+href+'&'+side+'dir='+dir+'&action=rename&mdir=' + dir + '&mfile=' + fileName +'&nfile=' + newName+'&side='+side+'&outside='+outside;
		}
	}
}

function RemoveFile(dir, file, href, side, outside, fileName) {
	if(dir !== undefined && file != undefined) {
		if(confirm('Вы действительно хотите удалить '+file)) {
			window.location.href = 'action.phtml'+href+'&'+side+'dir='+dir+'&action=delete&mdir=' + dir + '&mfile=' + fileName +'&side='+side+'&outside='+outside;
		}
	}
}

function EditFile(title, dir, href, side, outside) {
	g = window.open('popup.phtml'+href+'&'+side+'dir='+dir+'&mdir='+dir+'&mfile='+title+'&action=edit&side='+side+'&outside='+outside, '', 'width=760, height=550,toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=auto');
	g.focus();
}
