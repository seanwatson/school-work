/*******************************************************************************
 *
 * File         : plugins.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Helper functions for working with assembler plugins.
 *
 ******************************************************************************/

#ifndef PLUGINS_H
#define PLUGINS_H

/*******************************************************************************
 *
 * get_plugin_names
 *
 * Returns an array of .so files found in the specified plugin directory
 *
 * Parameters:
 *    plugin_dir    The directory in which to search for plugins
 *    count         [OUT PARAMETER] Stores the number of plugins in the returned
 *                  array after the function returns
 *
 * Returns:
 *  An array of filenames representing .so files found in the plugin directory
 *
 ******************************************************************************/
char** get_plugin_names(char* plugin_dir, int* count);

/*******************************************************************************
 *
 * load_plugin
 *
 * Attempts to load the specified plugin.  If successfully loaded, a fully 
 * initialized permanent symbol table entry is returned, ready to be inserted
 * into the permanent symbol table.
 *
 * Parameters:
 *    plugin_filename   Name of the plugin to load
 *
 * Returns:
 *  An initialized permanent symbol table entry, if the plugin was loaded 
 *  successfully; otherwise, NULL
 *
 ******************************************************************************/
perm_symtab_entry_t* load_plugin(char* plugin_filename);
	
#endif
