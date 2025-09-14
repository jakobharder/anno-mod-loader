# Lua Scripts

{{a117}}

!!! warning "Experimental"
    This is the first release with notable script features.
    Expect changes with the next versions.

Define scripts and commands in `modinfo.json`:

=== ":material-pillar: 117"
    ```json
    {
      // ..
      "scripts": {
        "modules": [
          "mymod/some-script.lua"
        ],
        "Init": "SomeScript = require(\"some-script\")",
        "Tick": "SomeScript:Tick()"
      }
    }
    ```

Be sure to use unique names to not clash with other mods!

`modules` defines where scripts are located.

Event|When
---|---
`Init`|after mod loading
`Load`|load of a save or new game
`Unload`|unload of the current game
`Tick`|meta game tick

Content of example `some-script.lua`:

```lua
local SomeScript = {}

function SomeScript.Tick()
end

return SomeScript;
```
