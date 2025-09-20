# Mod Loading order

## Two Phases

{{all}}

Mods are loaded depending on their [ModInfo](./modinfo.md#dependencies) in two phases.

``` mermaid
graph LR
  A[Normal Phase] --> C;
  C[Late Phase];
```

## Load After Dependency

{{all}}

Within a phase mods are ordered by their `LoadAfter` entry.

Mods specified in `LoadAfter` in `modinfo.json` are loaded before.
Circular dependencies result in undefined order.

??? info "No warning is printed if the mentioned mod is not available."
    Use [`Dependencies.Require`](./modinfo.md#dependencies) {{a117}} or [`ModDependencies`](./modinfo.md#dependencies) {{a1800}} to mark a mod as required.

=== ":material-pillar: 117"
    ```json title="modinfo.json"
    {
      "ModID": "mod-c",
      "Dependencies": {
        "LoadAfter": [
          "mod-a",
          "mod-b"
        ]
      }
    }
    ```
=== ":material-factory: 1800"
    ```json title="modinfo.json"
    {
      "ModID": "mod-c",
      "LoadAfterIds": [
        "mod-a",
        "mod-b"
      ]
    }
    ```

??? warning "Do not rely on alphabetical loading behavior."
    In order to keep some compatibilty with mods created before the introduction of mod dependencies, mods without dependency information are loaded alphabetically.

    But this is unpredictable, because as soon as another mod depends on it, it will not be alphabetical anymore.

## Late Phase

{{all}}

??? info "Only late phase mods can depend on other late phase mods."
    A mod without a `*` cannot load after a mod with a `*`.

=== ":material-pillar: 117"
    ```json
    {
      "ModID": "post-b",
      "Dependencies": {
        "LoadAfterIds": [
          "*",
          "post-a"
        ]
      }
    }
    ```
=== ":material-factory: 1800"
    ```json
    {
      "ModID": "post-b",
      "LoadAfterIds": [
        "*",
        "post-a"
      ]
    }
    ```
