
// Multi-Foci Force Layout:
//   http://bl.ocks.org/mbostock/1249681

// D3.js Drag and Drop, Zoomable, Panning, Collapsible Tree with auto-sizing:
//   http://www.robschmuecker.com/d3-js-drag-and-drop-zoomable-panning-collapsible-tree-with-auto-sizing/

// Labeled Force Layout:
//   http://bl.ocks.org/mbostock/950642

// Multi-Foci Force Layout:
//   http://bl.ocks.org/mbostock/1021953

var HOST_SIZE = 40;
var HOST_X = -8;
var HOST_Y = -8;
var HOST_IMG = "icons/host.svg";

var ROUTER_SIZE = 48;
var ROUTER_X = -8;
var ROUTER_Y = -8;
var ROUTER_IMG = "icons/router.png";

var RESIZE_INDEX = 1.08;


var width = $("#graph-diagram").width(),
    height = $("#graph-diagram").height();

function topoResize() { 
    width = $("#graph-diagram").width(),
    height = $("#graph-diagram").height();
    topo = topo.size([width, height]);
    svg.attr("width", width).attr("height", height);
    topo.resume();
};

d3.select("body")
    .on("keydown", function() {
        if (d3.event.ctrlKey && (d3.event.keyCode == 49 /* 49dec - 1 */))
        {
            d3.event.preventDefault();
            /* Freeze topology */
            svg.selectAll("g.node").classed("fixed", true);
            svg.selectAll("g.node")
                .each(function(d) { 
                    d.fixed = true; 
                    setCookie("odl:" + d.id + ":x", d.x, "Mon, 01-Jan-2100 00:00:00 GMT", "/");
                    setCookie("odl:" + d.id + ":y", d.y, "Mon, 01-Jan-2100 00:00:00 GMT", "/");
                });
        }
        else if (d3.event.ctrlKey && (d3.event.keyCode == 50 /* 50dec - 2 */))
        {
            d3.event.preventDefault();
            /* Unfreeze topology */
            svg.selectAll("g.node").classed("fixed", false);
            svg.selectAll("g.node")
                .each(function(d) { 
                    if (d.fixed == true) {                    
                        d.fixed = false;
                        delCookie("odl:" + d.id + ":x");
                        delCookie("odl:" + d.id + ":y");
                    }
                });
                
            topo.resume();
        }
    });


var svg = d3.select("#graph-diagram").append("svg:svg")
    .attr("id", "topo_svg")
    .attr("width", width)
    .attr("height", height);

var link = svg.selectAll(".link"),
    node = svg.selectAll(".node");

var topo = d3.layout.force()
    .size([width, height])
    .charge(-400)
    .linkDistance(100)
    .gravity(0.1)
    .on("tick", tick);

function tick() {
    link.attr("x1", function(d) { return d.source.x; })
        .attr("y1", function(d) { return d.source.y; })
        .attr("x2", function(d) { return d.target.x; })
        .attr("y2", function(d) { return d.target.y; });

    node.attr("transform", function(d) {
          return "translate(" + (d.x - 18) + "," + (d.y - 18) + ")";
    });
}

var drag = topo.drag().on("dragend", dragend);


function UpdateTopology() {
//d3.json("graph.json", function(error, graph) {

    NetNodes.forEach(function(d, i) {
        if (getCookie("odl:" + d.id + ":x") != null) {
            d.fixed = true;
            d.x = parseFloat(getCookie("odl:" + d.id + ":x"));
            d.y = parseFloat(getCookie("odl:" + d.id + ":y"));
        }
    });

    topo.nodes(NetNodes)
        .links(NetEdges)
        .start();
      
    link = link.data(NetEdges)
        .enter().append("line")
        .attr("class", "link");

    node = node.data(NetNodes)
        .enter().append("g")
        .attr("class", function(d) { return (getCookie("odl:" + d.id + ":x") == null) ? "node" : "node fixed"; })
        .on("dblclick", dblclick)
        .on("mouseover", mouseover)
        .on("mouseout", mouseout)
        .call(drag);

    node.append("image")
        .attr("xlink:href", function(d) { return (d.id.indexOf(":") == -1) ? HOST_IMG : ROUTER_IMG;})
        .attr("x", ROUTER_X)
        .attr("y", ROUTER_Y)
        .attr("width", function(d) { return (d.id.indexOf(":") == -1) ? HOST_SIZE : ROUTER_SIZE;})
        .attr("height", function(d) { return (d.id.indexOf(":") == -1) ? HOST_SIZE : ROUTER_SIZE;});

    node.append("text")
        .attr("dx", function(d) { return (d.id.indexOf(":") == -1) ? -10 : -40;})
        .attr("dy", "3.2em")
        .text(function(d) { return d.id; });
}
//);

if (ODL_DEMO == true) {
    UpdateTopology();
}

function dblclick(d) {
    d3.select(this).classed("fixed", d.fixed = false);
    delCookie("odl:" + d.id + ":x");
    delCookie("odl:" + d.id + ":y");
    topo.resume();
}

function mouseover(d) {

    if (d.id.indexOf(":") == -1)
    {
        var hostWidth = HOST_SIZE * RESIZE_INDEX;
        var hostX = HOST_X - (hostWidth - HOST_SIZE) / 2;

        node.each(function(d) { 
            if (d.id.indexOf(":") == -1) {
                d3.select(this).select("image").attr("width", hostWidth).attr("x", hostX);
            }
        });
    }
    else
    {
        var routerWidth = ROUTER_SIZE * RESIZE_INDEX;
        var routerX = ROUTER_X - (routerWidth - ROUTER_SIZE) / 2;

        node.each(function(d) { 
            if (d.id.indexOf(":") != -1) {
                d3.select(this).select("image").attr("width", routerWidth).attr("x", routerX);
            }
        });
    }
}

function mouseout(d) {
    node.select("image")
        .attr('width', function(d) { return (d.id.indexOf(":") == -1) ? HOST_SIZE : ROUTER_SIZE; }).attr('x', ROUTER_X);
}

function dragend(d) {
    d3.select(this).classed("fixed", d.fixed = d3.event.sourceEvent.ctrlKey || d.fixed);

    if (d.fixed) {
        setCookie("odl:" + d.id + ":x", d.x, "Mon, 01-Jan-2100 00:00:00 GMT", "/");
        setCookie("odl:" + d.id + ":y", d.y, "Mon, 01-Jan-2100 00:00:00 GMT", "/");
    }
}


/* EOF */
