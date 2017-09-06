	#include "nuetron.h"
	static int16_t wmm_latitude; 				// last lattitude
	static int16_t wmm_longitude; 			// last longitude
	static int16_t latitude;						// current gps latitude
	static int16_t longitude;						// current gps longitude

	static int16_t normalize_degrees(int16_t degrees)
		{
		while(degrees < 360)
			degrees += 360;

		while(degrees > 359)
			degrees -= 360;

		return degrees;
		}

	struct decl {
			int16_t base_declination;
			int8_t decl_adjust[180];		// each nibble is +/- degs
			// (low nibble is eastings, high nibble westings)
			};

	// stream that holds the wmm data
	static handle_t wmm_stream;

	void magnetic_variation_changed()
		{
			if(wmm_stream == 0)
				{
				if(reg_open_stream(0, "wmm.dat", &wmm_stream)<0)
					return;
				}

		// determine if we have changed
		if(latitude != wmm_latitude ||
			 longitude != wmm_longitude)
			{
					uint16_t pos = latitude;
				// convert to 0 base
				pos += 90;

				pos *= sizeof(decl);

				// TODO: handle errors
				stream_setpos(wmm_stream, pos);

				int16_t declination;
				stream_read(wmm_stream, &declination, sizeof(int16_t));

				// the position is based on the
				uint16_t lng = 0;
				uint16_t abs_lng = abs(longitude);
				iomnt8_t decl;
				for(lng = 0; lng < abs_lng; lng++)
					{
					stream_read(wmm_stream, &decl, 1);
					if(longitude < 0)
						decl >>= 4;
					decl &= 0x0f;
					declination += decl;
					}

				wmm_latitude = latitude;
				wmm_longitude = longitude;
				// we have a new mag variation
				publish_short(declination, id_magnetic_variation);
			}
		}

	void ev_msg(can_msg_t *msg)
		{
		switch(msg->id)
			{
			case id_gps_aircraft_lattitude :
				latitude = (int16_t) radians_to_degrees(get_param_float(msg));
				magnetic_variation_changed();
				break;
			case id_gps_aircraft_longitude :
				longitude = (int16_t) radians_to_degrees(get_param_float(msg));
				magnetic_variation_changed();
				break;
			}
		}
