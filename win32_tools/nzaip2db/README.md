This project will take a directory that contains geo json files and generate a database from them.

example usage:

nzaip2db -s "C:\Projects\NZDATA" -r W180,N90,E180,S60 -n "NZ Airspace" C:\Projects\map_data\nz_airspace.db

The path C:\Projects\map_data is also hard coded at present in the map constructor at 

diy-efis\libs\mfdlib\mfd_layout.c  : Line 83


    if (failed(result = load_layers(wnd, "C:\\Projects\\map_data")))
        return result;


All of the tools put the databases in there