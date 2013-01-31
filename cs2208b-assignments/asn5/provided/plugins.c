/*******************************************************************************
 *
 * File         : plugins.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in plugins.h.
 * 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>	
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "assembler.h"
#include "assembler_provided.h"

/*******************************************************************************
 *
 * Returns true if the specified file in the specified directory
 * has the extension .so and is a regular file
 *
 * Parameters:
 *   dir        The directory in which the file is located
 *   filename   The file to check
 *
 * Returns:
 *   1, if the file is a .so file; 0 otherwise
 *
 ******************************************************************************/
int is_so_file(char* dir, char* filename)
{
	struct stat entry_info;		// Used to get info on the file
	char path[255];			// Stores the full path of the file
	char* ext;			// File extension
	
	// Get the file extension
	ext = strrchr(filename, '.');

	// If the file has no extension, or it's not of length 3 (.so), or it
	// is not equal to .so, then it's not a .so file
	if ((!ext) || (strlen(ext) != 3) || (strncmp(ext, ".so", 3)))
		return 0;
	
	// Combine the directory and filename to get the full path 
	path[0] = '\0';
	strcat(path, dir);
	strcat(path, "/");
	strcat(path, filename);
	
	// If we can't read details on the file, or it's not a file (e.g. it's
	// a directory), then it's not a .so file
	if ((stat(path, &entry_info)) ||
	    ((entry_info.st_mode & S_IFREG) != S_IFREG))
		return 0;

	// Otherwise, we've found a valid .so file
	return 1;
}
	
/*******************************************************************************
 *
 * Returns an array of all filenames in the specified directory having
 * the extension .so.  The size of the array returned is contained in
 * the out parameter 'count' after the function returns.
 *
 * Parameters:
 *   plugin_dir The directory containing plugins
 *   count      [OUT PARAMETER] Stores the size of the returned array
 *
 * Returns:
 *   An array containing the names of all .so files in the specified plugin
 *   directory
 *
 ******************************************************************************/
char** get_plugin_names(char* plugin_dir, int* count)
{
	DIR* dir;			// Directory handle
	struct dirent* dir_entry;	// Entry in a directory
	char** plugin_names = NULL;	// Array of plugin names
	char* plugin_name;		// Name of a plugin

	// No plugins have been found yet
	*count = 0;	
	
	// Open a handle to the specified directory
	dir = opendir(plugin_dir);

	// Iterate through all entries in the directory
	while (dir_entry = readdir(dir))
	{
		// If the next entry is a valid .so file
		if (is_so_file(plugin_dir, dir_entry->d_name))
		{
			// Copy its name
			plugin_name = calloc(strlen(dir_entry->d_name) + 1, sizeof(char));
			strcpy(plugin_name, dir_entry->d_name);

			// Extend the plugin_names array, add the name to the array,
			// and increase the plugin count
			plugin_names = (char**)realloc(plugin_names, (*count + 1) * sizeof(char*));
			plugin_names[(*count)++] = plugin_name;
		}
	}

	return plugin_names;
}

/*******************************************************************************
 *
 * Dynamically loads the specified library and tries to load its plugin_name
 * variable.  If found, it is printed.  It then tries to load its do_operation()
 * function.  If found, the function is called with the specified operands.
 *
 * Parameters:
 *   plugin_filename  Name of the plugin file to load
 *
 ******************************************************************************/
perm_symtab_entry_t* load_plugin(char* plugin_filename)
{
	void* handle;				// Plugin handle
	char* mnemonic;
	void (*handler)(int, instruction_t, void (*)(int, instruction_t));
	perm_symtab_entry_t* entry;
	
	// Open the dynamic library containing the plugin
	handle = dlopen(plugin_filename, RTLD_NOW);

	// If we couldn't open it for some reason, return NULL
	if (handle == NULL)
		return NULL;

	// Clear any errors and try to load the plugin_name variable from the plugin
	dlerror();
	mnemonic = (char*)dlsym(handle, "mnemonic");

	// If an error occurred, print an error message; otherwise, print the plugin_name variable 
	// that was loaded from the plugin
	if (dlerror() != NULL)
	{
		dlclose(handle);
		return NULL;
	}
	
	// Clear any errors and try to load the do_operation() function from the plugin
	dlerror();
	handler = dlsym(handle, "handle");

	// If an error occurred, print an error message
	if (dlerror() != NULL)
	{
		dlclose(handle);
		return NULL;
	}
	
	entry = (perm_symtab_entry_t*)malloc(sizeof(perm_symtab_entry_t));
	entry->mnemonic = mnemonic;
	entry->handler.synth_fn = handler;
	entry->is_synthetic = TRUE;
	entry->plugin_handle = handle;
	
	return entry;
}
