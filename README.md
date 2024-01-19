**THIS IS STILL WIP!!!**

# wayfire-plugin-template

This repository contains a basic skeleton of a Wayfire plugin, together with a few examples of common operations
and features plugins typically use. The aim is to provide a good starting point when writing a custom plugin.

Generally, a plugin will consist of at least the following:

- `meson.build`: the plugin's build system.
- `metadata/<your plugin name>.xml`: contains descriptions of all options of your plugin.
- `src/<your plugin name>.cpp`: contains the code of your plugin

Take a look at the various files in this repository, they have been commented rather thoroughly to guide you through the API.

# Building

```
meson build
ninja -C build
sudo ninja -C build install
```

Keep in mind that if you have installed Wayfire to a non-standard location (for example `/opt/wayfire`) you will need
to adjust `PKG_CONFIG_PATH` so that meson can find the Wayfire installation
(`PKG_CONFIG_PATH=/opt/wayfire/lib64/pkgconfig` before `meson build`, adjust for your distro and installation prefix).

# Installing a plugin

The build system is set up by default so that plugins are installed at the same location
where Wayfire's main plugins are installed.
If you want to change that, you can edit `src/meson.build` and override where the plugin is installed.
Alternatively, you can compile your plugin and not install it at all: simply use the full plugin path
in your config file, and set `WAYFIRE_PLUGIN_XML_PATH` (before starting Wayfire) to contain the directory
where your plugin's XML file is located.
