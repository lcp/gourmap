/* (double)latitude, (double)longitude, (unsigned int) zoom*/
const char *google_map_template = "\
<!DOCTYPE html>\
<html>\
<head>\
<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />\
<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/>\
<title>Google Map template</title>\
<link href=\"http://code.google.com/apis/maps/documentation/javascript/examples/default.css\" rel=\"stylesheet\" type=\"text/css\" />\
<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\"></script>\
<script type=\"text/javascript\">\
function initialize() {\
  var myLatlng = new google.maps.LatLng(%.6f, %.6f);\
  var myOptions = {\
    zoom: %u,\
    center: myLatlng,\
    mapTypeControl: false,\
    mapTypeId: google.maps.MapTypeId.ROADMAP,\
    streetViewControl: false,\
  };\
  var map = new google.maps.Map(\
      document.getElementById(\"map_canvas\"),\
      myOptions);\
  var marker = new google.maps.Marker({\
      position: myLatlng,\
      map: map,\
  });\
}\
</script>\
</head>\
<body onload=\"initialize()\">\
  <div id=\"map_canvas\"></div>\
</body>\
</html>\
";
