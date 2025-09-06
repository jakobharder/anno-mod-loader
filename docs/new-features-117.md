# New Features Anno 117

The ModOp changes are backwards compatible to the modloader used in Anno 1800.
All new features are provided on top.

- [ModOp Basics](#modop-basics)
- [ModOp Paths](#modop-paths)
- [Inline ModOps](#inline-modops)
- [Advanced Modinfos](#advanced-modinfos)
- [Options](./options.md)

## ModOp Basics

There are top-level ModOps:

- `ModOp`: normal operations
- `Group`/`Include`: group operations
- `Asset` (new): simple asset adding

Additionally, there are new inline operations to be used within a `merge` `ModOp`:

- `ModItem` (new): change merge behavior of individual list items
- `ModValue` (new): insert or modify individual values within a `ModOp`.

### Short ModOps

ModOps can be shortened with `Merge=<XPath>` instead of the old long form `Type="merge" Path=<XPath>`.

When using `GUID` lookup with the short style skips `/Values` like the previously introduced `@GUID` notation.

=== "Short"
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

=== "Legacy"
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

### ModOp Types

Short | Legacy | Comment
--- | --- | ---
`Asset`|  |Similar to `addNextSibling` + `GUID` without `Path`.
`Add`|`Type="add"`|unchanged
`Append`|`Type="addNextSibling"`|renamed, otherwise unchanged
`Prepend`|`Type="addPrevSibling"`|renamed, otherwise unchanged
`Merge`|`Type="merge"`|Includes improved flags and list handling.
`Replace`|`Type="replace`|unchanged
`Remove`|`Type="remove"`|unchanged

### Add Assets

!!! warning "Not available in demo"

The fastest way to add assets is to directly drop them into your file without using a `ModOp` at all.
`BaseAssetGUID` order is automatically handled.

=== "117"
    ```xml
    <ModOps>
      <Asset>
        <Template>Text</Template>
        <Values> <!-- .. --> </Values>
      </Asset>
      <Asset>
        <BaseAssetGUID>100780</BaseAssetGUID>
        <Values> <!-- .. --> </Values>
      </Asset>
    </ModOps>
    ```

=== "1800 ⚠️"
    ```xml
    <ModOps>
      <ModOp Type="addNextSibling" GUID="100780">
        <Asset>
          <Template>Text</Template>
          <Values> <!-- .. --> </Values>
        </Asset>
        <Asset>
          <!-- manually ensure that this comes after 100780 -->
          <BaseAssetGUID>100780</BaseAssetGUID>
          <Values> <!-- .. --> </Values>
        </Asset>
      </ModOp>
    </ModOps>
    ```

### Property Lookup

You can use a property name for faster lookup and more readable code.
It will select `Values/<Property>/` of all assets containing that property.

Note: this will not select assets, that have the property in `templates.xml` but not in `assets.xml` or only in their base asset.

=== "117"
    ```xml
    <ModOp Property="ModuleOwner" Merge=".[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```
=== "1800"
    ```xml
    <ModOp Type="merge" Path="//ModuleOwner/[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```

!!! note "Intended for future performance optimizations"

### InfoTip GUID Lookup

You can now use `GUID` and `@` for easier lookup.

=== "@"
    ```xml
    <ModOp Remove="@3762/InfoElement[SubText='-6904723732129714876']" />
    ```
=== "GUID"
    ```xml
    <ModOp GUID="3762" Remove="/InfoElement[SubText='-6904723732129714876']" />
    ```
=== "1800"
    ```xml
    <ModOp Type="remove" Path="//InfoTipData[Guid='3762']/InfoElement[SubText='-6904723732129714876']" />
    ```

## Loops

!!! warning "Not available in demo"

You can repeat ModOps until a `Condition` doesn't match anymore with setting `MaxRepeat`.
The default `MaxRepeat=1` behaves like a normal `Group`.

```xml
<Group Condition="@123/List/Item" MaxRepeat="10">
  <!--  -->
</Group>
```

## ModOp Paths

### XPath

XPath 1.0 functions like `count()` and `number()` are fully supported now.

#### Example: add a number

Use `number()` to add to a number instead of replacing it.

=== "117"
    ```xml
    <ModOp GUID="1010343"
      Replace="Residence7/ResidentMax"
      Content="number(Residence7/ResidentMax) + 2">
      <ResidentMax><ModOpContent /></ResidentMax>
    </ModOp>
    ```

=== "1800 ⚠️"
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

### Special Self Node Handling

The use of `.[Condition]` is supported at the beginning of a `Path`, e.g. `Property="ModuleOwner" Merge=".[FarmType='PlantFarm']"`.

The usage of `.` before brackets is not allowed in XPath 1.0 but in XPath 2.0.
The modloader supports it by replacing it with `self::node()[Condition]`.

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

## Inline ModOps

Inline ModOps are operators you can use inside the content of a `merge` `ModOp`.

### Merge Flags - `ModValue`

Use `<ModValue Merge="Your;Flags" />` to insert one or more flags if not already present, instead of overwriting the existing flags value.
Similarily use `Remove` to remove flags.

=== "117"
    ```xml
    <ModOp Merge="@114365/Product">
      <AssociatedRegion><ModValue Merge="Moderate" /></AssociatedRegion>
    </ModOp>
    <ModOp Merge="@114365/Product">
      <AssociatedRegion><ModValue Remove="Moderate" /></AssociatedRegion>
    </ModOp>
    ```

=== "1800 ⚠️"
    ```xml
    <ModOp Type="add" GUID="114365"
      Condition="!/Values/Product/AssociatedRegion[contains(text(), 'Moderate')]"
      Path="/Values/Product/AssociatedRegion">;Moderate</ModOp>

    <!-- a generic remove was not possible -->
    ```

### Insert Local Content

Use `<ModValue Insert="<local path>" />` to copy data from a local path without specifying `GUID`.

=== "117"
    ```xml
    <ModOp Merge="@123">
      <Inline><ModValue Insert="../Standard/GUID/text()" /></Inline>
    </ModOp>
    <ModOp Property="ItemSocketSet" Merge="../ExpeditionAttribute">
      <FluffText><ModValue Insert="../../ItemSocketSet/SetBuff/text()" /></FluffText>
    </ModOp>
    ```

=== "1800 ⚠️"
    Previously, copying local content was only possible in combination with a specific `GUID`.

    ```xml
    <ModOp Type="add" GUID="123" Content="Standard/GUID/text()">
      <GUID><ModOpContent /></GUID>
    </ModOp>
    ```

### Insert Calculations - `ModValue`

=== "117"
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

=== "1800 ❌"
    ```xml
    <!-- Not supported. -->
    ```

Available operators: `+`, `-`, `*`, `div`, `mod`

### Merge Items - `ModItem`

The default list behavior of `merge` replaces `<Item>` in the same order as listed in the patch.

Use `<ModItem Merge="Attribute">` to merge items out of order, or add when there's no match.
The item is merged with the first item that matches the attribute in `Merge`.

In the rare even you want to change the attribute itself in the merge process use `<ModItem Merge="Attribute='Value'">` to select the item.

=== "117"
    ```xml
    <ModOp GUID="114365" Merge="Product">
      <ProductionRegions>
        <ModItem Merge="RegionType">
          <RegionType>Moderate</RegionType>
        </ModItem>
      </ProductionRegions>
    </ModOp>
    ```

=== "1800"
    ```xml
    <ModOp Type="add" GUID="114365"
      Condition="!/Values/Product/ProductionRegions[Item/RegionType='Moderate']"
      Path="/Values/Product/ProductionRegions">
      <Item>
        <RegionType>Moderate</RegionType>
      </Item>
    </ModOp>
    ```

#### Append Behavior

By default, missing items will be added to the end of the item list.
You can change that by defining a `Append` or `Prepend` path.

The default is the same as `Append='last()'`.

=== "117"
    ```xml
    <ModOp Merge="@502017/ProductList/List">
      <ModItem Merge="Product" Append="Product='1010200'">
        <Product>1500010836</Product>
      </ModItem>
    </ModOp>
    ```

=== "1800"
    ```xml
    <ModOp Type="addNextSibling" GUID="502017"
      Condition="!~/Values/ProductList/List/Item[Product='1500010836']"
      Path="/Values/ProductList/List/Item[Product='1010200']">
      <Item>
        <Product>1500010836</Product>
      </Item>
    </ModOp>
    ```

### Insert with Wrapper

With `ModValue` in combination `ModItem` + `ModValueContent` you can construct lists with automatic duplicate handling.

=== "117"
    ```xml
    <!-- .. -->
    <ItemEffectTargetPool>
      <ModValue Insert="@191455/ItemEffectTargetPool/EffectTargetGUIDs/Item" />
      <ModValue Insert="@191458/ItemEffectTargetPool/EffectTargetGUIDs/Item/GUID">
        <ModItem Merge="GUID">
          <ModValueContent />
        </ModItem>
      </ModValue>
    </ItemEffectTargetPool>
    <!-- .. -->
    ```

=== "1800 ❌"
    ```xml
    <!-- Not supported. -->
    ```

#### SkipParent

Use `SkipParent` if you need to exclude the top-level parent, because you have items with multiple elements.

`SkipParent` also works with `ModOpContent`.

=== "117"
    ```xml
    <!-- .. -->
    <ItemEffectTargetPool>
      <ModValue Insert="@191455/ItemEffectTargetPool/EffectTargetGUIDs/Item" />
      <ModValue Insert="@191458/ItemEffectTargetPool/EffectTargetGUIDs/Item">
        <ModItem Merge="GUID">
          <ModValueContent SkipParent="1" />
        </ModItem>
      </ModValue>
    </ItemEffectTargetPool>
    <!-- .. -->
    ```

=== "1800 ❌"
    ```xml
    <!-- Not supported. -->
    ```

## Advanced Modinfos

### Scripts

**Experimental**: This is the first release with notable script features.
Expect changes with the next versions.

Define scripts and commands in `modinfo.json`:

```jsonc
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

### Dependencies

The format for dependencies has changed a bit.
All entries are now grouped under `Dependencies`.

```jsonc
{
  // ..
  "Dependencies": {
    "Require": [],
    "Optional": [],
    "LoadAfter": [],
    "Deprecate": [],
    "Incompatible": []
  }
}
```

|Name|Value|Effect|
|---|---|---|
|`Require`|List mods and DLCs that are required to use this mod.
|`Optional`|List mods and DLCs that enable further content in this mod.
|`LoadAfter`|List mods to load before this mod.
|`Deprecate`|List mods that are replaced by this mod.<br/>A warning will be printed in the mod-loader.log. LoadAfter and Require will use the new ID.
|`Incompatible`|List incompatible mods. A warning will be printed in the mod-loader.log

