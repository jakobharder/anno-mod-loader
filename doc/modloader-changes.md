# ModLoader Changes for Anno 117

The changes are backwards compatible to the modloader used in Anno 1800.
All new features are on top.

- [ModOp Basics](#modop-basics)
- [Options](#options)
- [Inline ModOps](#inline-modops)
- [XPath](#xpath)

## ModOp Basics

### Shorter ModOps

ModOps can be shortened with `Merge=<XPath>` instead of the old long form `Type="merge" Path=<XPath>`.

When using `GUID` lookup with the short style skips `/Values` like the previously introduced `@GUID` notation.

```xml
<ModOp GUID="1010372" Merge="Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>

<ModOp Merge="@1010372/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>

<ModOp GUID="123" Replace="../Template">
  <Template>Icon</Template>
</ModOp>
```

<details>
<summary>The old way</summary>

```xml
<ModOp Type="merge" GUID="1010372" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>

<ModOp Type="merge" Path="@1010372/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>

<ModOp Type="replace" GUID="123">
  <Template>Icon</Template>
</ModOp>
```
</details>

### ModOp Types

Short | Legacy | Comment
--- | --- | ---
`Assets`| |Similar to `addNextSibling` + `GUID` without `Path`.
`Add`|`Type="add"`|Same as `Assets` when used without `Path`, `GUID` and `Property`. Otherwise unchanged.
`Remove`|`Type="remove"`|unchanged
`Append`|`Type="addNextSibling"`|renamed, otherwise unchanged
`Prepend`|`Type="addPrevSibling"`|renamed, otherwise unchanged
`Replace`|`Type="replace`|unchanged
`Merge`|`Type="merge"`|Includes improved flags and list handling.

### Add Assets - `Assets`

The fastest way to add assets is to use `add` without `GUID` or `Path`.
`BaseAssetGUID` order is automatically handled.

```xml
<Assets>
  <Asset>
    <Template>Text</Template>
    <Values> <!-- .. --> </Values>
  </Asset>
</Assets>
```

### Additional Lookups

There are additional lookups to make the code faster and more readable.

- `Property` in `assets.xml`:
  access `Values/<Property>/` of all assets containing a specific property

## Options

Conditions allowed to check the existings of another mod.

```xml
<ModOps>
  <Group Condition="#other-mod">
    <!-- do things -->
  </Group>
</ModOps>
```

This system got extended to be part of XPath, and with variables that can be set in an external file.

That means you can combine them like XPath expressions with `and` and `or`.
E.g. `Condition="#mod-a or #mod-b"`.

The format for variables is `$mod-id.option-name`.
You can leave out `mod-id` if the variable comes from the same mod shortening the path to `$option-name`.

`ModValue` can be used in all ModOp types.

```xml
<ModOps>
  <!-- as condition -->
  <ModOp Replace="@123/Costs/Influence"
    Condition="$use-influence">
    <Influence>3</Influence>
  </ModOp>

  <!-- as content -->
  <ModOp Merge="@123">
    <PublicServiceRange><ModValue Path="$other-mod.range"/></PublicServiceRange>
  </ModOp>
</ModOps>
```

`<user mods folder>/options.json`

```json
{
  "#mod-id": {
    "range": "10",
    "useInfluence": "true"
  }
}
```

### Defaults

`modinfo.json`:

```json
{
  /*..*/
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

Note: The mod loader only requires defaults. Additional information like labels, allowed values, type, steps, etc. is not relevant here and thus omitted.

## Inline ModOps

### Merge Items - `ModItem`

The default list behavior of `merge` replaces `<Item>` in the same order as listed in the patch.

Use `<ModItem Merge="Attribute">` to merge items out of order, or add when there's no match.
The item is merged with the first item that matches the attribute in `Merge`.

In the rare even you want to change the attribute itself in the merge process use `<ModItem Merge="Attribute='Value'">` to select the item.

*Note: ModItem is only available within ModOp type `merge`*

### Calculate Numbers - `ModValue`

```xml
<!-- addition -->
<ModOp Property="Maintenance" Merge="Workforce">
  <Workforce><ModValue Path="number(.) + 10" /></Workforce>
</ModOp>

<!-- division -->
<ModOp Property="Storage" Merge="Amount">
  <Amount><ModValue Path="(number(.) - number(.) mod 2) div 2" /></Amount>
</ModOp>
```

### Options - `ModValue`

Available operators: `+`, `-`, `*`, `div`, `mod`

### Merge Flags - `ModFlags`

Use `<ModFlags Merge="Your;Flags" />` to insert one or more flags if not already present, instead of overwriting the existing flags value.
Similarily use `Remove` to remove flags.

*Note: ModItem is only available within ModOp type `merge`*

### Example: add a region

```xml
<ModOp GUID="114365" Merge="Product">
  <Product>
    <ProductionRegions>
      <ModItem Merge="RegionType">
        <RegionType>Moderate</RegionType>
      </ModItem>
    </ProductionRegions>
    <AssociatedRegion><ModFlags Merge="Moderate" /></AssociatedRegion>
  </Product>
</ModOp>
```

<details>
<summary>The old way</summary>

```xml
<ModOp Type="add" GUID="114365"
  Condition="!/Values/Product/ProductionRegions[Item/RegionType='Moderate']"
  Path="/Values/Product/ProductionRegions">
  <Item>
    <RegionType>Moderate</RegionType>
  </Item>
</ModOp>
<ModOp Type="add" GUID="114365"
  Condition="!/Values/Product/AssociatedRegion[contains(text(), 'Moderate')]"
  Path="/Values/Product/AssociatedRegion">;Moderate</ModOp>
```
</details>

## XPath

XPath 1.0 functions like `count()` and `number()` are fully supported now.

### Example: add a number

Use `number()` to add to a number instead of replacing it.

```xml
<ModOp GUID="1010343"
  Replace="Residence7/ResidentMax"
  Content="number(Residence7/ResidentMax) + 2">
  <ResidentMax><ModOpContent /></ResidentMax>
</ModOp>
```

<details>
<summary>The old way</summary>

```xml
<!-- number table -->
<ModOp Type="add" GUID="1010343" Path="/Values">
  <Number>1</Number>
  <!-- ... -->
  <Number>100</Number>
</ModOp>
<!-- add your value and pick from the table -->
<ModOp Type="replace" GUID="1010343"
  Path="/Values/Residence7/ResidentMax"
  Content="~/Values/Number[number(../Residence7/ResidentMax) + 2]/text()">
  <ResidentMax><ModOpContent /></ResidentMax>
</ModOp>
<!-- remove table -->
<ModOp Type="remove" GUID="1010343" Path="/Values/Number" />
```
</details>
