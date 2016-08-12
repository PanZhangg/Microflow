// Zoom Behavior:
//   https://github.com/mbostock/d3/wiki/Zoom-Behavior

var SERV_IMG = "icons/server-ok.png";
var SERV_OFF_IMG = "icons/server-fail.png";
var SERV_SIZE = 54;

var serv_width = $("#server").width(),
    serv_height = $("#server").height();

window.onresize = function() { 
    serv_width = $("#server").width(),
    serv_height = $("#server").height();
    serv_layout = serv_layout.size([serv_width, serv_height]);
    serv_svg.attr("width", serv_width).attr("height", serv_height);
    serv_node_g.attr("transform", function(d, i) {
          return "translate(" + 30 + "," + (serv_height-80-75*(servers.length-i-1)) + ")";
    });
    serv_layout.resume();
    topoResize();
};

var serv_svg = d3.select("#server").append("svg:svg")
    .attr("id", "serv_svg")
    .attr("width", serv_width)
    .attr("height", serv_height);

var serv_node_g = serv_svg.selectAll(".node");

var serv_layout = d3.layout.force().size([serv_width, serv_height]);

var servers = [
{ 
  fixed: true,
  image: SERV_OFF_IMG,
  text: ["ODL Controller", ODL_ADDR]
},
{ 
  fixed: true,
  image: SERV_OFF_IMG,
  text: ["VTN Coordinator", ODL_IP_addr]
},
];

var serv_nodes = d3.range(servers.length).map(function(i) { return servers[i]; });

serv_layout.nodes(serv_nodes).start();

serv_node_g = serv_node_g.data(serv_nodes)
                .enter().append("g")
                .attr("class", "node")
                .attr("transform", function(d, i) {
                    return "translate(" + 30 + "," + (serv_height-80-75*(servers.length-i-1)) + ")";
                })
                .on("mouseover", serv_mouseover)
                .on("mouseout", serv_mouseout);
 
serv_node_g.append("image")
        .attr("xlink:href", function(d) { return d.image; })
        .attr("x", 0)
        .attr("y", 0)
        .attr("width", SERV_SIZE)
        .attr("height", SERV_SIZE);

serv_node_g.each(
    function(d){
        for (i = 0; i < d.text.length; i++)
            d3.select(this).append("text")
                .attr("dx", SERV_SIZE + 8)
                .attr("dy", (3.6-1.4*(d.text.length-i-1) + "em"))
                .text(d.text[i]);
    });

function set_serv_state(name, on) {
    serv_node_g.each(function(d) { 
        if (d.text[0].indexOf(name) >= 0) {
            d3.select(this).select("image")
                .attr("xlink:href", (on == true) ? SERV_IMG : SERV_OFF_IMG);
        }
    });
}

function toggle_serv_state(name) {
    set_serv_state(name, (d3.select(this).select("image").attr("xlink:href") == SERV_OFF_IMG));
}
    
function serv_mouseover(d) {
    var routerWidth = ROUTER_SIZE * RESIZE_INDEX;
    var routerX = ROUTER_X - (routerWidth - ROUTER_SIZE) / 2;

    node.each(function(d) { 
        if (d.id.indexOf(":") != -1) {
            d3.select(this).select("image").attr("width", routerWidth).attr("x", routerX);
        }
    });
}

function serv_mouseout(d) {
    node.select("image")
        .attr('width', function(d) { return (d.id.indexOf(":") == -1) ? HOST_SIZE : ROUTER_SIZE; }).attr('x', ROUTER_X);
}

//var timerId = setInterval(toggle_serv_state, 500, "VTN")


