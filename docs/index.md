# Test

> [!NOTE]
> This is a fork of the original mod loader of the great [meow](https://github.com/xforce/anno1800-mod-loader).
> Many thanks to him for creating and maintaining that project for so long.

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

=== "1800"
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