$(function(/*Enable all multi-selectors*/) {
	$('div.multiselect-all input').livequery(function(){ 
		$(this).removeAttr('disabled').click(function(){
			$(this).parent().nextAll('table.objects').find('td.c input:checkbox, td.c3 input:checkbox').attr('checked', $(this).attr('checked'))
		})
	})
})

function makeCall(method, action, objects){
	$('body').toggleClass("queryInProgress")
	var resultHandler = function(e, notice){
		if(e.callback){
			try {
				window[e.callback](e)
			} catch (err){}
		} else {
			alert(notice[0] + ": " + (e.notice ? e.notice : notice[1]))
		}
	}
	$.ajax({
		type:     method,
		cache:    false,
		dataType: 'json',
		url:      action,
		data:     objects,
		success:  function(data){
			if(data.isok){
				resultHandler(data.isok, ["OK","успешно"])
			} else if(data.iserror){
				resultHandler(data.iserror, ["ОШИБКА","неизвестная ошибка"])
			}
			
			if(!(data.norefresh || data.iserror))
				window.location.reload()
		},
		complete: function(){
			$('body').toggleClass("queryInProgress")
		},
		error: function() {
			alert("Oбработчик мультиопераций masschange не описан или произошла неизвестная ошибка");
		}
	});
}

$(function(/*Common multiple bottom controls*/) {
	$('div.multiple-bottom-controls a').livequery(function(){
		$(this).click(function(){
			var _doWhat      = $(this).attr('href') ? $(this).attr('href').replace('#', '') : null
			var _options     = $(this).attr('options')
			var _withHelper  = $(this).attr('helper')
			var _rearm       = function(_self){
				if(_withHelper){
					$(_self).removeAttr('options')
				}
				return false
			}
			var _confirm     = $(this).attr('confirm')
			var objectsTable = $(this).closest('div.table_wrapper')
			var obType       = objectsTable.children('div[dtype]').attr('dtype')
			
			var obArray      = []
			
			objectsTable.find('input.multiselect[checked]').each(function(){
				if($(this).attr('oid')) obArray.push($(this).attr('oid'))
			})
			
			if(!(_doWhat && obType && obArray.length && (_withHelper != 'true' || _options || EIactivateContextHelper(this)) && (!_confirm || confirm(_confirm)))) return _rearm(this)
			
			makeCall('post', 'action.phtml', {area: 'masschange', action: null, handler: _doWhat, options: _options, format: obType, 'objects[]': obArray})
			return _rearm(this)
		})
	})
})

$(function(/* Common helpers hide when click somewhere on document */){
	$('div.helper').click(function(e){e.stopPropagation()})
	$(document).click(function(e){
		$('div.helper').hide().parent().removeClass('helper-bubbled')
	})
})

function EIactivateContextHelper(caller){
	if($('div.helper').is(':visible')) {
		return false;
	}
	$(caller).next('div.helper').not('[activated]').find('[assign]').map(function(){
		switch(true) {
			case ($(this).attr('assign_on') && $(this).attr('assign_on').length > 0):
				var callee = $(this).attr('assign_on');
				break;
			case $(this).is('select'):
				var callee = 'change';
				break;
			default:
				var callee = 'click';
		}
		
		$(this).bind(callee, function(){
			eval("var options = " + $(this).attr('assign'))
			$(caller).attr('options', $.toJSON(options)).next('div.helper').hide().end().click().parent().toggleClass('helper-bubbled')
			if(this.form || (this.nodeName == 'FORM' && (this.form = this))){
				this.form.reset()
			}
			return false
		})
		return this
	}).end().end().attr('activated', true).andSelf().show().parent().toggleClass('helper-bubbled')
	
	return false;
}