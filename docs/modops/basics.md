# Basic ModOp Types

## Replace

Replaces the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Replace="Standard/Name">
      <Name>new name</Name>
    </ModOp>
    ```

=== ":material-at: 117"
    ```xml
    <ModOp Replace="@123/Standard/Name">
      <Name>new name</Name>
    </ModOp>
    ```

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace" Path="/Values/Standard">
      <Name>new name</Name>
    </ModOp>
    ```

=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>old name</Name>
    +     <Name>new name</Name>
        </Standard>
      </Values>
    </Asset>
    ```

Replace is not limited to single elements.
Elements containing elements can be replaced as well.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Replace="Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Replace="@123/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace" Path="/Values/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```

=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
    -     <GUID>123</GUID>
    -     <Name>name</Name>
    +     <GUID>456</GUID>
    +     <Description>description</Description>
        </Standard>
      </Values>
    </Asset>
    ```

??? note "An empty replace is the same as remove."
    === ":material-at: Replace"
        ```xml
        <ModOp Replace="@123/Cost" />
        ```

        Applies to all versions.

    === ":material-at: Remove"
        ```xml
        <ModOp Remove="@123/Cost" />
        ```

        Applies to all versions.

    === ":material-xml: Result"
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

Adds the content at the end inside of the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Add="">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Add="@123">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="add" Path="/Values">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    +   <Maintenance />
      </Values>
    </Asset>
    ```

`Add` does not check if an element already exists.
Use [Merge](#merge) or [Conditions](./conditions.md) for that purpose.

## Merge

Adds the content, or replaces it if it already exists.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Merge="Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Merge="@123/Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="merge" Path="/Values/Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>old name</Name>
    +     <Name>new name</Name>
    +     <Description>desc</Description>
        </Standard>
      </Values>
    </Asset>
    ```

??? warning "You may leave out the top-level element (here `Standard`) in most situations"
    The following is shorter and has to the same result.

    === ":material-at: 117"
        ```xml
        <ModOp Merge="@123/Standard">
          <Name>new name</Name>
          <Description>desc</Description>
        </ModOp>
        ```

    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="merge" Path="/Values/Standard">
          <Name>new name</Name>
          <Description>desc</Description>
        </ModOp>
        ```

    === ":material-xml: Result"
        ```diff
        <Asset>
          <Values>
            <Standard>
              <GUID>123</GUID>
        -     <Name>old name</Name>
        +     <Name>new name</Name>
        +     <Description>desc</Description>
            </Standard>
          </Values>
        </Asset>
        ```

    Only when the selected element has a child with the same name, you must use the long way.
    In doubt, always use the long way.

    === "Problem"
        ```xml hl_lines="1 3"
          <Text>
            <LineId>123</LineId>
            <Text>old text</Text>
          </Text>
        ```
    === ":material-at: 117"
        ```xml
        <ModOp Merge="@123/Text">
          <Text>
            <Text>new text</Text>
          </Text>
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="Merge" Path="/Values/Text">
          <Text>
            <Text>new text</Text>
          </Text>
        </ModOp>
        ```
    === ":material-xml: Result"
          ```diff
          <Text>
            <LineId>123</LineId>
          -   <Text>old text</Text>
          +   <Text>new text</Text>
          </Text>
          ```

## Append

Adds the content after the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Append="Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Append="@123/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
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

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123"
           Append="ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Append="@123/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling"
           Path="/Values/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-xml: Result"
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

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Prepend="Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Prepend="@123/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addPrevSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
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

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Remove="Cost" />
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Remove="@123/Cost" />
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="remove" Path="/Values/Cost" />
    ```
=== ":material-xml: Result"
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

!!! warning "Removing an not existing element leads to warning"
    As with all ModOps, there will be a warning if the selected element path does not exist.