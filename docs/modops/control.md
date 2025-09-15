# Groups and Conditions

## Group

{{all}}

You can group multiple ModOps into one group with `Group`.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOps>
      <Group>
        <ModOp />
        <ModOp />
      </Group>
    </ModOps>
    ```

Available attributes: [`Condition`](#condition), [`Skip`](#skip), [`MaxRepeat`](#loop)

## Include

{{all}}

`Include` behaves the same as `Group` except its content is loaded from another XML file.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOps>
      <Include File="feature.include.xml" />
    </ModOps>
    ```

Use the extension `.include.xml` to ensure the file is treated as an include file in tools and the game.

Available attributes: [`Condition`](#condition), [`Skip`](#skip)

??? info "Includes from other folders"

    The file path is relative to the file your including from.
    Use `../` to move up one folder.

    Start the path with `/` to specify a path relative to the mod root folder.

## Condition

{{all}}

ModOps are only applied when their `Condition` attribute matches to a non-empty list or to `true()`.
Conditions can be used with all ModOps: `ModOp`, `Group`, `Include` and `Asset`.

=== ":material-animation-outline: XPath"
    ```xml
    <Group Condition="//Values[Standard/GUID='1500010714']">
      <!-- Apply ModOps if asset 1500010714 exists. -->
    </Group>
    ```
    ```xml
    <Group Condition="!//Values[Standard/GUID='1500010714']">
      <!-- Apply ModOps if 1500010714 does NOT exist. -->
    </Group>
    ```
    Any valid XPath is allowed.
=== ":material-at: GUID"
    ```xml
    <Group Condition="@1500010714">
      <!-- Apply ModOps if asset 1500010714 exists. -->
    </Group>
    ```
    ```xml
    <Group Condition="!@1500010714">
      <!-- Apply ModOps if 1500010714 does NOT exist. -->
    </Group>
    ```
    `@GUID` short for `//Values[Standard/GUID='<GUID>']` only works at the start of an XPath.

??? info "Relative condition"

    Conditions can be relative to the `GUID` your ModOp is operating on.

    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="800111" Add="Property/List"
               Condition="!Property/List[Item/Product='1500010120']">
          <Item>
            <Product>1500010120</Product>
          </Item>
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="800111" Type="add" Path="/Values/Property/List"
               Condition="!/Values/Property/List[Item/Product='1500010120']">
          <Item>
            <Product>1500010120</Product>
          </Item>
        </ModOp>
        ```

    !!! warning "Be aware of condition path difference"
        When using short ModOp style also the Condition path skips `/Values/`.

??? tip "Prefer negative conditions."

    The following two ModOps will lead to the same result, but with a difference.

    === ":material-animation-outline: 117 & 1800"
        ```xml
        <!-- negative condition -->
        <ModOp Type="add" GUID="800111"
               Condition="!/Values/Feature/Options[Item/Product='1500010120']"
          Path="/Values/Feature/Options/FeedOptions" />

        <!-- positive condition -->
        <ModOp Type="add" GUID="800111"
               Condition="/Values/Feature/Options[not(Item/Product='1500010120')]"
          Path="/Values/Feature/Options" />
        ```

    The negative condition is true and results in log warnings, if GUID `800111` is missing or `Feature` has been removed from it.

    On the other hand, the positive condition skips the ModOp without any warning in the same situation.

## Loop

{{a117r}}

You can repeat ModOps until a `Condition` doesn't match anymore with setting `MaxRepeat`.
The default `MaxRepeat=1` behaves like a normal `Group`.

=== ":material-pillar: 117"
    ```xml
    <Group Condition="@123/List/Item" MaxRepeat="10">
      <!-- ..  -->
    </Group>
    ```

## ModID Condition

{{all}}

Conditions can check if a mod is active.
The condition does not consider mod loadering order.
Use `LoadAfterIds` to ensure it's loaded before your mod.

=== ":material-pound: ModID"
    ```xml
    <Group Condition="#mod-id">
      <!-- Apply ModOps if `mod-id` is active. -->
    </Group>
    ```
    ```xml
    <Group Condition="!#mod-id">
      <!-- Apply ModOps if `mod-id` is NOT active. -->
    </Group>
    ```

??? info "Check multiple ModIDs {{a117}}"
    Since Anno 117, `#mod-id` expressions are expanded to `true()` and `false()` to be used within XPath.
    That means you can combine them like XPath expressions with `and` and `or`.

    === ":material-pillar: 117"
        ```xml
        <Group Condition="#mod-a and not(#mod-b)">
          <!-- ... -->
        </Group>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <Group Condition="#mod-a">
          <Group Condition="!#mod-b">
            <!-- ... -->
          </Group>
        </Group>
        ```

## Disable Match Warning

{{all}}

The attribute `AllowNoMatch` disables match not found warnings.
That is useful if you have a complex or expensive path and don't want to repeat it as a condition.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="replace" AllowNoMatch="1"
           Path="//EffectTargets/Item[GUID='1010346' and (../../../../Template='TownhallBuff')]">
      <Item>
        <GUID>193861</GUID>
      </Item>
    </ModOp>
    ```

!!! warning "Use AllowNoMatch sparingly as it hides all warnings."

??? warning "`AllowNoMatch` can be slow when there's no match. {{a1800}}"

    This only applies to Anno 1800. In Anno 117, `AllowNoMatch` is as fast as `Condition`.

## Skip

{{all}}

!!! info "You only need this feature for iModYourAnno tweaks."

The attribute `Skip` can be used with `Group` and `Include` to ignore that operation.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOps>
      <Include File="/feature.include.xml" Skip="1" />
    </ModOps>
    ```

Note: the skip happens when the attribute `Skip` is present.
It doesn't matter if you write `Skip="1"`, `Skip="True"` or even `Skip="0"` - all of them lead to skipping the include.