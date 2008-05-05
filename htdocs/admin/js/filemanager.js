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
	alert('������');
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
	t.style.display = (t.currentStyle.display == 'none') ? 'block' : 'none';
}

function CreateDir(dir, href, side) {
	if(dir !== undefined) {
		newName = prompt('������� ��� ������ ��������', '');

		var outside = GetQueryVariable('action', 0);

		if(newName !== null && newName.length && newName.indexOf('/') == -1) {
			window.location.href = 'action.phtml'+href+'&action=createdir&mdir=' + dir + '&ndir=' + newName + '&side='+side+'&outside='+outside;
		}
	}
}

function RenameFile(dir, file, href, side, outside) {
	if(dir !== undefined && file != undefined) {
		newName = prompt('������� ����� ��� �����', file);

		if(newName !== null && newName.length) {
			window.location.href = 'action.phtml'+href+'&'+side+'dir='+dir+'&action=rename&mdir=' + dir + '&mfile=' + file +'&nfile=' + newName+'&side='+side+'&outside='+outside;
		}
	}
}

function EditFile(title, dir, href, side, outside) {
	g = window.open('popup.phtml'+href+'&'+side+'dir='+dir+'&mdir='+dir+'&mfile='+title+'&action=edit&side='+side+'&outside='+outside, '', 'width=760, height=550,toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=auto');
	g.focus();
}

function ContextFile(title, isDir, urlAlias, isWrite, isDelete, isBinary, dir, href, side, isCopy) {
	var ret;
	var name = GetQueryVariable('name', 0);
	var outside = GetQueryVariable('action', 0);

	ret  = '<strong>'+title+'</strong>';

	if(isDir) {
		ret += '<a class="edit" href="'+href+'&'+side+'dir='+dir+title+'&side='+side+'&outside='+outside+'">����� � �������</a>';

	} else {
		if(isWrite != 0 && !isBinary) {
			ret += '<a class="edit" href="javascript: EditFile(\''+title+'\', \''+dir+'\', \''+href+'\', \''+side+'\', \''+outside+'\')">��������</a>';
		} else {
			ret += '<span class="edit">��������</span>';
		}

		if(isDelete && isBinary) {
			ret += '<a class="unzip" href="action.phtml'+href+'&'+side+'dir='+dir+'&action=unzip&mdir='+dir+'&mfile='+title+'&side='+side+'&outside='+outside+'">���������������</a>';
		} else {
			ret += '<span class="unzip">���������������</span>';
		}
	}

	if(isCopy && !name && outside !== 'filesarray') {
		if(isCopy == 1) {
			confirmJ = ' onclick="javascript: return confirm(\'� �������� ���������� ��� ���� ����/������� � ����� ������. �� ������������� ������ ���������� ������������ ����/������� '+title+'?\');"';
		} else {
			confirmJ = '';
		}

		if(isDelete) {
			ret += '<a class="move" '+confirmJ+' href="action.phtml'+href+'&'+side+'dir='+dir+'&action=move&mdir='+dir+'&mfile='+title+'&side='+side+'&outside='+outside+'">�����������</a>';
		} else {
			ret += '<span class="move">�����������</span>';
		}

		ret += '<a class="copy" '+confirm+' href="action.phtml'+href+'&'+side+'dir='+dir+'&action=copy&mdir='+dir+'&mfile='+title+'&side='+side+'&outside='+outside+'">�����������</a>';
	} else {
		ret += '<span class="move">�����������</span>';
		ret += '<span class="copy">�����������</span>';
	}

	if(isDelete) {
		ret += '<a class="rename" href="javascript: RenameFile(\''+dir+'\', \''+title+'\', \''+href+'\', \''+side+'\', \''+outside+'\');">�������������</a>';
	} else {
		ret += '<span class="rename">�������������</span>';
	}

	if(isDelete) {
		ret += '<a class="del" href="action.phtml'+href+'&'+side+'dir='+dir+'&action=delete&mdir='+dir+'&mfile='+title+'&side='+side+'&outside='+outside+'" onClick="javascript: return confirm(\'�� ������������� ������ ������� '+title+'\');">�������</a>';
	} else {
		ret += '<span class="del">�������</span>';
	}

	ret += '<div class="hr"></div>';

	if(urlAlias && urlAlias != '0') {
		ret += '<a class="alias"  href="'+urlAlias+'" target="_blank">�������/��������</a>';
		ret += '<a class="memory" href="'+urlAlias+'" onClick="return InMemory(\''+urlAlias+'\');">� ����� ������</a>';
	} else {
		ret += '<span class="alias" >�������/��������</span>';
		ret += '<span class="memory">� ����� ������</span>';
	}

	return ret;
}
