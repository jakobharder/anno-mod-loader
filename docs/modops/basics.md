# Basic ModOps

## Replace

Replaces the selected element.

=== "117 (short)"
    ```xml
    <ModOp Replace="@123/Standard/Name">
      <Name>new name</Name>
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace"
           Path="/Values/Standard">
      <Name>new name</Name>
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>old name</Name>
    +     <Name>new name</Name>
        </Standard>
        <Cost />
      </Values>
    </Asset>
    ```

Replace is not limited to single elements.
Elements containing elements can be replaced as well.

=== "117 (short)"
    ```xml
    <ModOp Replace="@123/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace" Path="/Values/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
    -     <GUID>123</GUID>
    -     <Name>name</Name>
    +     <GUID>456</GUID>
    +     <Description>description</Description>
        </Standard>
        <Cost />
      </Values>
    </Asset>
    ```

??? note "An empty replace is the same as remove."
    === "Replace (short)"
        ```xml
        <ModOp Replace="@123/Cost" />
        ```

        Applies to all versions.

    === "Remove (short)"
        ```xml
        <ModOp Remove="@123/Cost" />
        ```

        Applies to all versions.

    === "Result"
        ```diff
        <Asset>
          <Values>
            <Standard>
              <GUID>123</GUID>
            </Standard>
        -   <Cost />
          </Values>
        </Asset>
        ```

## Add

Adds the content at the end insider of the selection.

=== "117 (short)"
    ```xml
    <ModOp Add="@123">
      <Maintenance />
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="add" Path="/Values">
      <Maintenance />
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <Cost />
    +   <Maintenance />
      </Values>
    </Asset>
    ```

`Add` does not check if an element already exists.
Use [Merge](#merge) or [Conditions](./conditions.md) for that purpose.

## Merge

Adds the content, or replaces it if it already exists.

=== "117 (short)"
    ```xml
    <ModOp Merge="@123/Standard">
      <Name>Hello</Name>
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="merge"
           Path="/Values/Standard">
      <Name>Hello</Name>
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>some name</Name>
    +     <Name>Hello</Name>
        </Standard>
        <Cost />
      </Values>
    </Asset>
    ```

!!! warning "Special case: element name = parent name"
    `Add` works inside of a node, `Replace` with the node itself.
    `Merge` does both based on context, but in case the child name and the parent name are the same it always works on the node like `Replace`.

    === "Problem"
        ```xml hl_lines="1 3"
          <Text>
            <LineId>123</LineId>
            <Text>old text</Text>
          </Text>
        ```

    === "117"
        ```xml
        <ModOp Merge="@123/Text">
          <Text>
            <Text>new text</Text>
          </Text>
        </ModOp>
        ```

    === "Result"
          ```diff
          <Text>
            <LineId>123</LineId>
          -   <Text>old text</Text>
          +   <Text>new text</Text>
          </Text>
          ```

## Append

Adds the content after the selection.

=== "117 (short)"
    ```xml
    <ModOp Append="@123/Standard">
      <Maintenance />
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    +   <Maintenance />
        <Cost />
      </Values>
    </Asset>
    ```

A common situation is when you want to insert an item at a specific position.

=== "117 (short)"
    ```xml
    <ModOp Append="@123/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling"
           Path="/Values/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <ConstructionCategory>
          <BuildingList>
            <Item>
              <Building>1000178</Building>
            </Item>
    +       <Item>
    +         <Building>123</Building>
    +       </Item>
            <Item>
              <Building>1010372</Building>
            </Item>
          </BuildingList>
        </ConstructionCategory>
      </Values>
    </Asset>
    ```

## Prepend

Prepend works the same as `Append` except that it adds the content before the selection.

=== "117 (short)"
    ```xml
    <ModOp Prepend="@123/Standard">
      <Maintenance />
    </ModOp>
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addPreviousSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
    +   <Maintenance />
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <Cost />
      </Values>
    </Asset>
    ```

## Remove

Removes the selected elements.

=== "117 (short)"
    ```xml
    <ModOp Remove="@123/Cost" />
    ```

=== "117 & 1800"
    ```xml
    <ModOp GUID="123" Type="remove" Path="/Values/Cost" />
    ```

=== "Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    -   <Cost />
      </Values>
    </Asset>
    ```
