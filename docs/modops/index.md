# Get Started with ModOps

## Basic Structure

!!! tip "Templates in Visual Studio Code"
    Skip the following steps by using a template.

    Press key `F1` in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template`.

1. Create a mod folder
2. Add a patch XML file

    The patch file must be in the same location as target file.

    === "117"
        `data\base\config\export\assets.xml`
    === "1800"
        `data\config\export\main\asset\assets.xml`

3. Add the following empty structure
    ```xml
    <ModOps>
      <ModOp>
        <!-- content -->
      </ModOp>
    </ModOps>
    ```

    You can add as many `<ModOp>` as needed.

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

- Short: type as path attribute `<type>="<path>"` (available since Anno 117)
- Legacy: extra type attribute `Type="<type>"`

=== "117 (short)"
    ```xml
    <!-- remove Standard/Name of asset 1337 -->
    <ModOp GUID="1337" Remove="Standard/Name" />
    ```
=== "117 & 1800"
    ```xml
    <!-- remove Standard/Name of asset 1337 -->
    <ModOp GUID="1337" Type="Remove" Path="/Values/Standard/Name" />
    ```

!!! warning "Short ModOps skip the `/Values/` part of the path"

    If you want to select the `Asset` node you need to use `../` as the path.

    === "117 (short)"
        ```xml
        <!-- removes Values/ child of asset 1337 -->
        <ModOp GUID="1337" Remove="" />
        ```

        ```xml
        <!-- removes asset 1337 -->
        <ModOp GUID="1337" Remove="../" />
        ```
    === "Legacy"
        ```xml
        <!-- removes Values/ child of asset 1337 -->
        <ModOp GUID="1337" Type="Remove" Path="/Values/" />
        ```

        ```xml
        <!-- removes asset 1337 -->
        <ModOp GUID="1337" Type="Remove" Path="" />
        ```

