# Get Started with ModOps

## Basic Structure

??? tip "Templates in Visual Studio Code"
    Skip the following steps by using a template.

    Press key `F1` in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template`.

1. Create a mod folder
2. Add a patch XML file

    The patch file must be in the same location as target file.

    === ":material-pillar: 117"
        ```plaintext
        data/base/config/export/assets.xml
        ```
    === ":material-factory: 1800"
        ```plaintext
        data/config/export/main/asset/assets.xml
        ```

3. Add the following empty structure

    ```xml
    <ModOps>
      <ModOp>
        <!-- Content -->
      </ModOp>
      <!-- Add as many <ModOp> as needed -->
    </ModOps>
    ```

## Select a Target

Look up and select the XML node you want to edit with XPath using the `Path` attribute.

```xml
<ModOp Path="/Templates/Group[Name='Objects']/Template">
```

For the assets file, you can also use the `GUID` attribute.

```xml
<!-- standard XPath way -->
<ModOp Path="//Asset[Values/Standard/GUID='1137']/Values/Standard/Name">

<!-- with GUID helper -->
<ModOp GUID="1337" Path="/Values/Standard/Name">
```

!!! info "XPath can be quite powerful. Checkout the [XPath Cheatsheet](https://devhints.io/xpath) to learn more."

## Choose Type

Specify the type of modification:

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