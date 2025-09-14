# New Features Anno 117

!!! info "Go to [important changes](./changes-117.md) for the full list."
    This page only describes new features that have not been incorporated in the rest of the documentation yet.

## ModOp Basics

### ModOp Types

Short | Legacy | Comment
--- | --- | ---
`Asset`|  |Similar to `addNextSibling` + `GUID` without `Path`.
`Add`|`Type="add"`|unchanged
`Append`|`Type="addNextSibling"`|unchanged
`Prepend`|`Type="addPrevSibling"`|unchanged
`Merge`|`Type="merge"`|Includes improved flags and list handling.
`Replace`|`Type="replace`|unchanged
`Remove`|`Type="remove"`|unchanged

### Property Lookup

You can use a property name for faster lookup and more readable code.
It will select `Values/<Property>/` of all assets containing that property.

Note: this will not select assets, that have the property in `templates.xml` but not in `assets.xml` or only in their base asset.

=== ":material-pillar: 117"
    ```xml
    <ModOp Property="ModuleOwner" Merge=".[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="merge" Path="//ModuleOwner/[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```

!!! note "Intended for future performance optimizations"

## ModOp Paths

### XPath

XPath 1.0 functions like `count()` and `number()` are fully supported now.

#### Example: add a number

Use `number()` to add to a number instead of replacing it.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="1010343"
      Replace="Residence7/ResidentMax"
      Content="number(Residence7/ResidentMax) + 2">
      <ResidentMax><ModOpContent /></ResidentMax>
    </ModOp>
    ```

=== ":material-animation-outline: 117 & 1800 ⚠️"
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
