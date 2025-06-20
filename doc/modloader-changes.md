# ModLoader Changes for Anno 117

The ModOp changes are backwards compatible to the modloader used in Anno 1800.
All new features are provided on top.

There are a few changes in file structure and modinfo.json, some are new or modified, some have changed from optional to mandatory.

- [General Changes](#general-changes)
- [ModOp Basics](#modop-basics)
- [ModOp Paths](#modop-paths)
- [Inline ModOps](#inline-modops)
- [Advanced Modinfos](#advanced-modinfos)

## General Changes

### Changed Paths

The location of the main `assets.xml` has changed.

Its path is now: `data\base\config\export\assets.xml`.

### Meta Information

Only folders with a `modinfo.json` are considered a mod.
The mod loader will not load mods without it.

There are a few more informational fields in `modinfo.json`.

```json
{
  // ..
  "Difficulty": "harder",
  "RequiresNewGame": true,
  "SafeToRemove": false
}
```

|Name|Value|Effect|
|---|---|---|
|`Difficulty` (mandatory)|`cheat`|e.g. no construction costs
|.|`easier`|makes the game easier, e.g. reduced needs consumption
|.|`unchanged`|is balanced similar as the vanilla game
|.|`harder`|makes the game harder
|`RequiresNewGame`|`true` or `false`|only works with a new savegame, for example like river slots (default is `false`)
|`SafeToRemove`|`true` or `false`|can be removed from a savegame without leaving trails. For example construction menu reordering (default is `false`)

*Note: 'mandatory' means the mod loader prints an error if the entry is missing or wrong.*

## ModOp Basics

There are two top-level types of ModOps:

- `ModOp`: normal operations
- `Assets`: simple asset adding
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
<ModOps>
  <Assets>
    <Asset>
      <Template>Text</Template>
      <Values> <!-- .. --> </Values>
    </Asset>
  </Assets>
</ModOps>
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
<ModOp Merge="@502017/ProductList/List">
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

## Advanced Modinfos

### Options

Specify user customizable options of a mod (like iModYourAnno tweaks) in `modinfo.json`:

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

*Note: Only `default` is relevant for the actual mod loading process.*
*The rest is information for valid values and descriptions to be used by mod managers modifying the user values in `options.json`.*

### Scripts

Define scripts and commands in `modinfo.json`:

```json
{
  // ..
  "scripts": {
    "modules": [
      "mymod/some-script.lua"
    ],
    "Init": "SomeScript = require(\"some-script\")",
    "Tick": "SomeScript:Tick()"
  }
}
```

Be sure to use unique names to not clash with other mods!

`modules` defines where scripts are located.

Event|When
---|---
`Init`|after mod loading
`Load`|load of a save or new game
`Unload`|unload of the current game
`Tick`|meta game tick

Content of example `some-script.lua`:

```lua
local SomeScript = {}

function SomeScript.Tick()
end

return SomeScript;
```
