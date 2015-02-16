function LaserOSC() {
}

LaserOSC.prototype.beginStroke = function() {
	console.log('LaserOSC: Pen down.');
}

LaserOSC.prototype.setColor = function(r, g, b) {
	console.log('LaserOSC: Pen color:', r, g, b);
}

LaserOSC.prototype.move = function(x, y) {
	console.log('LaserOSC: Pen move:', x, y);
}

LaserOSC.prototype.endStroke = function() {
	console.log('LaserOSC: Pen up.');
}

LaserOSC.prototype.clear = function() {
	console.log('LaserOSC: Clear.');
}

exports.LaserOSC = LaserOSC;
