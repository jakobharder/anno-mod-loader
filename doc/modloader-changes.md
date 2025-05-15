# ModLoader Changes for Anno 117

- [ModOp Basics](#modop-basics)
- [Variables](#variables)
- [Lists and Flags](#lists-and-flags)
- [XPath](#xpath)
- [FileOps](#fileops)

## ModOp Basics

The changes are backwards compatible to the modloader used in Anno 1800.
All new features are on top.

### Shorter ModOps

ModOps can be shortened with `Merge=<XPath>` instead of the old long form `Type="merge" Path=<XPath>`.

That new shorter way also skips `/Values` of the XPath by default when using `GUID` lookup.

```xml
<ModOp GUID="1010372" Merge="Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>

<!-- alternatively using @ notation -->
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

<ModOp Type="replace" GUID="123">
  <Template>Icon</Template>
</ModOp>
```
</details>

### ModOp Types

Short | Legacy | Comment
--- | --- | ---
`Add`|`Type="add"`|new: also adds full assets like `addNextSibling` + `GUID`
`Remove`|`Type="remove"`|as is
`Append`|`Type="addNextSibling"`|renamed
`Prepend`|`Type="addPrevSibling"`|renamed
`Replace`|`Type="replace`|as is
`Merge`|`Type="merge"`|new: improved flags and list support

### Add Assets without GUID

The fastest way to add assets is to use `add` without `GUID` or `Path`.

Note: BaseAssetGUID order is not considered (yet).

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

## Variables

Mod checks are now proper XPath elements by using `$mod-id`.
That means you can combine them like XPath expressions with `and` and `or`.
E.g. `Condition="$mod-a or $mod-b"`.
`#mod-id` is not supported anymore, use `$mod-id` instead.

External variables can also be passed using the same mechanism as `$mod-id` condition checks.
Additionally, variables can be used as content for `ModValue`.

The format is `$mod-id.option-name`.
You can leave out `mod-id` if the ovariable comes from the same mod shortening the path to `$option-name`.

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
