function selectdiv(color)
{
	var targetBox = $("." + color);
	$(".box").not(targetBox).hide();
	$(targetBox).show();
}	

function selectOption(show)
{
	showsel=document.getElementById("showsel");
	for (i=0;i<showsel.options.length;i++)
	{
		if (showsel.options[i].value == show)
		{
			showsel.selectedIndex = i;
			break;
		}
	}
}

function postAndRedirect(url, postData)
{
    var postFormStr = "<form method='POST' action='" + url + "'>\n";

    for (var key in postData)
    {
        if (postData.hasOwnProperty(key))
        {
            postFormStr += "<input type='hidden' name='" + key + "' value='" + postData[key] + "'></input>";
        }
    }

    postFormStr += "</form>";

    var formElement = $(postFormStr);

    $('body').append(formElement);
    $(formElement).submit();
}

function newShow(showNr)
{
	
	const data={
		"show":showNr,
	}
	postAndRedirect('/',data);

}

function includeHTML(isStatic) 
{
	var z, i, elmnt, file, xhttp;
	if (isStatic=="no")
	{
		return;
	}
	/* Loop through a collection of all HTML elements: */
	z = document.getElementsByTagName("*");
	for (i = 0; i < z.length; i++) 
	{
		elmnt = z[i];
		/*search for elements with a certain atrribute:*/
		file = elmnt.getAttribute("w3-include-html");
		if (file) {
		/* Make an HTTP request using the attribute value as the file name: */
		xhttp = new XMLHttpRequest();
		xhttp.onreadystatechange = function() 
		{
			if (this.readyState == 4) 
			{
			if (this.status == 200) {elmnt.innerHTML = this.responseText;}
			if (this.status == 404) {elmnt.innerHTML = "Page not found.";}
			/* Remove the attribute, and call this function once more: */
			elmnt.removeAttribute("w3-include-html");
			includeHTML();
			}
		}
		xhttp.open("GET", file, true);
		xhttp.send();
		/* Exit the function: */
		return;
		}
	}
}

function toggleField(hideObj,showObj)
{
	hideObj.disabled=true;		
	hideObj.style.display='none';
	showObj.disabled=false;	
	showObj.style.display='inline';
	showObj.focus();
}

function presetChanged()
{
	sel=$('#presetselect')[0];
	pres=$('#presetinput')[0];
	sav=$('#presetsave')[0];
	if (sel.options[sel.selectedIndex].value=='customOption')
	{
		sel.disabled=true;
		sel.style.display='none';
		pres.disabled=false;
		pres.style.display='inline';
		sav.disabled=false;
		sav.style.display='inline'
		pres.focus();
		sel.selectedIndex='0';
	}
}

function presetBlur()
{	
	sel=$('#presetselect')[0];
	pres=$('#presetinput')[0];
	sav=$('#presetsave')[0];
	if (pres.value=='')
	{
		sel.disabled=false;
		sel.style.display='inline';
		pres.disabled=true;
		pres.style.display='none';
		sav.disabled=true;
		sav.style.display='none';

	}
}



/*
$(document).ready(function(){
    $('option').click(function(){
        var inputValue = $(this).attr("value");
				selectdiv(inputValue);
    });
});
*/

/*
const queryString = window.location.search;
const urlParams = new URLSearchParams(queryString);
if (urlParams.has('color'))
{
	const color = urlParams.get('color');
	selectdiv(color);
	colorsel=document.getElementById("colorsel");
	for (i=0;i<colorsel.options.length;i++)
	{
		if (colorsel.options[i].value == color)
		{
			colorsel.selectedIndex = i;
			break;
		}
	}	
}
*/
