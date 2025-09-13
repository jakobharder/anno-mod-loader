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

## How to Write a ModOp

Look up and select the XML node you want to edit with XPath using the Path argument.

  ```xml
  <ModOp Path="/Templates/Group[Name='Objects']/Template">
  ```

  For the assets file, you can also use the GUID argument.

  ```xml
  <!-- standard XPath way -->
  <ModOp Path="//Asset[Values/Standard/GUID='1137']/Values/Standard/Name">

  <!-- with GUID helper -->
  <ModOp GUID="1337" Path="/Values/Standard/Name">
  ```
