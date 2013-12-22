#Shot Limit Zone

A BZFlag plugin that allows you add shot limits to flag based on the location of the flag. The difference between this plugin and the default BZFS `-sl` option is that by using the option, you are limiting all of the flags of the same type where with this plugin you will only limit the shot limit of a flag if grabbed from a specific area in the map. This plugin will not overwrite the -sl option.

##Author

Vladimir "allejo" Jimenez

##Example

In this example, any SW flag grabbed from inside of this zone will be limited to 10 shots only.

```
shotLimitZone
    position 0 0 0
    size 10 10 5
    shotLimit 10
    flag SW
end
```

###Notes

* This custom zone will not actually spawn a SW flag, you will need to add a regular zone object to do so.
* The name ('shotLimitZone') of the object is case-insensitive so camel case is not required.
* The "position" field can also be written shorthand as "pos."
* These special zones cannot be rotated as of right now; this will be implemented in a future release.

##License
GPLv3