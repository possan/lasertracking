(function() {


	var Stroke = function() {
		this.path = [];
		this.ipath = [];
		this.color = [ 255, 255, 255 ];
		this.width = 1;
		this.drip = 0;
		this.vertices = [];
		this.dirty = true;
		this.active = true;
	}

	Stroke.prototype.begin = function() {
		this.path = [];
		this.dirty = true;
	}

	Stroke.prototype.setColor = function(r, g, b) {
		this.color = [ r, g, b ];
	}

	Stroke.prototype.setWidth = function(w) {
		this.width = w;
	}

	Stroke.prototype.setDrip = function(d) {
		this.drip = d;
	}

	Stroke.prototype.end = function() {
		this.dirty = true;
		this.active = false;
	}

	Stroke.prototype.move = function(x, y) {

		if (this.path.length > 0) {
			var lx = this.path[this.path.length - 1].x;
			var ly = this.path[this.path.length - 1].y;
			var dx = x - lx;
			var dy = y - ly;
			var dl = Math.sqrt(dx*dx + dy*dy);
			if (dl < 0.01)
				return;
		}

		this.path.push({
			x: x,
			y: y,
			r: this.color[0],
			g: this.color[1],
			b: this.color[2],
			w: this.width,
			d: this.drip
		});

		this.dirty = true;
		// console.log('move', x, y);
	}

/*
   Tension: 1 is high, 0 normal, -1 is low
   Bias: 0 is even,
         positive is towards first segment,
         negative towards the other
*/
function HermiteInterpolate(
   y0, y1,
    y2, y3,
    mu,
    tension,
    bias)
{
   var m0,m1,mu2,mu3;
   var a0,a1,a2,a3;

	mu2 = mu * mu;
	mu3 = mu2 * mu;
   m0  = (y1-y0)*(1+bias)*(1-tension)/2;
   m0 += (y2-y1)*(1-bias)*(1-tension)/2;
   m1  = (y2-y1)*(1+bias)*(1-tension)/2;
   m1 += (y3-y2)*(1-bias)*(1-tension)/2;
   a0 =  2*mu3 - 3*mu2 + 1;
   a1 =    mu3 - 2*mu2 + mu;
   a2 =    mu3 -   mu2;
   a3 = -2*mu3 + 3*mu2;

   return(a0*y1+a1*m0+a2*m1+a3*y2);
}


	Stroke.prototype.prepare = function(drawing) {
		if (!this.dirty)
			return;

		this.dirty = false;
		this.vertices = [];

		// console.log('update vertexbuffer with ' + this.path.length + ' path steps.');

		// calculate forward tangents
		if (this.path.length < 2)
			return;

		for(var i=0; i<this.path.length - 1; i++) {
			var p0 = this.path[i];
			var p1 = this.path[i + 1];

			p0.sx = (p0.x - 0.5) * 2.0;
			p0.sy = (0.5 - p0.y) * 2.0;

			p1.sx = (p1.x - 0.5) * 2.0;
			p1.sy = (0.5 - p1.y) * 2.0;

			var dx = p1.sx - p0.sx;
			var dy = p1.sy - p0.sy;
			var dl = Math.sqrt(dx * dx + dy * dy);
			dx /= dl;
			dy /= dl;

			p0.dd = Math.max(0.1, 1.0 - 2.0 * dl);
			p0.tx = dx;
			p0.ty = dy;
		}

		this.path[this.path.length-1].tx = this.path[this.path.length-2].tx;
		this.path[this.path.length-1].ty = this.path[this.path.length-2].ty;

		// interpolate path

		this.ipath = [];
		for(var i=1; i<this.path.length - 3; i++) {
			var p0 = this.path[i - 1];
			var p1 = this.path[i];
			var p2 = this.path[i + 1];
			var p3 = this.path[i + 2];

			for(var mu = 0; mu < 1.0; mu += 0.2) {
				var T = 0.15;
				var B = 0;

				var tx = HermiteInterpolate(p0.tx, p1.tx, p2.tx, p3.tx, mu, T, B);
				var ty = HermiteInterpolate(p0.ty, p1.ty, p2.ty, p3.ty, mu, T, B);

				var tl = Math.sqrt(tx * tx + ty * ty);
				tx /= tl;
				ty /= tl;

				var ipt = {
					sx: HermiteInterpolate(p0.sx, p1.sx, p2.sx, p3.sx, mu, T, B),
					sy: HermiteInterpolate(p0.sy, p1.sy, p2.sy, p3.sy, mu, T, B),
					w: HermiteInterpolate(p0.w, p1.w, p2.w, p3.w, mu, T, B),
					r: HermiteInterpolate(p0.r, p1.r, p2.r, p3.r, mu, T, B),
					g: HermiteInterpolate(p0.g, p1.g, p2.g, p3.g, mu, T, B),
					b: HermiteInterpolate(p0.b, p1.b, p2.b, p3.b, mu, T, B),
					d: HermiteInterpolate(p0.d, p1.d, p2.d, p3.d, mu, T, B),
					dd: HermiteInterpolate(p0.dd, p1.dd, p2.dd, p3.dd, mu, T, B),
					tx: tx,
					ty: ty,
				};

				this.ipath.push(ipt);
			}
		}

		// render interpolated path

		if (this.ipath.length > 2) {
			for(var i=0; i<this.ipath.length - 1; i++) {
				var p0 = this.ipath[i];
				var p1 = this.ipath[i + 1];

				var R0 = 0.025 * p0.w * p0.dd;
				var R1 = 0.025 * p1.w * p1.dd;

				var mul = Math.min(
					Math.min( 1.0, i * 0.33 ),
					Math.min( 1.0, (this.ipath.length - i) * 0.33 )
				);
					//   1.0 Math.sin(i * 3.14259 / this.ipath.length);
				R0 *= mul;
				R1 *= mul;

				//   p1
				// A---B  .
				// |  /| /|\
				// | / |  |
				// |/  |  |
				// C---D fwd
				//   p0

				var lx0 = -p0.ty;
				var ly0 = p0.tx;

				var lx1 = -p1.ty;
				var ly1 = p1.tx;

				var ax = p1.sx - R1 * lx1;
				var ay = p1.sy - R1 * ly1;

				var bx = p1.sx + R1 * lx1;
				var by = p1.sy + R1 * ly1;

				var cx = p0.sx - R0 * lx0;
				var cy = p0.sy - R0 * ly0;

				var dx = p0.sx + R0 * lx0;
				var dy = p0.sy + R0 * ly0;


				this.vertices = this.vertices.concat([
					// 1-2
					// |/
					// 3
					ax, ay, bx, by, cx, cy,

					//   1
					//  /|
					// 3-2
					bx, by, dx, dy, cx, cy
				]);
			}
		}

	}






	var Drawing = function() {
		this.strokes = [];
		this.color = [ 255, 255, 255 ];
		this.drip = 0;
		this.width = 1;
		// this.width = 1;
		// this.height = 1;
		this.laststroke = null;
	}

	Drawing.prototype.clear = function() {
		// console.log('Drawing::clear');
		this.strokes = [];
		this.laststroke = null;
	}

	Drawing.prototype.begin = function() {
		if (this.laststroke == null) {
			this.laststroke = new Stroke();
			this.laststroke.begin();
			this.strokes.push(this.laststroke);
		}
	}

	Drawing.prototype.setColor = function(r, g, b) {
		if (this.laststroke == null)
			return;
		this.color = [ r, g, b ];
	}

	Drawing.prototype.setWidth = function(w) {
		if (this.laststroke == null)
			return;
		this.width = w;
	}

	Drawing.prototype.setDrip = function(d) {
		if (this.laststroke == null)
			return;
		this.drip = d;
	}

	Drawing.prototype.end = function() {
		// console.log('Drawing::end');
		if (this.laststroke != null) {
			this.laststroke.end();
			this.laststroke = null;
		}
	}

	Drawing.prototype.move = function(x, y) {
		if (this.laststroke == null)
			return;

		this.laststroke.setColor(this.color[0], this.color[1], this.color[2]);
		this.laststroke.setWidth(this.width);
		this.laststroke.setDrip(this.drip);
		this.laststroke.move(x, y);
	}




























	var Renderer = function() {
		this.drawing = null;
		this.width = 100;
		this.height = 100;
		this.time = 0;
	}

	Renderer.prototype.renderFrame = function() {
		var _this = this;

		// console.log('Renderer::renderFrame ' + this.drawing.strokes.length + ' strokes.');
		this.drawing.strokes.forEach(function(s) {
			s.prepare(this.drawing);
		});

		// draw all strokes to renderbuffer

		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.rttFramebuffer);

		this.gl.viewport(0, 0, this.rttFramebuffer.width, this.rttFramebuffer.height);
		this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

		_this.gl.useProgram(_this.program);
		_this.gl.uniform2f(_this.resolutionLocation, _this.rttFramebuffer.width, _this.rttFramebuffer.height);
		_this.gl.bindTexture(_this.gl.TEXTURE_2D, null);

		this.drawing.strokes.forEach(function(s) {

			if (s.vertices.length > 0) {

				_this.gl.bindBuffer(_this.gl.ARRAY_BUFFER, _this.buffer);

				_this.gl.bufferData(_this.gl.ARRAY_BUFFER, new Float32Array(s.vertices), _this.gl.STATIC_DRAW);

				_this.gl.bindBuffer(_this.gl.ARRAY_BUFFER, _this.buffer);
				_this.gl.enableVertexAttribArray(_this.positionLocation);
				_this.gl.vertexAttribPointer(_this.positionLocation, 2, _this.gl.FLOAT, false, 0, 0);

				_this.gl.enableVertexAttribArray(_this.positionLocation);

				_this.gl.vertexAttribPointer(_this.positionLocation, 2, _this.gl.FLOAT, false, 0, 0);

				_this.gl.uniform1f(_this.timeLocation, _this.time);

				_this.gl.drawArrays(_this.gl.TRIANGLES, 0, s.vertices.length / 2);

				// s.prepare();
			}
		});

		this.gl.bindTexture(this.gl.TEXTURE_2D, this.rttTexture);
		this.gl.generateMipmap(this.gl.TEXTURE_2D);
		this.gl.bindTexture(this.gl.TEXTURE_2D, null);


		// render screen
		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);

		this.gl.viewport(0, 0, this.width, this.height);
		this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

		this.gl.useProgram(this.program2);
		this.gl.uniform2f(this.resolutionLocation2, this.width, this.height);
		this.gl.activeTexture(this.gl.TEXTURE0);
		this.gl.bindTexture(this.gl.TEXTURE_2D, this.rttTexture);
		this.gl.uniform1i(this.textureLocation, 0);
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer2);
		this.gl.enableVertexAttribArray(this.positionLocation2);
		this.gl.vertexAttribPointer(this.positionLocation2, 2, this.gl.FLOAT, false, 0, 0);
		this.gl.uniform1f(this.timeLocation2, this.time);
		this.gl.drawArrays(this.gl.TRIANGLES, 0, 6);

		this.time += 1.0 / 60.0;

		// draw renderbuffer to screen, projected correctly.



		requestAnimationFrame(this.renderFrame.bind(this));
	}

	Renderer.prototype.setup = function(element) {
		var _this = this;

		this.element = element;

		var canvas = element;// document.getElementById('gfx');

		this.gl = getWebGLContext(canvas);
		if (!this.gl) {
			return;
		}

		this.width = canvas.offsetWidth;
		this.height = canvas.offsetHeight;
		console.log('size', this.width, this.height);

		// setup GLSL program
		var vertexShader = createShaderFromScriptElement(this.gl, "2d-vertex-shader");
		var fragmentShader = createShaderFromScriptElement(this.gl, "2d-fragment-shader");
		this.program = createProgram(this.gl, [vertexShader, fragmentShader]);

		var vertexShader2 = createShaderFromScriptElement(this.gl, "2d-vertex-shader-2");
		var fragmentShader2 = createShaderFromScriptElement(this.gl, "2d-fragment-shader-2");
		this.program2 = createProgram(this.gl, [vertexShader2, fragmentShader2]);

		this.rttFramebuffer = this.gl.createFramebuffer();

		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.rttFramebuffer);

		this.rttFramebuffer.width = 1024;
		this.rttFramebuffer.height = 1024;

		this.rttTexture = this.gl.createTexture();
		this.gl.bindTexture(this.gl.TEXTURE_2D, this.rttTexture);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR_MIPMAP_NEAREST);
		this.gl.generateMipmap(this.gl.TEXTURE_2D);

		this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.RGBA, this.rttFramebuffer.width, this.rttFramebuffer.height, 0, this.gl.RGBA, this.gl.UNSIGNED_BYTE, null);

		var renderbuffer = this.gl.createRenderbuffer();
		this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, renderbuffer);
		this.gl.renderbufferStorage(this.gl.RENDERBUFFER, this.gl.DEPTH_COMPONENT16, this.rttFramebuffer.width, this.rttFramebuffer.height);

		this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.COLOR_ATTACHMENT0, this.gl.TEXTURE_2D, this.rttTexture, 0);
		this.gl.framebufferRenderbuffer(this.gl.FRAMEBUFFER, this.gl.DEPTH_ATTACHMENT, this.gl.RENDERBUFFER, renderbuffer);

		this.gl.bindTexture(this.gl.TEXTURE_2D, null);
		this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, null);
		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);

		this.positionLocation = this.gl.getAttribLocation(this.program, "position");
		this.positionLocation2 = this.gl.getAttribLocation(this.program2, "position");
		this.texCoordLocation2 = this.gl.getAttribLocation(this.program2, "texcoord");
		this.resolutionLocation = this.gl.getUniformLocation(this.program, "resolution");
		this.resolutionLocation2 = this.gl.getUniformLocation(this.program2, "resolution");
		this.timeLocation = this.gl.getUniformLocation(this.program, "time");
		this.timeLocation2 = this.gl.getUniformLocation(this.program2, "time");
		this.textureLocation = this.gl.getUniformLocation(this.program2, "texture");

		// Create a buffer and put a single clipspace rectangle in
		// it (2 triangles)
		this.buffer = this.gl.createBuffer();
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer);
		this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array([
			-1.0, -1.0, 1.0, -1.0, -1.0,  1.0,
			-1.0,  1.0, 1.0, -1.0, 1.0,  1.0]), this.gl.STATIC_DRAW);
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer);
		this.gl.enableVertexAttribArray(this.positionLocation);
		this.gl.vertexAttribPointer(this.positionLocation, 2, this.gl.FLOAT, false, 0, 0);

		this.buffer2 = this.gl.createBuffer();
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer2);
		this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array([
			-0.5, -1.0, 1.0, -1.0, -1.0,  1.0,
			-0.5,  1.0, 1.0, -1.0, 1.0,  1.0]), this.gl.STATIC_DRAW);
		this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffer2);
		this.gl.enableVertexAttribArray(this.positionLocation2);
		this.gl.vertexAttribPointer(this.positionLocation2, 2, this.gl.FLOAT, false, 0, 0);
		// this.gl.textureCoordAttribPointer(this.texCoordLocation2, 2, this.gl.FLOAT, false, 0, 0);

		this.gl.bindTexture(this.gl.TEXTURE_2D, this.rttTexture);
		this.gl.generateMipmap(this.gl.TEXTURE_2D);
		this.gl.bindTexture(this.gl.TEXTURE_2D, null);

		function resize() {
			_this.width = window.innerWidth;
			_this.height = window.innerHeight;
			console.log('resize to', _this.width, _this.height);
			canvas.width = _this.width;
			canvas.height = _this.height;
		}

		window.addEventListener('resize', resize);

		resize();
		this.renderFrame();
	};




	window.addEventListener('load', function() {
		var socket = io();

		var renderer = new Renderer();
		renderer.drawing = new Drawing();

		socket.on('message', function(msg) {
			if (msg[0] == '/draw/clear') {
				renderer.drawing.clear();
			}
			else if (msg[0] == '/draw/pen/move') {
				renderer.drawing.move(msg[1], msg[2]);
			}
			else if (msg[0] == '/draw/pen/down') {
				renderer.drawing.begin();
			}
			else if (msg[0] == '/draw/pen/up') {
				renderer.drawing.end();
			}
			else if (msg[0] == '/draw/set/color') {
				renderer.drawing.setColor(msg[1], msg[2], msg[3]);
			}
			else if (msg[0] == '/draw/set/drip') {
				renderer.drawing.setColor(msg[1]);
			}
			else if (msg[0] == '/draw/set/width') {
				renderer.drawing.setColor(msg[1]);
			}
			else {
				console.log('Unhandled message', msg);
			}
		});

		renderer.setup(document.getElementById('gfx'));





































	});

})();
