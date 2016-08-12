var vtn_terminal = null; // VTN terminal object

var vtn_cli_help = {}; // Help for commands

var term_cfg = {
	prompt: 'VTN > ', 
	greetings: 'ODL VTN Coordinator CLI\n\r-----------------------', 
	outputLimit: 0
};


// IP address of the ODL conroller
var ODL_IP_addr = (getCookie("odl_vtn_coordinator_addr") == null) ? '127.0.0.1' : getCookie("odl_vtn_coordinator_addr");

// Base URI for VTN coodinator RESTfull API
var vtn_coord_base_URI = 'http://' + ODL_IP_addr + ':8080';


// HTTP request custom headers for VTN Coordinators
var vtn_coord_hdr = {
	'content-type': 'application/json',
	'username': 'admin',
	'password': 'adminpass',
	'ipaddr': vtn_coord_base_URI
};

if (ODL_DEMO == false) {
	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/controllers.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		set_serv_state("VTN", true);
	});
}


vtn_cli_help.echo = "Sample echo function: echo [string]";
function vtn_echo(parm1) {
	vtn_terminal.echo(parm1);
}

vtn_cli_help.set_co = "Set VTN Coordinator IP: set_co [IP address]";
function vtn_set_coord(param) {
	ODL_IP_addr = param;
	vtn_coord_base_URI = 'http://' + ODL_IP_addr + ':8080';
	vtn_coord_hdr = {
		'content-type': 'application/json',
		'username': 'admin',
		'password': 'adminpass',
		'ipaddr': vtn_coord_base_URI
	};
	setCookie("odl_vtn_coordinator_addr", param, "Mon, 01-Jan-2100 00:00:00 GMT", "/");

	if (ODL_DEMO == false) {
		$.ajax({
			url: vtn_coord_base_URI + '/vtn-webapi/controllers.json',
			type: 'GET',
			dataType: 'json',
			headers: vtn_coord_hdr
		}).done(function( data ) {
			set_serv_state("VTN", true);
		});
	}
	location.reload();
}

vtn_cli_help.set_demo = "Set demo mode: set_demo [true|false]";
function vtn_set_demo(param) {
    ODL_DEMO = (param == "true");
    setCookie("odl_vtn_coordinator_demo", param, "Mon, 01-Jan-2100 00:00:00 GMT", "/");
    location.reload();
}

vtn_cli_help.lst_ctls = "List controllers: lst_ctls";
function vtn_list_ctls() {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/controllers.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
		set_serv_state("VTN", true);
	}).fail(function() {
		set_serv_state("VTN", false);
	});
}


// curl -v -X POST -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass' -H 'ipaddr:172.20.6.157' -d '{"controller": {"controller_id": "controller_1", "ipaddr":"172.20.6.158", "type": "odc", "version": "1.0", "auditstatus":"enable"}}' http://172.20.6.157:8080/vtn-webapi/controllers.json

vtn_cli_help.crt_ctl = "Create Controller: crt_ctl [controller_id] [IP address]";
function vtn_crt_ctl(param1, param2) {
	var post_data =  JSON.stringify({
		controller: {
			controller_id: param1,
			ipaddr: param2,
			type: 'odc',
			version: '1.0',
			auditstatus: 'enable'
		}
	});
	ODL_ADDR = param2;
	setCookie("odl_addr", param2, "Mon, 01-Jan-2100 00:00:00 GMT", "/");

	if (ODL_DEMO == false) {
		$.ajax({
			url: vtn_coord_base_URI + '/vtn-webapi/controllers.json',
			type: 'POST',
			dataType: 'json',
			headers: vtn_coord_hdr,
			processData: false,
			data: post_data
		}).done(function( data ) {
			vtn_terminal.echo(JSON.stringify(data));
		});
	}
	location.reload();
}

// curl -X DELETE -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass'   -H 'ipaddr:172.20.6.157' http://172.20.6.157:8080/vtn-webapi/controllers/controller_1.json

vtn_cli_help.dlt_ctl = "Delete Controller: dlt_ctl [controller_id]";
function vtn_dlt_ctl(param1) {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/controllers/' + param1 +'.json',
		type: 'DELETE',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
		location.reload();
	});
}

// curl -v -X POST -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass' -H 'ipaddr:172.20.6.157' -d '{"vtn" : {"vtn_name":"vtn1","description":"test VTN" }}' http://172.20.6.157:8080/vtn-webapi/vtns.json

vtn_cli_help.crt_vtn = "Create a VTN: crt_vtn [VTN name] [description]";
function vtn_crt_vtn(param1, param2) {
	var post_data =  JSON.stringify({
		vtn: {
			vtn_name: param1,
			description: param2
		}
	});

	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns.json',
		type: 'POST',
		dataType: 'json',
		headers: vtn_coord_hdr,
		processData: false,
		data: post_data
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -X GET -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass'   -H 'ipaddr:172.20.6.157' http://172.20.6.157:8080/vtn-webapi/vtns.json

vtn_cli_help.lst_vtns = "List VTNs: lst_vtn";
function vtn_list_vtns() {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

//curl -v -X POST -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass' -H 'ipaddr:172.20.6.157' -d '{"vbridge" : {"vbr_name":"vBridge1","controller_id":"controller_1","domain_id":"(DEFAULT)" }}' http://172.20.6.157:8080/vtn-webapi/vtns/vtn1/vbridges.json

vtn_cli_help.crt_vbr = "Create vBridge: crt_vbr [bridge name] [controller ID]";
function vtn_crt_vbr(param1, param2) {
	var post_data =  JSON.stringify({
		vbridge: {
			vbr_name: param1,
			controller_id: param2,
			domain_id: '(DEFAULT)'
		}
	});

	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns/vtn1/vbridges.json',
		type: 'POST',
		dataType: 'json',
		headers: vtn_coord_hdr,
		processData: false,
		data: post_data
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -X GET -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass'   -H 'ipaddr:172.20.6.157' http://172.20.6.157:8080/vtn-webapi/vtns/vtn1/vbridges.json

vtn_cli_help.lst_vbrs = "List vBridges: lst_vbrs";
function vtn_list_vbrs() {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns/vtn1/vbridges.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -v -X POST -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass' -H 'ipaddr:172.20.6.157'  -d '{"interface": {"if_name": "if1","description": "if_desc1"}}' http://172.20.6.157:8080/vtn-webapi/vtns/vtn1/vbridges/vBridge1/interfaces.json


vtn_cli_help.crt_vbr_if = "Create Interface on vBridge: crt_vbr_if [VTN name] [bridge name] [interface name] [interface desccription]";
function vtn_crt_vbr_if(param1, param2, param3, param4) {
	var post_data =  JSON.stringify({
		interface: {
			if_name: param3,
			description: param4
		}
	});

	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns/' + param1 + '/vbridges/' + param2 + '/interfaces.json',
		type: 'POST',
		dataType: 'json',
		headers: vtn_coord_hdr,
		processData: false,
		data: post_data
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -X GET -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass'   -H 'ipaddr:172.20.6.157' http://172.20.6.157:8080/vtn-webapi/vtns/vtn1/vbridges/vBridge1/interfaces.json

vtn_cli_help.lst_vbr_ifs = "List vBridge interfaces: lst_vbr_ifs [VTN name] [bridge name]";
function vtn_list_vbr_ifs(param1, param2) {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns/' + param1 + '/vbridges/' + param2 + '/interfaces.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -X GET -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass'   -H 'ipaddr:172.20.6.157' http://172.20.6.157:8080/vtn-webapi/controllers/controller_1/domains/\(DEFAULT\)/logical_ports.json

vtn_cli_help.lst_lports = "List of logical ports configured: lst_lports [controller_id]";
function vtn_list_lports(param1) {
	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/controllers/' + param1 +'/domains/\(DEFAULT\)/logical_ports.json',
		type: 'GET',
		dataType: 'json',
		headers: vtn_coord_hdr
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
	});
}

// curl -v -X PUT -H 'content-type: application/json' -H 'username: admin' -H 'password: adminpass' -H 'ipaddr:172.20.6.157' -d '{"portmap":{"logical_port_id": "PP-00:00:00:00:00:00:00:03-s3-eth1"}}' http://172.20.6.157:8080/vtn-webapi/vtns/vtn1/vbridges/vBridge1/interfaces/if1/portmap.json

vtn_cli_help.map_vif2lport = "Map vBridge virtual interfac to the controller's logical port: map_vif2lport [VTN name] [bridge name] [interface name] [lport ID]";
function vtn_map_vif2lport(param1, param2, param3, param4) {
	var post_data =  JSON.stringify({
		portmap: {
			logical_port_id: param4
		}
	});

	if (ODL_DEMO == true) return;

	$.ajax({
		url: vtn_coord_base_URI + '/vtn-webapi/vtns/' + param1 + '/vbridges/' + param2 + '/interfaces/' + param3 + '/portmap.json',
		type: 'PUT',
		dataType: 'json',
		headers: vtn_coord_hdr,
		processData: false,
		data: post_data
	}).done(function( data ) {
		vtn_terminal.echo(JSON.stringify(data));
		location.reload();
	});
}



vtn_cli_help.help = "Prints this help: help";
function vtn_help() {
	for (var cmd in vtn_cli_commands) {
		vtn_terminal.echo(cmd + "\t\t- " + vtn_cli_help[cmd]);
	}
}

var vtn_cli_commands = {
	help:       	vtn_help,
	echo:     		vtn_echo,
	set_co:		vtn_set_coord,
	set_demo:	vtn_set_demo,
	lst_ctls: 		vtn_list_ctls,
	crt_ctl:  		vtn_crt_ctl,
	dlt_ctl: 		vtn_dlt_ctl,
	crt_vtn:    	vtn_crt_vtn,
	lst_vtns:		vtn_list_vtns,
	crt_vbr:		vtn_crt_vbr,
	lst_vbrs:		vtn_list_vbrs,
	crt_vbr_if: 	vtn_crt_vbr_if,
	lst_vbr_ifs:	vtn_list_vbr_ifs,
	lst_lports:		vtn_list_lports,
	map_vif2lport:  vtn_map_vif2lport
};

// Init terminal object and attach it to #vtn_term ID
jQuery(document).ready(function($) {
	vtn_terminal = $('#vtn_term').terminal(vtn_cli_commands, term_cfg);
});

