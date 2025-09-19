# Get Started with ModOps

## ModOps

A `ModOp` is a single patch operation that is done when the game loads its game data.
It can modify, remove or add to a file.

```xml
<ModOp Type="Replace"
       Path="/Path/To/Node">
  <!-- content -->
</ModOp>
```

The `Type` defines the kind of operation like replace, remove or add.

The `Path` defines the target node or nodes of the operation.

## Choose Type

The modification type can be specified in two ways:

- Short: type as path attribute `<type>="<path>"` {{a117}}
- Legacy: extra type attribute `Type="<type>" Path="<path>"` {{all}}

=== ":material-pillar: Short 117"
    ```xml
    <!-- remove Standard/Name of asset 1337 -->
    <ModOp GUID="1337" Remove="Standard/Name" />
    ```
=== ":material-animation-outline: Legacy 117 & 1800"
    ```xml
    <!-- remove Standard/Name of asset 1337 -->
    <ModOp GUID="1337" Type="Remove" Path="/Values/Standard/Name" />
    ```

??? warning "Short ModOps skip the `/Values/` part of the path. {{a117}}"

    If you want to select the `Asset` node you need to use `../` as the path.

    === ":material-pillar: Short"
        ```xml
        <!-- removes Values/ child of asset 1337 -->
        <ModOp GUID="1337" Remove="" />
        ```

        ```xml
        <!-- removes asset 1337 -->
        <ModOp GUID="1337" Remove="../" />
        ```
    === ":material-animation-outline: Legacy"
        ```xml
        <!-- removes Values/ child of asset 1337 -->
        <ModOp GUID="1337" Type="Remove" Path="/Values/" />
        ```

        ```xml
        <!-- removes asset 1337 -->
        <ModOp GUID="1337" Type="Remove" Path="" />
        ```

    ??? note "Short ModOps and @GUID have same path behavior."

        === ":material-pillar: Short"
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

        === ":material-animation-outline: Legacy"
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

## Select a Target

Look up and select the XML node you want to edit with XPath using the `Path` attribute.

```xml
<ModOp Path="/Templates/Group[Name='Objects']/Template">
```

!!! info "XPath can be quite powerful. Checkout the [XPath Cheatsheet](https://devhints.io/xpath) to learn more."

## Lookup Helper

Lookup helper simplify the selection path with common patterns.
They make the code more readable, and also improve loading speed.

For the assets file, you can also use the `GUID` attribute for example.

```xml
<!-- standard XPath way -->
<ModOp Path="//Asset[Values/Standard/GUID='1137']/Values/Standard/Name">

<!-- same with GUID helper -->
<ModOp GUID="1337" Path="/Values/Standard/Name">
```

Lookup | Files | XPath Equivalent
--- | --- | ---
`GUID` | Assets (`assets.xml`) | `//Asset[Values/Standard/GUID='<guid>']`
`Property` | Assets (`assets.xml`) | `//Values/<property>` {{a117}}
`GUID` | InfoTips (`export.bin`) | `//InfoTipData[Guid='<guid>']` {{a117}}
`Template` | Templates (`templates.xml`) | `//Template[Name='template']`

### Property Lookup

=== ":material-pillar: 117"
    ```xml hl_lines="1"
    <ModOp Property="ModuleOwner" Merge=".[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml hl_lines="1"
    <ModOp Type="merge" Path="//ModuleOwner/[FarmType='PlantFarm']">
      <ModuleOwner>
        <ModuleBuildRadius>20</ModuleBuildRadius>
      </ModuleOwner>
    </ModOp>
    ```

??? info "Only selects assets with the property in `assets.xml`"
    The lookup does not select assets without that property in `assets.xml`, even if it is part of their template or base asset."
