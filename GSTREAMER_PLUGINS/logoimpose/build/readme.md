//How to compile this plugin

first top of direction in which Cmake file exist,
  ----> cmake -B build
    -- Configuring done
    -- Generating done
    -- Build files have been written to: <path>
 This command creates all the build files realted to project and links all the packages including make file in build directory.
 
 Now, go to build directory and compile using make
 
 _____________________________________________________
 
 Cairo_Surface_T *
Cairo_Image_Surface_Create_From_Png (Const Char *Filename);
Creates a new image surface and initializes the contents to the given PNG file.
Parameters
filename
name of PNG file to load.
Returns
a new cairo_surface_t initialized with the contents of the PNG file, or a "nil" surface if any error occurred. A nil surface can be checked for with cairo_surface_status(surface) 


