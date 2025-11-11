# Options in Anno 117

!!! note "Options are comparable to [iModYourAnno Tweaks](https://github.com/anno-mods/iModYourAnno/wiki/Setting-up-your-Mod-for-tweaking) in Anno 1800."

Options are values defined in a single file in the user documents.
These options are accessible as `$mod-id.option-name` in XPath.

Options allow a mod to be configured without changing any data in the mod folder making it more robust against updates.

## Options file

{{a117}}

The `active-options.jsonc` file is read from the `mods/` folder with the following format:

=== ":material-pillar: 117"
    ```json title="active-options.jsonc"
    {
      "mod-id": {
        "range": "10",
        "useInfluence": "true"
      }
    }
    ```

!!! info "Future versions will generate the `active-options.jsonc` from default values, but for now you have to create it yourself."

The `modinfo.json` file contains defaults:

=== ":material-pillar: 117"
    ```json title="modinfo.json"
    {
      // ..
      "Options": {
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

{{a117}}

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

{{a117}}

Option values can be used in `<ModValue Insert/>` as values to insert.

<!-- --8<-- [start:use-as-value] -->

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

<!-- --8<-- [end:use-as-value] -->

## Use in XPath Expressions

{{a117}}

Options act as XPath variables.
They can be used in comparisons and [calculations](./modops/content.md#insert-calculations).

This example uses the option value to compare in a condition, and only sets it when higher.

Also note that `mod-id` can be skipped if the variable comes from the same mod shortening the path to `$option-name`.

=== ":material-at: 117"
    ```xml
    <ModOps>
      <ModOp GUID="123" Merge="RangeOne"
             Condition="RangeOne &lt; $range">
        <RangeOne><ModValue Insert="$range"/></RangeOne>
      </ModOp>
      <ModOp GUID="123" Merge="RangeTwo"
             Condition="RangeTwo &lt; $range">
        <RangeTwo><ModValue Insert="$range"/></RangeTwo>
      </ModOp>
      <ModOp GUID="123" Merge="Amount">
        <Amount><ModValue Insert="self::node() * $factor"/><!-- (2)! --></Amount>
      </ModOp>
    </ModOps>
    ```

    1.  `$range` (10) is bigger than `RangeOne` (30). This ModOp is not applied.

    2.  Read more about [calculations](./modops/content.md#insert-calculations).
=== ":material-cog-outline: Options"
    ```json
    {
      "mod-id": {
        "range": "10",
        "factor": "2"
      }
    }
    ```
=== ":material-file-code-outline: Input"
    ```xml hl_lines="5-7"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <RangeOne>30</RangeOne>
        <RangeTwo>5</RangeTwo>
        <Amount>3</Amount>
      </Values>
    ```
=== ":material-xml: Result"
    ```diff hl_lines="5-9"
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <RangeOne>30</RangeOne>
    -   <RangeTwo>5</RangeTwo>
    -   <Amount>3</Amount>
    +   <RangeTwo>10</RangeTwo>
    +   <Amount>6</Amount>
      </Values>
    ```

!!! info "`<>` are not valid characters in attributes. Use `&lt;` for `<` and `&gt;` for `>` instead."

## Additional Modinfos

{{a117}}

Specify user customizable options of a mod (like iModYourAnno tweaks) in `modinfo.json`:

=== ":material-pillar: 117"
    ```json
    {
      // ..
      "Options": {
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
    The rest is information for valid values and descriptions to be used by mod managers modifying the user values in `active-options.jsonc`.

