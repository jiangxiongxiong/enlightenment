#include <Ecore.h>

static Ecore_List *loaded_plugins = NULL;

/**
 * ecore_plugin_load - load the specified plugin from the specified path group
 * @group_id: the path group to search for the plugin to load
 * @plugin_name: the name of the plugin to load
 *
 * Returns a pointer to the newly loaded plugin on success, NULL on failure.
 */
Ecore_Plugin *
ecore_plugin_load(int group_id, char *plugin_name)
{
	char *path;
	char temp[PATH_MAX];

	Ecore_Plugin *plugin;
	void *handle = NULL;

	CHECK_PARAM_POINTER_RETURN("plugin_name", plugin_name, NULL);

	snprintf(temp, PATH_MAX, "%s.so", plugin_name);
	path = ecore_path_group_find(group_id, temp);
	if (!path)
		return NULL;

	handle = dlopen(path, RTLD_LAZY);
	if (!handle)
		return NULL;

	/*
	 * Allocate the new plugin and initialize it's fields
	 */
	plugin = malloc(sizeof(Ecore_Plugin));
	memset(plugin, 0, sizeof(Ecore_Plugin));

	plugin->group = group_id;
	plugin->name = strdup(plugin_name);
	plugin->handle = handle;

	/*
	 * Now add it to the list of the groups loaded plugins
	 */
	if (!loaded_plugins)
		loaded_plugins = ecore_list_new();

	ecore_list_append(loaded_plugins, plugin);

	FREE(path);

	return plugin;
}

/**
 * ecore_plugin_unload - unload the specified plugin
 * @plugin: the plugin to unload from memory
 *
 * Returns no value.
 */
void
ecore_plugin_unload(Ecore_Plugin * plugin)
{
	CHECK_PARAM_POINTER("plugin", plugin);

	if (!plugin->handle)
		return;

	if (ecore_list_goto(loaded_plugins, plugin))
		ecore_list_remove(loaded_plugins);

	dlclose(plugin->handle);

	FREE(plugin->name);
	FREE(plugin);
}

/*
 * Lookup the specified symbol for the plugin
 */
void *
ecore_plugin_call(Ecore_Plugin * plugin, char *symbol_name)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("plugin", plugin, NULL);
	CHECK_PARAM_POINTER_RETURN("symbol_name", symbol_name, NULL);

	if (!plugin->handle)
		return NULL;

	ret = dlsym(plugin->handle, symbol_name);

	return ret;
}
