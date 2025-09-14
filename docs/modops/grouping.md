# ModOp Groups

## Groups

You can group multiple ModOps into one group with `Group`.

This is beneficial for readability and to use a shared `Condition`.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOps>
      <Group>
        <ModOp />
        <ModOp />
      </Group>

      <Group Condition="/XPath">
        <ModOp />
        <ModOp />
      </Group>
    </ModOps>
    ```

## Include Files

Include works same as `Group` except that it contents come from another XML file.

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOps>
      <Include File="feature.include.xml" />
    </ModOps>
    ```

Use the extension `.include.xml` to ensure the file is treated as an include file in tools and the game.

The file path is relative to the file your including from.

Start the path with `/` to specify a path relative to the mod folder.

## Loops

!!! warning "Not available in demo"

You can repeat ModOps until a `Condition` doesn't match anymore with setting `MaxRepeat`.
The default `MaxRepeat=1` behaves like a normal `Group`.

=== ":material-pillar: 117"
    ```xml
    <Group Condition="@123/List/Item" MaxRepeat="10">
      <!-- ..  -->
    </Group>
    ```
