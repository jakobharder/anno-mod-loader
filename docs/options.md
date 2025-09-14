# Options in Anno 117

Options are comparable to iModYourAnno Tweaks.

Options can be defined in an external `options.jsonc` file.

These options are accessible as `$mod-id.option-name` in XPath.

## Options file

{{v8()}}

The `options.jsonc` file is read from the `mods/` folder with the following format:

=== ":material-pillar: 117"
    ```json title="options.jsonc"
    {
      "mod-id": {
        "range": "10",
        "useInfluence": "true"
      }
    }
    ```

!!! info "Future versions will generate the options.json from default values, but for now you have to create it yourself."

The `modinfo.json` file contains defaults:

=== ":material-pillar: 117"
    ```json title="modinfo.json"
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

{{v8()}}

Option values can be used in `Condition` XPath.

=== ":material-at: 117"
    ```xml hl_lines="2"
    <ModOps>
      <ModOp Condition="$mod-id.use-influence"
             Merge="@123/Costs">
        <Influence>3</Influence>
      </ModOp>
    </ModOps>
    ```
=== ":material-cog-outline: Options"
    ```json hl_lines="3"
    {
      "mod-id": {
        "useInfluence": true
      }
    }
    ```
=== ":material-xml: Result"
    ```diff hl_lines="6"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <Costs>
    +     <Influence>3</Influence>
        </Costs>
      </Values>
    ```

## Use As Value

{{v8()}}

Option values can be used in `<ModValue Insert>` as values to insert.

=== ":material-at: 117"
    ```xml hl_lines="2"
    <ModOp Merge="@123">
      <PublicServiceRange><ModValue Insert="$mod-id.range"/></PublicServiceRange>
    </ModOp>
    ```
=== ":material-cog-outline: Options"
    ```json hl_lines="3"
    {
      "mod-id": {
        "range": "10"
      }
    }
    ```
=== ":material-xml: Result"
    ```diff hl_lines="5 6"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    -   <PublicServiceRange>20</PublicServiceRange>
    +   <PublicServiceRange>10</PublicServiceRange>
      </Values>
    ```

## Combined Example

{{v8()}}

This example uses the option value to compare in a condition, and only sets it when higher.

Also note that `mod-id` can be skipped if the variable comes from the same mod shortening the path to `$option-name`.

=== ":material-at: 117"
    ```xml
    <ModOps>
      <ModOp GUID="123" Merge="Service/RangeOne"
             Condition="Service/RangeOne &lt; $range">
        <RangeOne><ModValue Insert="$range"/></RangeOne>
      </ModOp>
      <ModOp GUID="123" Merge="Service/RangeTwo"
             Condition="Service/RangeTwo &lt; $range">
        <RangeTwo><ModValue Insert="$range"/></RangeTwo>
      </ModOp>
    </ModOps>
    ```
=== ":material-cog-outline: Options"
    ```json
    {
      "mod-id": {
        "range": "10"
      }
    }
    ```
=== ":material-file-code-outline: Input"
    ```xml hl_lines="5 6"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <RangeOne>30</RangeOne>
        <RangeTwo>5</RangeTwo>
      </Values>
    ```
=== ":material-xml: Result"
    ```diff hl_lines="5 6 7"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <RangeOne>30</RangeOne>
    -   <RangeTwo>5</RangeTwo>
    +   <RangeTwo>10</RangeTwo>
      </Values>
    ```

!!! info "`<>` are not valid characters in attributes. Use `&lt;` for `<` and `&gt;` for `>` instead."

## Additional Modinfos

{{v8()}}

Specify user customizable options of a mod (like iModYourAnno tweaks) in `modinfo.json`:

=== ":material-pillar: 117"
    ```json
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

