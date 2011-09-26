
/**
 * Pluralizer
 */
Number.prototype.getPluralForm = function () { // ru only
	var n = this, z = arguments;
	var f = (((n%10)==1 && (n%100)!=11 ) ? 0 : ( n%10>1 && n%10<5 && (n%100<10 || n%100>=20) ? 1 : 2 ));
	if (z.length == 0)
		return f;
	var a = (typeof z[0] != 'string') ? z[0] : z;
	var r = a[Math.min(z.length-1,f)];
	if (typeof r != 'string')
		return r;
	return r.replace(/%[ds]/, n);
}

