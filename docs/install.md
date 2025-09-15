# Install Mods

## Mod Folders

{{all}}

Place a mod in one of the following locations to use it.

=== ":material-pillar: 117"
    - `<user>/Anno 117/mods/`
    - `<install>/Anno 117/mods/`
=== ":material-factory: 1800"
    - `<user>/Anno 1800/mods/`
    - `<install>/Anno 1800/mods/`

## Collection Folders

{{all}}

```yaml
- mods/
  - Collection/
    - ModA/
      - data/
      - modinfo.json
    - ModB/
      - data/
      - modinfo.json
```

Collection folders do not act as mods.
They don't have activation or `ModID`s.

A mod manager may bulk enable/disable based on those folders though...

`modinfo.json` is required for mods in sub-folders.

## Zip Files

{{a1800}}

Mods can be read directly from .zip files.
Each individual mod within the .zip file is treated as a mod.

=== "Single Mod"
    ```shell
    └─ Mod.zip/
       ├─ data/
       └─ modinfo.json
    ```
=== "With Subfolder"
    ```shell
    └─ Mod.zip/
       └─ Mod/
          ├─ data/
          └─ modinfo.json
    ```
=== "Collection"
    ```shell
    └─ Collection.zip/
       ├─ ModA/
       │  ├─ data/
       │  └─ modinfo.json
       └─ ModB/
          ├─ data/
          └─ modinfo.json
    ```

!!! warning "Island files are known to have problems when loaded from .zip files."
