# File Structure

## Basic File Structure

{{all}}

=== ":material-pillar: 117"
    ```shell title="basic mod file structure" hl_lines="11"
    ├─ data/
    │  ├─ base/config/
    │  │  ├─ export/
    │  │  │  └─ assets.xml            # asset XML patches
    │  │  └─ gui/
    │  │     └─ texts_english.xml     # language files
    │  └─ <your name>/
    │     └─ **/*                     # models, textures
    ├─ banner.jpg
    ├─ logo.jpg
    ├─ modinfo.json                   # mod id, version, ...
    └─ README.md
    ```
=== ":material-factory: 1800"
    ```shell title="basic mod file structure" hl_lines="11"
    ├─ data/
    │  ├─ config/
    │  │  ├─ export/main/asset/
    │  │  │  └─ assets.xml            # asset XML patches
    │  │  └─ gui/
    │  │     └─ texts_english.xml     # language files
    │  └─ <your name>/
    │     └─ **/*                     # models, textures
    ├─ banner.jpg
    ├─ logo.jpg
    ├─ modinfo.json                   # mod id, version, ...
    └─ README.md
    ```

!!! warning "Anno 117 and Anno 1800 (mod.io only) won't load mods without a modinfo file."
    A `modinfo.json` file is required in the top-level folder of your mod.
    Description, images and all other content files are optional.

File | Description
--- | ---
`banner.jpg` | Large 16:9 image for offline mod managers.
`logo.jpg` | Small 16:9 image. Used in Anno 1800 mod activation list.
`modinfo.json` | ModID, version, dependencies and other infos.
`README.md` | Mod description.

??? tip "Templates in Visual Studio Code"
    Use a template to create the basic mod file structure with empty files.

    Press key `F1` in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template`.

## Nested Mods

{{all}}

Mods can contain mods.

```shell hl_lines="3"
├─ data/
├─ modinfo.json
└─ nested-mod/
   ├─ data/
   └─ modinfo.json
```

??? info "The loading order of nested mods is independent of their parent mod."

    === ":material-pillar: 117"
        ```json title="modinfo.json"
        {
          // ..
          "Dependencies": {
            "LoadAfter": [ "other-mod" ]
          }
        }
        ```
    === ":material-factory: 1800"
        ```json title="modinfo.json"
        {
          // ..
          "LoadAfterIds": [ "other-mod" ]
        }
        ```

    Read more about [Loading Order](./loading-order.md).

## Shared Mods

{{all}}

Mods can share nested mods.
Only one version of it is loaded.
The highest version is chosen, if multiple of the same are there then one is picked.

```shell hl_lines="4 10"
- mod-a/
  ├─ data/
  ├─ modinfo.json
  └─ mod-c/
     ├─ data/
     └─ modinfo.json
- mod-b/
  ├─ data/
  ├─ modinfo.json
  └─ mod-c/
     ├─ data/
     └─ modinfo.json
```

## Link Files

{{a1800}}

You can link files by adding a text file with the extension `.lnk`.
The path and name of the file is the link source, the text file contents is the link target.

This is useful to duplicate islands under a different path for example.

!!! note "{{a117}} This feature will come back for Anno 117, but it is currently untested in its old form."

## Mod Collections

{{all}}

You can distribute and use multiple mods in one folder.

See [Collection Folders](./install.md#collection-folders).
