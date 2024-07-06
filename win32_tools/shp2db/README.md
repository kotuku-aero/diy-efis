This is probably the most complex of the database tools in that it converts shapefiles into databases.

Usage: shp2db Options path

 Options are one of:

    -h      Print help

    -s <path>  set source path to <path> (as many as required)

    -t <type>  set the container type (required)

    -r <w,n,e,s> set the spatial rect as west, north, east, south

    -n <name>  set the name

    -z <min>,<max> set the min-max scale for the geography (required)

    -m <filename>  create a metadata file that exports the shape metadata

example: shp2db -s \\Projects\\contours\\land.shp -t coastlines -r W180,N90,E180,S60 -n \"Coastlines 7.5 arcsec\" -c 2301  \\Projects\\map_data\\coastlines.db;
  
