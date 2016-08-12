
var ODL_ADDR = (getCookie("odl_addr") == null) ? '127.0.0.1' : getCookie("odl_addr");
var ODL_BASE_URL = "http://" + ODL_ADDR + ":8080/controller/nb/v2/"
var ODL_CNAME = "default/"
var UNAME = 'admin'
var PASSWORD = 'admin'

var ODL_DEMO = (getCookie("odl_vtn_coordinator_demo") == null) ? true : (getCookie("odl_vtn_coordinator_demo") == "true");

var NetNodes = [];
var NetEdges = [];

var DemoNetNodes = [
    {"id":"00:00:00:00:00:00:00:02"},{"id":"00:00:00:00:00:00:00:03"},
    {"id":"00:00:00:00:00:00:00:01"},{"id":"00:00:00:00:00:00:00:04"},
    {"id":"00:00:00:00:00:00:00:07"},{"id":"00:00:00:00:00:00:00:06"},
    {"id":"00:00:00:00:00:00:00:05"},
    {"id":"10.0.0.6"},{"id":"10.0.0.2"},{"id":"10.0.0.5"},
    {"id":"10.0.0.7"},{"id":"10.0.0.1"},{"id":"10.0.0.4"},
    {"id":"10.0.0.3"},{"id":"10.0.0.8"}
];
  
var DemoNetEdges = [
    {"source":1,"target":0},{"source":3,"target":0},
    {"source":6,"target":5},{"source":4,"target":6},
    {"source":6,"target":2},{"source":2,"target":0},
    {"source":5,"target":7},{"source":1,"target":8},
    {"source":5,"target":9},{"source":4,"target":10},
    {"source":1,"target":11},{"source":3,"target":12},
    {"source":3,"target":13},{"source":4,"target":14}
];


if (ODL_DEMO == true)
{
    NetNodes = DemoNetNodes;
    NetEdges = DemoNetEdges;
    document.getElementById('demo').style.display='normal';
}
else
{
    document.getElementById('demo').style.display='none';

    jQuery(document).ready(function($) {

    var topoRequest = $.ajax({
        type: "GET",
        url: ODL_BASE_URL + "topology/" + ODL_CNAME,
        dataType: 'json',
        username: UNAME,
        password: PASSWORD,
        xhrFields: {
            withCredentials: true
        },
        crossDomain: true,
        success: function (msg) {
            console.log( "Topology response: " + JSON.stringify(msg) );

            msg['edgeProperties'].forEach(function(entry) {

                var src = entry['edge']['headNodeConnector']['node']['id'];
                var dst = entry['edge']['tailNodeConnector']['node']['id'];

                for (var i = 0; i < NetEdges.length; i++)
                { 
                    if (NetEdges[i].source == dst && NetEdges[i].target == src) return;
                }
          
                NetEdges.push({"source":src, "target":dst});

            });
        },
        error: function(jqXHR, textStatus) {
            console.error("Topology request failed: " + textStatus );
        }
    });

    var nodeRequest = $.ajax({
        type: "GET",
        url: ODL_BASE_URL + "switchmanager/" + ODL_CNAME + "nodes/",
        dataType: 'json',
        username: UNAME,
        password: PASSWORD,
        xhrFields: {
            withCredentials: true
        },
        crossDomain: true,
        success: function (msg) {
            console.log( "Switch Manager response: " + JSON.stringify(msg) );

            msg['nodeProperties'].forEach(function(entry) {
                NetNodes.push({"id":entry['node']['id']});
            });
        },
        error: function(jqXHR, textStatus) {
            console.error("Switch Manager request failed: " + textStatus );
        }
    });



    function hostRequest() {
        return $.ajax({
            type: "GET",
            url: ODL_BASE_URL + "hosttracker/" + ODL_CNAME + "hosts/active",
            dataType: 'json',
            username: UNAME,
            password: PASSWORD,
            xhrFields: {
               withCredentials: true
            },
            crossDomain: true,
            success: function (msg) {
                console.log( "Host Tracker response: " + JSON.stringify(msg) );

                msg['hostConfig'].forEach(function(entry) {
                    NetNodes.push({"id":entry['networkAddress']});
                    NetEdges.push({"source":entry['nodeId'], "target":entry['networkAddress']});
                });

                for (var n = 0; n < NetNodes.length; n++)
                { 
                    for (var e = 0; e < NetEdges.length; e++)
                    { 
                        if (NetEdges[e].source == NetNodes[n].id) NetEdges[e].source = n;
                        if (NetEdges[e].target == NetNodes[n].id) NetEdges[e].target = n;
                    }
                }

                console.log("Nodes: " + JSON.stringify(NetNodes));
                console.log("Links: " + JSON.stringify(NetEdges));

                UpdateTopology();
            },
            error: function(jqXHR, textStatus) {
                console.error("Host Tracker request failed: " + textStatus );
            }
        });
    };

    $.when(topoRequest, nodeRequest)
        .done(function() { 
            set_serv_state("ODL", true);
            hostRequest();
        });

    }); /* jQuery(document).ready() */

} /* ODL_DEMO */


/*
var hdr = {
	'content-type': 'application/json',
	'username': 'admin',
	'password': 'admin',
};

function hostRequest() {
    return $.ajax({
        type: "GET",
        url: "http://172.20.6.159:8080/admin/cluster?x-page-url=devices",
        dataType: 'json',
	username: UNAME,
	password: PASSWORD,
        headers: hdr,
        xhrFields: {
            withCredentials: true
        },
        crossDomain: true,
        success: function (msg) {
            console.warn( "Raw Cluster Info" + msg );
            console.warn( "JSON Cluster Info: " + JSON.stringify(msg) );
            UpdateTopology();
        },
        error: function(jqXHR, textStatus) {
            console.error("Host Tracker request failed: " + textStatus );
        }
    });
};
*/

function setCookie(name, value, expires, path, domain, secure) {
    document.cookie = name + "=" + escape(value) +
        ((expires) ? "; expires=" + expires : "") +
        ((path) ? "; path=" + path : "") +
        ((domain) ? "; domain=" + domain : "") +
        ((secure) ? "; secure" : "");
        console.log("READ: " + document.cookie);
}

function delCookie(name) {
    setCookie(name, 0, "Thu, 01 Jan 1970 00:00:01 GMT", "/");
}

function getCookie(name) {
	var cookie = " " + document.cookie;
	var search = " " + name + "=";
	var setStr = null;
	var offset = 0;
	var end = 0;

	if (cookie.length > 0) {
		offset = cookie.indexOf(search);
		if (offset != -1) {
			offset += search.length;
			end = cookie.indexOf(";", offset)
			if (end == -1) {
				end = cookie.length;
			}
			setStr = unescape(cookie.substring(offset, end));
		}
	}
	return(setStr);
}


