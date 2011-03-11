/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2011 Gary Ching-Pang Lin <glin@novell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * Special Thanks to google-maps-icons project which provides a free high
 * quality restaurant icon.
 */

#ifndef __GOOGLE_MAP_TEMPLATE_H__
#define __GOOGLE_MAP_TEMPLATE_H__

/* Parameters:
   (double) my_latitude,
   (double) my_longitude,
   (double) map_latitude,
   (double) map_longitude,
   (unsigned int) map_zoom_level,
   (unsigned int) circle_radius,
   (char *) gmap_restaurant_markers
*/
const char *google_map_template =
"<!DOCTYPE html><html><head>"
"<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/>"
"<title>Google Map template</title>"
"<link href=\"http://code.google.com/apis/maps/documentation/javascript/examples/default.css\" rel=\"stylesheet\" type=\"text/css\" />"
"<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\"></script>"
"<script type=\"text/javascript\">"
"var map;"
"var circle;"
"var myLatlng = new google.maps.LatLng(%.6f, %.6f);" /*my_latitude, my_longitude*/
"var mapCenter = new google.maps.LatLng(%.6f, %.6f);" /*map_latitude, map_ongitude*/
"function initialize() {"
"	var myOptions = {"
"		zoom: %u," /* Zoom level */
"		center: mapCenter,"
"		mapTypeControl: false,"
"		mapTypeId: google.maps.MapTypeId.ROADMAP,"
"		streetViewControl: false,"
"		scaleControl: true,"
"	};"
"	map = new google.maps.Map("
"		document.getElementById(\"map_canvas\"),"
"		myOptions"
"	);"
"	circle = new google.maps.Circle({"
"		center: myLatlng,"
"		radius: %u," /* Radius of the circle */
"		map: map,"
"		fillOpacity: 0.1,"
"		strokeOpacity: 0.3,"
"		strokeWeight: 1,"
"		clickable: false,"
"	});"
"	var marker = new google.maps.Marker({"
"		position: myLatlng,"
"		map: map,"
"	});"
"%s" /* Restaurant Markers */
"}"
"function attachInfowindow(marker, content) {"
"	var infowindow = new google.maps.InfoWindow({"
"		content: content"
"	});"
"	google.maps.event.addListener(marker, 'click', function() {"
"		infowindow.open(map, marker);"
"	});"
"}"
"</script></head><body onload=\"initialize()\"><div id=\"map_canvas\"></div></body></html>"
"";

/* Parameters:
   (char *) array_of_locations
      format: ['name_1', latitude_1, longitude_1, address_1], ['name_2', latitude_2, longitude_2, address_2], ...
*/
const char *gmap_restaurant_markers =
"	var restaurants = ["
"		%s"
"	];"
"	for (var i = 0; i < restaurants.length; i++) {"
"		var restaurant = restaurants[i];"
"		var location = new google.maps.LatLng("
"			restaurant[1],"
"			restaurant[2]"
"		);"
"		var image = 'http://google-maps-icons.googlecode.com/files/restaurant.png';"
"		var marker = new google.maps.Marker({"
"			position: location,"
"			map: map,"
"			icon: image,"
"			title: restaurant[0],"
"		});"
"		if (location.equals (mapCenter)) {"
"			marker.setZIndex (10);"
"			marker.setAnimation(google.maps.Animation.BOUNCE);"
"		} else {"
"			marker.setZIndex (0);"
"		}"
"		var content = '<b>' + restaurant[0] + '</b><br>' + restaurant[3];"
"		attachInfowindow (marker, content);"
"	}"
"";

#endif /* __GOOGLE_MAP_TEMPLATE_H__ */
