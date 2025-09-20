# Debug Mods

The game writes a new `mod-loader.log` file every game start.
It lists loading order, patch and dependency issues.

=== ":material-pillar: 117"
    ```plaintext
    <user documents>/Anno 117/mods/mod-loader.log
    ```
=== ":material-factory: 1800"
    ```plaintext
    <user documents>/Anno 1800/log/mod-loader.log
    ```

To test what a patch does to the original game file, you can also use `xmltest.exe`.
It will simulate what the game will load.

```
xmltest.exe c:\anno\all-rda\assets.xml patch.xml
```

??? tip "Use [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) for testing."

    The plugin does [syntax and error checking](https://github.com/anno-mods/vscode-anno-modding-tools/blob/HEAD/doc/error-checking.md).

    It also has commands to [show diffs](https://github.com/anno-mods/vscode-anno-modding-tools/blob/HEAD/doc/utilities.md#show-diff) of full mods, single files or selections.
