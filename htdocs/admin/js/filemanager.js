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
	window.clipboardData.setData('Text', url);
	alert('Готово');
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
			window.location.href = 'action.phtml'+href+'&'+side+'dir='+dir+'&action=rename&mdir=' + dir + '&mfile=' + fileName +'&nfile=' + newName+'&side='+side+'&outside='+outside+'&charcheck=йцукен';
		}
	}
}

function RemoveFile(dir, file, href, side, outside, fileName) {
	if(dir !== undefined && file != undefined) {
		if(confirm('Вы действительно хотите удалить '+file)) {
			window.location.href = 'action.phtml'+href+'&'+side+'dir='+dir+'&action=delete&mdir=' + dir + '&mfile=' + fileName +'&side='+side+'&outside='+outside +'&charcheck=йцукен';
		}
	}
}

function EditFile(title, dir, href, side, outside) {
	g = window.open('popup.phtml'+href+'&'+side+'dir='+dir+'&mdir='+dir+'&mfile='+title+'&action=edit&side='+side+'&outside='+outside, '', 'width=760, height=550,toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=auto');
	g.focus();
}

function ContextFile(title, isDir, urlAlias, isWrite, isDelete, isBinary, dir, href, side, isCopy, fileName, isBrokenFilename) {
	var ret;
	var name = GetQueryVariable('name', 0);
	var outside = GetQueryVariable('action', 0);

	ret  = '<strong>'+title+'</strong>';

	if(isDir) {
		ret += '<a class="edit" href="'+href+'&'+side+'dir='+dir+fileName+'&side='+side+'&outside='+outside+'">Зайти в каталог</a>';

	} else {
		if(isWrite != 0 && !isBinary && !isBrokenFilename) {
			ret += '<a class="edit" href="#" onClick="EditFile(\''+title+'\', \''+dir+'\', \''+href+'\', \''+side+'\', \''+outside+'\'); return false;">Изменить</a>';
		} else {
			ret += '<span class="edit">Изменить</span>';
		}

		if(isDelete && isBinary && !isBrokenFilename) {
			ret += '<a class="unzip" href="action.phtml'+href+'&'+side+'dir='+dir+'&action=unzip&mdir='+dir+'&mfile='+fileName+'&side='+side+'&outside='+outside+'">Разархивировать</a>';
		} else {
			ret += '<span class="unzip">Разархивировать</span>';
		}
	}

	if(isCopy && !name && outside !== 'filesarray' && !isBrokenFilename) {
		if(isCopy == 2) {
			confirmJ = ' onclick="return confirm(\'В каталоге назначения уже есть файл/кататог с таким именем. Вы действительно хотите переписать существующий файл/каталог '+title+'?\');"';
		} else {
			confirmJ = '';
		}

		if(isDelete) {
			ret += '<a class="move" '+confirmJ+' href="action.phtml'+href+'&'+side+'dir='+dir+'&action=move&mdir='+dir+'&mfile='+fileName+'&side='+side+'&outside='+outside+'">Переместить</a>';
		} else {
			ret += '<span class="move">Переместить</span>';
		}

		ret += '<a class="copy" '+confirmJ+' href="action.phtml'+href+'&'+side+'dir='+dir+'&action=copy&mdir='+dir+'&mfile='+fileName+'&side='+side+'&outside='+outside+'">Скопировать</a>';
	} else {
		ret += '<span class="move">Переместить</span>';
		ret += '<span class="copy">Скопировать</span>';
	}

	if(isDelete) {
		ret += '<a class="rename" href="#" onClick="RenameFile(\''+dir+'\', \''+title+'\', \''+href+'\', \''+side+'\', \''+outside+'\', \''+fileName+'\'); return false;">Переименовать</a>';
	} else {
		ret += '<span class="rename">Переименовать</span>';
	}

	if(isDelete) {
		ret += '<a class="del" href="#" onClick="RemoveFile(\''+dir+'\', \''+title+'\', \''+href+'\', \''+side+'\', \''+outside+'\', \''+fileName+'\'); return false;">Удалить</a>';
	} else {
		ret += '<span class="del">Удалить</span>';
	}

	ret += '<div class="hr"></div>';

	if(urlAlias && urlAlias != '0') {
		ret += '<a class="alias"  href="'+urlAlias+'" target="_blank">Скачать/Показать</a>';
		ret += '<a class="memory" href="'+urlAlias+'" onClick="return InMemory(\''+urlAlias+'\');">В буфер обмена</a>';
	} else {
		ret += '<span class="alias" >Скачать/Показать</span>';
		ret += '<span class="memory">В буфер обмена</span>';
	}

	return ret;
}
