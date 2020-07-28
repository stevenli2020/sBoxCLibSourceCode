# Files
abecore and cpabe: core libs source code  <br>
abes: a seperated and simplified library  <br>
sbox_api: sbox_api lib source code <br>
sbox_api_without_glib: sbox_api lib source code compiled with myglib instead of glib <br>
sbox_api_without_glib_abes: sbox_api lib source code compiled with myglib instead of glib and using abes library without abecore and cpabe<br>
myglib: a library achieving some functions of glib <br>
sharelibs: all involved shared library files (.so) <br>
TestApp: a test program for abecore and cpabe libraries <br>

# For runnable program
	in sbox_api/, sbox_api_without_glib/ or sbox_api_without_glib_abes/ directory 
	run "make exe" "make install" to get 'sbox' program 
	run "./sbox" with arguements
	we have a sample script in that directory (exe_sample) 

# For test runable program
	in sbox_api/ or sbox_api_without_glib/ directory 
	run "make exe EXEFLAG=-DTEST" "make install" to get 'sbox' program 
	run "./sbox" 
	but in sbox_api_without_glib_abes/ directory
	run "make exe" "make install" to get 'sbox_test' program 
	run "./sbox" 

# For sbox library
	in sbox_api/ or sbox_api_without_glib/ directory 
	run "make libs" to get libsboxapi.so and lib, it will be install in ./target/ 

# For abecore, cpabe, myglib, abes and sbox library
	run "make libs" to get both shared and static libraries in ./target/ 
	
	
