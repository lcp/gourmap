#ifndef __GOOGLE_MAP_TEMPLATE_H__
#define __GOOGLE_MAP_TEMPLATE_H__

/* Parameters:
   (double) latitude,
   (double) longitude,
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
"function initialize() {"
"	var myLatlng = new google.maps.LatLng(%.6f, %.6f);" /*Latitude, Longitude*/
"	var myOptions = {"
"		zoom: %u," /* Zoom level */
"		center: myLatlng,"
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
   (char *) restaurant_name,
   (double) restaurant_latitude,
   (double) restaurant_longitude,
*/
const char *gmap_restaurant_markers =
"	var restaurants = ["
"		['%s', %0.6f, %0.6f],"
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
"		var content = '<b>' + restaurant[0] + '</b>';"
"		attachInfowindow (marker, content);"
"	}"
"";

#endif /* __GOOGLE_MAP_TEMPLATE_H__ */
