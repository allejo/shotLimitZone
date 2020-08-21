# Shot Limit Zone

[![GitHub release](https://img.shields.io/github/release/allejo/shotLimitZone.svg)](https://github.com/allejo/shotLimitZone/releases/latest)
![Minimum BZFlag Version](https://img.shields.io/badge/BZFlag-v2.4.4+-blue.svg)
[![License](https://img.shields.io/github/license/allejo/shotLimitZone.svg)](/LICENSE.md)

A BZFlag plugin that allows you add shot limits to flag based on the location of the flag. The difference between this plugin and the default BZFS `-sl` option is that by using the option, you are limiting all of the flags of the same type where with this plugin you will only limit the shot limit of a flag if grabbed from a specific area in the map. This plugin will not overwrite the `-sl` option.

Author
------

Vladimir "allejo" Jimenez

Compiling
---------

### Requirement

- BZFlag 2.4.3+

### How to Compile

1.  Check out the BZFlag source code.

    `git clone -b v2_4_x https://github.com/BZFlag-Dev/bzflag-import-3.git bzflag`

2.  Go into the newly checked out source code and then the plugins directory.

    `cd bzflag/plugins`

3.  Run a git clone of this repository from within the plugins directory. This should have created a new shotLimitZone directory within the plugins directory.

    `git clone -b release https://github.com/allejo/shotLimitZone.git`

4.  Add the plug-in to the Linux build system

    `sh addToBuild.sh shotLimitZone`

5.  Instruct the build system to generate a Makefile and then compile and install the plugin.

    `cd ..; ./autogen.sh; ./configure; make; make install;`
    

### Updating the Plugin

1.  Go into the shotLimitZone folder located in your plugins folder.

2.  Pull the changes from Git.

    `git pull origin release`

3.  (Optional) If you have made local changes to any of the files from this project, you may receive conflict errors where you may resolve the conflicts yourself or you may simply overwrite your changes with whatever is in the repository, which is recommended. *If you have a conflict every time you update because of your local change, submit a pull request and it will be accepted, provided it's a reasonable change.*

    `git reset --hard origin/release; git pull`

4.  Compile the changes.

    `make; make install;`
    
Server Details
--------------

### How to Use

To use this plugin after it has been compiled, simply load the plugin via the configuration file.

`-loadplugin /path/to/shotLimitZone.so`

Map Details
-----------

### Example

In this example, any SW flag grabbed from inside of this zone will be limited to 10 shots only.

**Box**

```
shotLimitZone
    position 0 0 0
    size 10 10 5
    shotLimit 10
    flag SW
end
```

**Cylinder**

```
shotLimitZone
    position 0 0 0
    radius 15
    height 10
    shotLimit 10
    flag SW
end
```


### Notes

* This custom zone will not actually spawn a SW flag, you will need to add a regular zone object to do so.
* The name ('shotLimitZone') of the object is case-insensitive so camel case is not required.

Planned Features
----------------

This plugin has a lot of room for additions and these are the ones I have in mind. If you don't see your idea here, please suggest it by creating an "issue" and marking it as an "enchancement."

- Add possible zone generation to the world blob to spawn a flag instead of forcing the user to add it?

License
-------

[GNU General Public License Version 3.0](https://github.com/allejo/shotLimitZone/blob/master/LICENSE.markdown)
