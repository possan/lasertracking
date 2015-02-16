var cheerio = require('cheerio');

function Drawing() {
	this.width = 512;
	this.height = 512;
	this.up = [ 0, 1, 0 ];
	this.duration = 1;
	this.strokes = [];
}

Drawing.prototype.parse = function(xml) {
	var _this = this;
	$ = cheerio.load(xml);

	var env = $('GML > tag > environment');
	this.up[0] = parseFloat($('up > x', env).text());
	this.up[1] = parseFloat($('up > y', env).text());
	this.up[2] = parseFloat($('up > z', env).text());

	var w = $('screenBounds > x', env);
	var h = $('screenBounds > y', env);
	if (w.text() != '' && h.text() != '') {
		this.width = parseFloat(w.text());
		this.height = parseFloat(h.text());
	}

	var globaltime = 0.0;


	var drawings = $('GML > tag > drawing');

	drawings.each(function(k, drawing) {

		var starttime = globaltime;

		var strokes = $('stroke', drawing);
		// console.log('strokes', strokes.length);
		strokes.each(function(i, stroke) {

			var str = {
				r: 255,
				g: 255,
				b: 255,
				width: 1,
				dripping: 0,
				pts: [],
			};

			var tmp = $('color', stroke);
			if (tmp) {
				var r = $('r', tmp);
				var g = $('g', tmp);
				var b = $('b', tmp);
				if (r.text() != '' && g.text() != '' && b.text() != '') {
					str.r = parseFloat(r.text());
					str.g = parseFloat(g.text());
					str.b = parseFloat(b.text());
				} else {
					var rgb = (new RegExp('\#?([a-f0-9]{2})([a-f0-9]{2})([a-f0-9]{2})')).test(tmp.text());
					if (rgb.length > 3) {
						console.log(rgb);
						str.r = rgb[1];
						str.g = rgb[2];
						str.b = rgb[3];
					}
				}
			}

			var tmp = $('dripping', stroke);
			if (tmp.text() != '') {
				str.dripping = parseFloat(tmp.text());
			}

			var tmp = $('stroke_size', stroke);
			if (tmp.text() != '') {
				str.width = parseFloat(tmp.text());
			}

			$('pt', stroke).each(function(j, pt) {

				var ptout = {
					x: 0,
					y: 0,
					z: 0,
					t: 0
				};

				ptout.x = parseFloat($('x', pt).text());
				ptout.y = parseFloat($('y', pt).text());
				ptout.z = parseFloat($('z', pt).text());
				ptout.t = parseFloat($('time', pt).text());

				ptout.dt = ptout.t + starttime;

				if (ptout.t > _this.duration) {
					_this.duration = ptout.t;
				}

				str.pts.push(ptout);
			});

			if (str.pts.length > 0) {
				_this.strokes.push(str);
			}
		});

	});
}

exports.Drawing = Drawing;
