# ModLoader Changes for Anno 117

The changes are backwards compatible to the modloader used in Anno 1800.
All new features are provided on top.

- [ModOp Basics](#modop-basics)
- [ModOp Paths](#modop-paths)
- [Inline ModOps](#inline-modops)

## ModOp Basics

There are two top-level types of ModOps:

- `ModOp`: normal operations
- `Group`/`Include`: group operations

Additional, there are two operations to be used within a `ModOp`:

- `ModItem`: change merge behavior of individual list items
- `ModValue`: insert or modify individual values within a `ModOp`.

### Short ModOps

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
`Assets`|`Type="add"`|Similar to `addNextSibling` + `GUID` without `Path`.
`Add`|`Type="add"`|Unchanged, except when used without `Path`, `GUID` and `Property` it acts like `Assets`.
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

## ModOp Paths

### XPath

XPath 1.0 functions like `count()` and `number()` are fully supported now.

#### Example: add a number

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

### ModIDs

Conditions allowed to check if a another mod is loaded.

These `#mod-id` expressions are now expanded to `true()` and `false()` to be used within XPath.
That means you can combine them like XPath expressions with `and` and `or`.

```xml
<ModOps>
  <Group Condition="#mod-a and not(#mod-b)">
    <!-- do things -->
  </Group>
</ModOps>
```

### Options

Additionally, options can be defined in an external `options.json` file.
These options are accessible as `$mod-id.option-name` in XPath.
You can leave out `mod-id` if the variable comes from the same mod shortening the path to `$option-name`.

```xml
<ModOps>
  <ModOp Replace="@123/Costs/Influence" Condition="$use-influence">
    <Influence>3</Influence>
  </ModOp>
  <ModOp Merge="@123/Service/PublicServiceRange" Condition="$other-mod.range &lt; 20">
    <PublicServiceRange>20</PublicServiceRange>
  </ModOp>
</ModOps>
```

#### Options file

The `options.json` file is read from the `mods/` folder with the following format:

```json
{
  "mod-id": {
    "range": "10",
    "useInfluence": "true"
  }
}
```

*Note: future versions will generate the options.json from default values, but for now you have to create it yourself.*

#### Modinfo contains defaults

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

*Note: The mod loader only requires defaults. Additional information like labels, allowed values, type, steps, etc. is not relevant here and thus omitted.*

## Inline ModOps

### Merge Flags - `ModValue`

Use `<ModValue Merge="Your;Flags" />` to insert one or more flags if not already present, instead of overwriting the existing flags value.
Similarily use `Remove` to remove flags.

```xml
<ModOp Merge="@114365/Product">
  <AssociatedRegion><ModValue Merge="Moderate" /></AssociatedRegion>
</ModOp>
```

<details>
<summary>The old way</summary>

```xml
<ModOp Type="add" GUID="114365"
  Condition="!/Values/Product/AssociatedRegion[contains(text(), 'Moderate')]"
  Path="/Values/Product/AssociatedRegion">;Moderate</ModOp>
```
</details>

### Insert Options - `ModValue`

```xml
<ModOps>
  <ModOp Merge="@123">
    <PublicServiceRange><ModValue Insert="$other-mod.range"/></PublicServiceRange>
  </ModOp>
</ModOps>
```

### Insert Calculations - `ModValue`

```xml
<!-- addition -->
<ModOp Property="Maintenance" Merge="Workforce">
  <Workforce><ModValue Insert="number(.) + 10" /></Workforce>
</ModOp>

<!-- division -->
<ModOp Property="Storage" Merge="Amount">
  <Amount><ModValue Insert="(number(.) - number(.) mod 2) div 2" /></Amount>
</ModOp>
```

Available operators: `+`, `-`, `*`, `div`, `mod`

### Merge Items - `ModItem`

The default list behavior of `merge` replaces `<Item>` in the same order as listed in the patch.

Use `<ModItem Merge="Attribute">` to merge items out of order, or add when there's no match.
The item is merged with the first item that matches the attribute in `Merge`.

In the rare even you want to change the attribute itself in the merge process use `<ModItem Merge="Attribute='Value'">` to select the item.

```xml
<ModOp GUID="114365" Merge="Product">
  <ProductionRegions>
    <ModItem Merge="RegionType">
      <RegionType>Moderate</RegionType>
    </ModItem>
  </ProductionRegions>
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
```
</details>

#### Append Behavior

By default, missing items will be added to the end of the item list.
You can change that by defining a `Append` or `Prepend` path.

The default is the same as `Append='last()'`.

```xml
<ModOp Type="merge" Path="@502017/ProductList/List">
  <ModItem Merge="Product" Append="Product='1010200'">
    <Product>1500010836</Product>
  </ModItem>
</ModOp>
```

<details>
<summary>The old way</summary>

```xml
<ModOp Type="addNextSibling" GUID="502017"
  Condition="!~/Values/ProductList/List/Item[Product='1500010836']"
  Path="/Values/ProductList/List/Item[Product='1010200']">
  <Item>
    <Product>1500010836</Product>
  </Item>
</ModOp>
```
</details>
