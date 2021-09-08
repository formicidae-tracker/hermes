//hello
versions=['latest','v0.5.3'];

version='latest';

function changeVersion(newVersion) {
	newURL = window.location.href;
	newURL.replace('/'+version+'/','/'+newVersion+'/');
	if ( newURL != window.location.href ) {
		window.location.href = newURL;
	}
}

$(document).ready(function() {
	let content = '<div class="version"><div class="dropdown"><div class="entry">▸ '+version+'</div><div class="dropdown-content">';

	for (let i in versions) {
		if (versions[i] == version) {
			continue;
		}
		content += '<a onclick="changeVersion(\''+versions[i]+'\')">' + versions[i] + '</a>';
	}
	content += '</div></div></div>';
	versionDiv = $('.version');
	versionDiv.replaceWith(content);
	$('.version .dropdown').css({
		"position":"relative",
		"display": "inline-block",
		"width": "100px",
		//"list-style": "none",
		//"padding":"0px",
		//"margin":"0px"});
	});
	$('.version .dropdown a').css({
		"display":"block",
		//"background":"#990E00",
		//"margin-right":"5px",
		//"width":"100px",
		//"height":"20px",
		//"line-height":"20px",
		//"text-align":"center",
		"color":"hsla(0,0%,100%,.3)",
	});
	$('.version .dropdown-content').css({
		"display":"block",
		"position":"absolute",
		"background":"#2980b9",
		"width": "100px",
		"z-index":"1",
	});
	$('.version .dropdown-content').hide();
	menuIsShown = false;
	entryItem = $('.version .dropdown .entry');
	contentItem = $('.version .dropdown .dropdown-content');
	showMenu = function() {
		entryItem.text('▾ ' + version);
		contentItem.show(300);
		menuIsShown = true;
	};

	hideMenu = function() {
		entryItem.text('▸ ' + version);
		contentItem.hide(300);
		menuIsShown = false;
	};

	$('.version .dropdown .entry').click(function() {
		if ( menuIsShown == true ) {
			hideMenu();
		} else {
			showMenu();
		}
	});

	window.onclick = function(event) {
		if ( !event.target.matches('.entry') ) {
			hideMenu();
		}
	}


});
