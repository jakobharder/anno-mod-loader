# New Feature: Options in Anno 117

Options are comparable to iModYourAnno Tweaks.

Options can be defined in an external `options.json` file.

These options are accessible as `$mod-id.option-name` in XPath.

## Options file

The `options.json` file is read from the `mods/` folder with the following format:

=== "117"
    ```jsonc
    {
      "mod-id": {
        "range": "10",
        "useInfluence": "true"
      }
    }
    ```

!!! info "Future versions will generate the options.json from default values, but for now you have to create it yourself."

The `modinfo.json` file contains defaults:

=== "117"
    ```jsonc
    {
      // ..
      "options": {
        "range": {
          "default": "10"
        },
        "useInfluence": {
          "default": "true"
        }
      }
    }
    ```

!!! info "The mod loader only requires defaults"
    Additional information like labels, allowed values, type, steps, etc. is not relevant here and thus omitted.

## Use as Condition

You can leave out `mod-id` if the variable comes from the same mod shortening the path to `$option-name`.

=== "117"
    ```xml
    <ModOps>
      <ModOp Condition="$use-influence"
            Replace="@123/Costs/Influence" >
        <Influence>3</Influence>
      </ModOp>
      <ModOp Condition="$other-mod.range &lt; 20"
            Merge="@123/Service/PublicServiceRange">
        <PublicServiceRange>20</PublicServiceRange>
      </ModOp>
    </ModOps>
    ```

!!! info "`<>` are not valid characters in attributes. Use `&lt;` for `<` and `&gt;` for `>` instead."

## Use As Value

Use options directly as values using `ModValue Insert`.

=== "117"
    ```xml
    <ModOp Merge="@123">
      <PublicServiceRange><ModValue Insert="$mod-id.range"/></PublicServiceRange>
    </ModOp>
    ```


## Additional Modinfos

Specify user customizable options of a mod (like iModYourAnno tweaks) in `modinfo.json`:

=== "117"
    ```jsonc
    {
      // ..
      "options": {
        "range": {
          "label": "Electricity Range",
          "type": "enum",
          "default": "10",
          "values": [ "10", "20", "30" ],
          "labels": [ "10 Street Range", "20 Street Range", "30 Street Range" ]
        },
        "useInfluence": {
          "label": "Use Influence Cost",
          "default": "true",
          "type": "toggle"
        }
      }
    }
    ```

!!! info "Only `default` is relevant for the actual mod loading process"
    The rest is information for valid values and descriptions to be used by mod managers modifying the user values in `options.json`.

