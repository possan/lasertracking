var osc = require('node-osc');
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var oscServer = new osc.Server(8001, '0.0.0.0');

var allsockets = [];

oscServer.on("message", function (msg, rinfo) {
	console.log("Message:", msg);
	io.emit('message', msg);
});

io.on('connection', function(socket) {
	console.log('a user connected');
	allsockets.push(socket);
	socket.on('disconnect', function() {
		console.log('user disconnected');
	});
});

app.use('/', express.static(__dirname + '/public'));

http.listen(8080, function(){
	console.log('Server up: http://localhost:8080/ ');
});
