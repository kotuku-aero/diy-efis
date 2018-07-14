function (key) {
	this.vs0 = reg_get_uint16(key, "vs0");
	this.vs1 = reg_get_uint16(key, "vs1");
	this.vfe = reg_get_uint16(key, "vfe");
	this.vno = reg_get_uint16(key, "vno");
	this.vne = reg_get_uint16(key, "vne");
	this.va = reg_get_uint16(key, "va");
	this.vx = reg_get_uint16(key, "vx");
	this.vy = reg_get_uint16(key, "vy");
	this.scale = reg_get_float(key, "scale");
	this.offset = reg_get_float(key, "offset");
	this.background = reg_get_string(key, "background");
	this.color = reg_get_string(key, "color");
	// get the font key....
	this.font = { face:"neo", height: 9};
	this.pen = { width:1, color: reg_get_string(key, "color");
	this.large_roller = { face:"neo", height: 18 };
	this.small_roller = { face:"neo", height: 9 };

	this.airspeed = 0;
	this.vertical_speed = 0;
	
	// constructor function for a JS object that implements an airspeed window
	function on_paint() {
		this.rectangle(8, 8, this.width-9, this.height-9, background);
		var median = this.height / 2;
		// the vertical tape displays 28 knots around the current position
		// as there are 240 pixels.  We calc the upper one first
		var top_asi = (this.airspeed * 10) + median -10;
		var asi_line =(top_asi / 25) * 25;
		var marker_line;
		// work out how many lines to the next lowest marker
		for(marker_line =(top_asi - asi_line)+ 10; marker_line < this.height; marker_line += 25)
		{
			// draw a line from 10 pixels to 30 pixels then the text.
			// lines at 25 are shorter
			var pts = {
				{ x:(asi_line ==((asi_line / 50) * 50) ? 50 : 55), y: marker_line },
				{ x:65, y:marker_line }
			};

			this.polyline(pts, this.pen);

			if(asi_line ==((asi_line / 100) * 100))
			{
			var str = asi_line / 10;
			
			var ex = this.text_extent(str, this.font);
			this.draw_text(str, this.font, this.color, this.background,
				0, 0, this.width, this.height,
				47 - ex.dx, marker_line -(ex.dy >> 1));
			
			asi_line -= 25;

			if(asi_line < 0)
				break;
		}
	
		var roller = [
			{ x:47,  y:median },
			{ x:40,  y:median+7 },
			{ x:40,  y:median+20 },
			{ x:0,  y:median+20 },
			{ x:0,  y:median-20 },
			{ x:40,  y:median-20 },
			{ x:40,  y:median-7 },
			{ x:47,  y:median }];
			
		this.polygon(roller, "black", { color:white, width:1 });

		// now we draw the roller
		display_roller(1, median-19, 39, median+19, 
					 this.airspeed, 1, "black", "white",
					 large_roller, small_roller);
		return true;
	}
	
	// should be a library function in init.js
	// TODO:
	function display_roller(left, top, right, bottom,
		value, decimals, bg, fg, large_font, small_font)
		{
		}
}