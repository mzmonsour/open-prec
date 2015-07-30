# Open-PREC

Record demos in Team Fortress 2 automatically

Open-PREC is an open source remake of the popular demo recording tool POV-Record (P-REC). This
plugin remains mostly compatible with P-REC, sharing most of the same cvars, commands, and
configuration. However, this plugin is designed to build not only on Windows, but also Linux and
OSX, for players who cannot use P-REC on their system.

## Usage

Since this plugin is in development, it has not been signed yet, and will not be loaded into the
game normally. If you want to preview the plugin, or test your own build, make sure to start
Team Fortress 2 with the following option:

```
-insecure
```

On Steam, this can be added to your launch options by right clicking on the game, then going to
Properties > Set Launch Options

When in the game, the plugin can either be loaded through steam pipe automatically, or via the
in-game console with the following command:

```
plugin_load /path/to/open-prec.so
```

Once loaded, the plugin can be controlled through various commands and cvars. Use

```
prec_about
```

and

```
prec_info
```

to get more information about the plugin.

## Building From Scratch

After cloning the repo, make sure to check out the source SDK via:

```
$ git submodule update --init --recursive
```

### Linux

Simply running:

```
$ make
```

will create open-prec.so in your working directory.
