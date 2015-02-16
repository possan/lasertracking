//
// GML Playback
//

if (process.argv.length < 4) {
	console.log('Syntax: node play.js [playback speed multiplier] [file.gml] [file.gml] ...');
	process.exit(1);
}

var GML = require('./GML.js');
var fs = require('fs');
var LaserOSC = require('./LaserOSC');

var speed = parseFloat(process.argv[2]);

var osc = require('node-osc');

var client = new osc.Client('127.0.0.1', 8000);
var client2 = new osc.Client('127.0.0.1', 8001);
// client.send('/oscAddress', 200);

var output = new LaserOSC.LaserOSC(client);
var queue = [];

var firstfile = true;

for(var i=3; i<process.argv.length; i++) {
	var fn = process.argv[i];
	console.log('Playing back ' + fn + ' at ' + (100.0 * speed) + '% speed...');

	console.log('Loading...');

	if (!firstfile) {
		queue.push({ c: 'end', d: 2000 / speed });
	}
	firstfile = false;

	queue.push({
		c: 'clear',
		d: 1,
	});

	var g = new GML.Drawing();
	g.parse(fs.readFileSync(fn, 'UTF-8'));

	var timemultiplier = (g.duration > 30.0 ? 1.0 : 1000.0) / speed;

	var lasttime = 0;
	g.strokes.forEach(function(s) {
		console.log('Playing back stroke with ' + s.pts.length + ' points...');
		// output.beginStroke();
		// output.setColor(s.r, s.g, s.b);
		queue.push({ c: 'begin', d: 0 });
		queue.push({ c: 'color', a: [ s.r, s.g, s.b ], d: 0 });
		queue.push({ c: 'drip', a: [ s.dripping ], d: 0 });
		queue.push({ c: 'size', a: [ s.width ], d: 0 });
		s.pts.forEach(function(pt) {
			// output.move(pt.x, pt.y);
			// delay some
			var dt = pt.t - lasttime;
			dt = Math.round(Math.max(0, dt) * timemultiplier);
			// console.log('delay after', dt);
			lasttime = pt.t;
			queue.push({ c: 'move', a: [ pt.x * g.width / g.width, pt.y * g.height / g.width ], d: dt });

		});
		// output.endStroke();
		queue.push({ c: 'end', d: 50 / speed });
	});
}

queue.push({ c: 'clear', d: 5 });

console.log('Queue is ' + queue.length + ' long.');

// output.endStroke();

function tick() {
	if (queue.length == 0) {
		console.log('Queue empty, Exiting...');
		process.exit(0);
		return;
	}

	var cmd = queue.splice(0, 1)[0];
	console.log('Handle command', cmd);

	switch (cmd.c) {
		case 'clear':
			client.send('/draw/clear');
			client2.send('/draw/clear');
			break;
		case 'begin':
			client.send('/draw/pen/down');
			client2.send('/draw/pen/down');
			break;
		case 'end':
			client.send('/draw/pen/up');
			client2.send('/draw/pen/up');
			break;
		case 'move':
			client.send('/draw/pen/move', cmd.a[0], cmd.a[1]);
			client2.send('/draw/pen/move', cmd.a[0], cmd.a[1]);
			break;
		case 'color':
			client.send('/draw/set/color', cmd.a[0], cmd.a[1], cmd.a[2]);
			client2.send('/draw/set/color', cmd.a[0], cmd.a[1], cmd.a[2]);
			break;
		case 'width':
			client.send('/draw/set/size', cmd.a[0]);
			client2.send('/draw/set/size', cmd.a[0]);
			break;
		case 'dripping':
			client.send('/draw/set/drip', cmd.a[0]);
			client2.send('/draw/set/drip', cmd.a[0]);
			break;
	}

	queueTick(cmd.d || 1);
}

function queueTick(delta) {
	setTimeout(tick, delta);
}

queueTick(1);
